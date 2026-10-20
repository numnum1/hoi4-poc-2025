#include "Focuses/UseOrganizationFocusTree.h"

#include "Banking.h"
#include "Departments.h"
#include "EventAsset.h"
#include "FocusExclusiveGroup.h"
#include "Focuses.h"
#include "Focuses/FocusEventOptionViewModel.h"
#include "Focuses/FocusNodeViewModel.h"
#include "Focuses/FocusTreeTabViewModel.h"
#include "Focuses/RtsFocuses.h"
#include "OrganizationFocusAsset.h"
#include "OrganizationFocusTreeAsset.h"
#include "Organizations.h"
#include "RtsGameStateComponent.h"
#include "Time.h"

void UUseOrganizationFocusTree::Initialize()
{
	UnbindFacades();

	OrganizationFocuses = FindRtsGameStateComponent<UOrganizationFocuses>(this);
	Organizations = FindRtsGameStateComponent<UOrganizations>(this);
	Departments = FindRtsGameStateComponent<UDepartments>(this);
	Banking = FindRtsGameStateComponent<UBanking>(this);
	Time = FindRtsGameStateComponent<UTime>(this);

	BindFacades();
	RefreshTree();
	RefreshEvent();
}

void UUseOrganizationFocusTree::BeginDestroy()
{
	UnbindFacades();

	Super::BeginDestroy();
}

void UUseOrganizationFocusTree::SetOrganizationId(const FOrganizationId InOrganizationId)
{
	if (OrganizationId == InOrganizationId)
	{
		return;
	}

	OrganizationId = InOrganizationId;
	UE_MVVM_SET_PROPERTY_VALUE(bHasOrganization, InOrganizationId.IsValid());

	RefreshTree();
	RefreshEvent();
}

void UUseOrganizationFocusTree::SelectTree(const int32 InTreeIndex)
{
	const TArray<UOrganizationFocusTreeAsset*> AvailableTrees = URtsFocuses::CollectAvailableTrees(OrganizationId, MakeContext());
	if (!AvailableTrees.IsValidIndex(InTreeIndex) || AvailableTrees[InTreeIndex] == ActiveTree)
	{
		return;
	}

	ActiveTree = AvailableTrees[InTreeIndex];
	RefreshTree();
}

FRtsFocusContext UUseOrganizationFocusTree::MakeContext() const
{
	FRtsFocusContext Context;
	Context.Organizations = Organizations;
	Context.Departments = Departments;
	Context.Banking = Banking;
	Context.OrganizationFocuses = OrganizationFocuses;
	Context.Now = Time ? Time->GetCurrentTime() : FDateTime();
	return Context;
}

void UUseOrganizationFocusTree::RefreshTree()
{
	const FRtsFocusContext Context = MakeContext();
	const TArray<UOrganizationFocusTreeAsset*> AvailableTrees = URtsFocuses::CollectAvailableTrees(OrganizationId, Context);

	if (!AvailableTrees.Contains(ActiveTree))
	{
		ActiveTree = AvailableTrees.IsEmpty() ? nullptr : AvailableTrees[0];
	}

	TArray<TObjectPtr<UFocusTreeTabViewModel>> NewTabs;
	for (int32 TreeIndex = 0; TreeIndex < AvailableTrees.Num(); ++TreeIndex)
	{
		if (!TabCache.IsValidIndex(TreeIndex))
		{
			TabCache.Add(NewObject<UFocusTreeTabViewModel>(this));
		}

		UFocusTreeTabViewModel* Tab = TabCache[TreeIndex];
		Tab->SetSource(TreeIndex, AvailableTrees[TreeIndex], AvailableTrees[TreeIndex] == ActiveTree);
		NewTabs.Add(Tab);
	}
	UE_MVVM_SET_PROPERTY_VALUE(Tabs, NewTabs);

	TArray<TObjectPtr<UFocusNodeViewModel>> NewNodes;
	TArray<FFocusTreeEdge> NewEdges;
	TArray<FFocusTreeExclusiveLink> NewExclusiveLinks;

	if (ActiveTree)
	{
		const TArray<TObjectPtr<UOrganizationFocusAsset>>& Focuses = ActiveTree->GetFocuses();
		const TArray<FFocusTreeLayoutCell> Cells = URtsFocuses::LayoutTree(ActiveTree);

		TArray<int32> NodeIndexOfFocusIndex;
		NodeIndexOfFocusIndex.Init(INDEX_NONE, Focuses.Num());

		for (int32 FocusIndex = 0; FocusIndex < Focuses.Num(); ++FocusIndex)
		{
			UOrganizationFocusAsset* Focus = Focuses[FocusIndex];
			if (Focus == nullptr)
			{
				continue;
			}

			TObjectPtr<UFocusNodeViewModel>& Node = NodeCache.FindOrAdd(Focus);
			if (Node == nullptr)
			{
				Node = NewObject<UFocusNodeViewModel>(this);
			}

			Node->SetSource(Focus, Cells[FocusIndex]);
			NodeIndexOfFocusIndex[FocusIndex] = NewNodes.Add(Node);
		}

		for (const FFocusTreeEdge& Edge : URtsFocuses::CollectEdges(ActiveTree))
		{
			FFocusTreeEdge NodeEdge = Edge;
			NodeEdge.FromIndex = NodeIndexOfFocusIndex[Edge.FromIndex];
			NodeEdge.ToIndex = NodeIndexOfFocusIndex[Edge.ToIndex];
			NewEdges.Add(NodeEdge);
		}

		for (const FFocusExclusiveGroup& Group : ActiveTree->GetExclusiveGroups())
		{
			TArray<int32> MemberNodes;
			for (const UOrganizationFocusAsset* Member : Group.Focuses)
			{
				const int32 FocusIndex = Focuses.IndexOfByKey(Member);
				if (FocusIndex != INDEX_NONE && NodeIndexOfFocusIndex[FocusIndex] != INDEX_NONE)
				{
					MemberNodes.Add(NodeIndexOfFocusIndex[FocusIndex]);
				}
			}

			MemberNodes.Sort([&NewNodes](const int32 A, const int32 B)
			{
				return NewNodes[A]->GetGridColumn() < NewNodes[B]->GetGridColumn();
			});

			for (int32 Member = 1; Member < MemberNodes.Num(); ++Member)
			{
				FFocusTreeExclusiveLink Link;
				Link.FirstIndex = MemberNodes[Member - 1];
				Link.SecondIndex = MemberNodes[Member];
				NewExclusiveLinks.Add(Link);
			}
		}
	}

	Edges = MoveTemp(NewEdges);
	ExclusiveLinks = MoveTemp(NewExclusiveLinks);
	UE_MVVM_SET_PROPERTY_VALUE(Nodes, NewNodes);

	RefreshNodeStatus();
}

void UUseOrganizationFocusTree::RefreshNodeStatus()
{
	const FRtsFocusContext Context = MakeContext();
	for (UFocusNodeViewModel* Node : Nodes)
	{
		const UOrganizationFocusAsset* Focus = Node->GetFocus();
		Node->SetStatus(
			URtsFocuses::GetFocusState(Focus, OrganizationId, Context),
			URtsFocuses::GetFocusStateReason(Focus, OrganizationId, Context),
			URtsFocuses::GetFocusProgress(Focus, OrganizationId, Context));
	}
}

void UUseOrganizationFocusTree::RefreshEvent()
{
	const UOrganizationFocusAsset* PendingFocus =
		OrganizationFocuses && OrganizationId.IsValid() ? OrganizationFocuses->GetPendingEventFocus(OrganizationId) : nullptr;
	const UEventAsset* Event = PendingFocus ? PendingFocus->GetEvent() : nullptr;

	UE_MVVM_SET_PROPERTY_VALUE(bHasPendingEvent, Event != nullptr);
	UE_MVVM_SET_PROPERTY_VALUE(EventTitle, Event ? Event->GetTitle() : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(EventDescription, Event ? Event->GetDescription() : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(EventImage, Event ? Event->GetImage() : FSlateBrush());

	TArray<TObjectPtr<UFocusEventOptionViewModel>> NewOptions;
	if (Event)
	{
		const TArray<FEventOption>& Options = Event->GetOptions();
		for (int32 OptionIndex = 0; OptionIndex < Options.Num(); ++OptionIndex)
		{
			if (!EventOptionCache.IsValidIndex(OptionIndex))
			{
				EventOptionCache.Add(NewObject<UFocusEventOptionViewModel>(this));
			}

			UFocusEventOptionViewModel* Option = EventOptionCache[OptionIndex];
			Option->SetSource(OptionIndex, Options[OptionIndex]);
			NewOptions.Add(Option);
		}
	}
	UE_MVVM_SET_PROPERTY_VALUE(EventOptions, NewOptions);
}

void UUseOrganizationFocusTree::HandleFocusChanged(const FOrganizationId InOrganizationId)
{
	if (InOrganizationId == OrganizationId)
	{
		RefreshNodeStatus();
	}
}

void UUseOrganizationFocusTree::HandlePendingEventChanged(const FOrganizationId InOrganizationId)
{
	if (InOrganizationId == OrganizationId)
	{
		RefreshEvent();
	}
}

void UUseOrganizationFocusTree::HandleHourElapsed(int32 InHoursElapsed)
{
	RefreshTree();
}

void UUseOrganizationFocusTree::HandleBalanceChanged(FBankAccountId InAccountId)
{
	RefreshTree();
}

void UUseOrganizationFocusTree::BindFacades()
{
	if (OrganizationFocuses)
	{
		OrganizationFocuses->OnFocusChanged.AddDynamic(this, &UUseOrganizationFocusTree::HandleFocusChanged);
		OrganizationFocuses->OnPendingEventChanged.AddDynamic(this, &UUseOrganizationFocusTree::HandlePendingEventChanged);
	}

	if (Time)
	{
		Time->OnHourElapsed.AddDynamic(this, &UUseOrganizationFocusTree::HandleHourElapsed);
	}

	if (Banking)
	{
		Banking->OnBalanceChanged.AddDynamic(this, &UUseOrganizationFocusTree::HandleBalanceChanged);
	}
}

void UUseOrganizationFocusTree::UnbindFacades()
{
	if (OrganizationFocuses)
	{
		OrganizationFocuses->OnFocusChanged.RemoveAll(this);
		OrganizationFocuses->OnPendingEventChanged.RemoveAll(this);
	}

	if (Time)
	{
		Time->OnHourElapsed.RemoveAll(this);
	}

	if (Banking)
	{
		Banking->OnBalanceChanged.RemoveAll(this);
	}
}
