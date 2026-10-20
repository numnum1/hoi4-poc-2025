#include "Focuses.h"

#include "Engine/World.h"
#include "FocusExclusiveGroup.h"
#include "FocusPrerequisiteGroup.h"
#include "OrganizationFocusAsset.h"
#include "OrganizationFocusEntitySubsystem.h"
#include "OrganizationFocusTreeAsset.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsFocusesFacade, Log, All);

#define LOCTEXT_NAMESPACE "FFocusesModule"

void FFocusesModule::StartupModule()
{
}

void FFocusesModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFocusesModule, Focuses)

namespace
{
	bool HasPrerequisiteCycle(const UOrganizationFocusAsset* InFocus, TSet<const UOrganizationFocusAsset*>& InVisiting, TSet<const UOrganizationFocusAsset*>& InDone)
	{
		if (InFocus == nullptr || InDone.Contains(InFocus))
		{
			return false;
		}

		if (InVisiting.Contains(InFocus))
		{
			return true;
		}

		InVisiting.Add(InFocus);
		for (const FFocusPrerequisiteGroup& Group : InFocus->GetPrerequisites())
		{
			for (const UOrganizationFocusAsset* Prerequisite : Group.AnyOf)
			{
				if (HasPrerequisiteCycle(Prerequisite, InVisiting, InDone))
				{
					return true;
				}
			}
		}
		InVisiting.Remove(InFocus);
		InDone.Add(InFocus);
		return false;
	}
}

UOrganizationFocuses::UOrganizationFocuses()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

UOrganizationFocusEntitySubsystem* UOrganizationFocuses::GetEntities() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UOrganizationFocusEntitySubsystem>() : nullptr;
}

void UOrganizationFocuses::Setup(const TArray<UOrganizationFocusTreeAsset*>& InTrees)
{
	Trees.Reset();
	FocusByPath.Reset();
	TreeByFocus.Reset();

	for (UOrganizationFocusTreeAsset* Tree : InTrees)
	{
		if (Tree == nullptr)
		{
			UE_LOG(LogRtsFocusesFacade, Warning, TEXT("Scenario lists an empty focus tree slot"));
			continue;
		}

		Trees.Add(Tree);
		for (UOrganizationFocusAsset* Focus : Tree->GetFocuses())
		{
			if (Focus == nullptr)
			{
				UE_LOG(LogRtsFocusesFacade, Warning, TEXT("Tree %s lists an empty focus slot"), *Tree->GetName());
				continue;
			}

			if (const UOrganizationFocusTreeAsset* const* OtherTree = TreeByFocus.Find(Focus))
			{
				UE_LOG(LogRtsFocusesFacade, Error, TEXT("Focus %s is listed in tree %s and in tree %s; foci are not shared, the second listing is ignored"),
					*Focus->GetName(), *(*OtherTree)->GetName(), *Tree->GetName());
				continue;
			}

			TreeByFocus.Add(Focus, Tree);
			FocusByPath.Add(FTopLevelAssetPath(Focus), Focus);
		}
	}

	for (const UOrganizationFocusTreeAsset* Tree : Trees)
	{
		ValidateTree(Tree);
	}
}

void UOrganizationFocuses::ValidateTree(const UOrganizationFocusTreeAsset* InTree) const
{
	const auto IsInTree = [this, InTree](const UOrganizationFocusAsset* InFocus)
	{
		const UOrganizationFocusTreeAsset* const* Owner = TreeByFocus.Find(InFocus);
		return Owner && *Owner == InTree;
	};

	TSet<const UOrganizationFocusAsset*> Visiting;
	TSet<const UOrganizationFocusAsset*> Done;

	for (const UOrganizationFocusAsset* Focus : InTree->GetFocuses())
	{
		if (Focus == nullptr)
		{
			continue;
		}

		for (const FFocusPrerequisiteGroup& Group : Focus->GetPrerequisites())
		{
			if (Group.AnyOf.IsEmpty())
			{
				UE_LOG(LogRtsFocusesFacade, Warning, TEXT("Focus %s in tree %s has an empty prerequisite group, which can never be met"),
					*Focus->GetName(), *InTree->GetName());
			}

			for (const UOrganizationFocusAsset* Prerequisite : Group.AnyOf)
			{
				if (!IsInTree(Prerequisite))
				{
					UE_LOG(LogRtsFocusesFacade, Error, TEXT("Focus %s in tree %s names prerequisite %s, which is not part of the same tree"),
						*Focus->GetName(), *InTree->GetName(), Prerequisite ? *Prerequisite->GetName() : TEXT("None"));
				}
			}
		}

		if (HasPrerequisiteCycle(Focus, Visiting, Done))
		{
			UE_LOG(LogRtsFocusesFacade, Error, TEXT("Tree %s has a prerequisite cycle through focus %s"), *InTree->GetName(), *Focus->GetName());
		}
	}

	for (const FFocusExclusiveGroup& Group : InTree->GetExclusiveGroups())
	{
		for (const UOrganizationFocusAsset* Member : Group.Focuses)
		{
			if (!IsInTree(Member))
			{
				UE_LOG(LogRtsFocusesFacade, Error, TEXT("Tree %s has an exclusive group member %s that is not part of the tree"),
					*InTree->GetName(), Member ? *Member->GetName() : TEXT("None"));
			}
		}
	}
}

const UOrganizationFocusAsset* UOrganizationFocuses::FindFocus(const FTopLevelAssetPath& InPath) const
{
	if (InPath.IsNull())
	{
		return nullptr;
	}

	const UOrganizationFocusAsset* const* Found = FocusByPath.Find(InPath);
	return Found ? *Found : nullptr;
}

const UOrganizationFocusTreeAsset* UOrganizationFocuses::FindTreeOf(const UOrganizationFocusAsset* InFocus) const
{
	const UOrganizationFocusTreeAsset* const* Found = TreeByFocus.Find(InFocus);
	return Found ? *Found : nullptr;
}

const UOrganizationFocusAsset* UOrganizationFocuses::GetCurrentFocus(const FOrganizationId InOrganizationId) const
{
	const UOrganizationFocusEntitySubsystem* Entities = GetEntities();
	return Entities ? FindFocus(Entities->GetCurrentFocus(InOrganizationId)) : nullptr;
}

FDateTime UOrganizationFocuses::GetStartedAt(const FOrganizationId InOrganizationId) const
{
	const UOrganizationFocusEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetStartedAt(InOrganizationId) : FDateTime();
}

bool UOrganizationFocuses::IsCompleted(const FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus) const
{
	const UOrganizationFocusEntitySubsystem* Entities = GetEntities();
	return Entities && InFocus && Entities->IsCompleted(InOrganizationId, FTopLevelAssetPath(InFocus));
}

const UOrganizationFocusAsset* UOrganizationFocuses::GetPendingEventFocus(const FOrganizationId InOrganizationId) const
{
	const UOrganizationFocusEntitySubsystem* Entities = GetEntities();
	return Entities ? FindFocus(Entities->GetPendingEventFocus(InOrganizationId)) : nullptr;
}

TArray<FOrganizationId> UOrganizationFocuses::CollectOrganizationsWithRunningFocus() const
{
	const UOrganizationFocusEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectOrganizationsWithRunningFocus() : TArray<FOrganizationId>();
}

void UOrganizationFocuses::StartFocus(const FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus, const FDateTime& InNow)
{
	UOrganizationFocusEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr || InFocus == nullptr)
	{
		return;
	}

	Entities->SetCurrentFocus(InOrganizationId, FTopLevelAssetPath(InFocus), InNow);
	OnFocusChanged.Broadcast(InOrganizationId);
}

const UOrganizationFocusAsset* UOrganizationFocuses::CompleteCurrentFocus(const FOrganizationId InOrganizationId)
{
	UOrganizationFocusEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return nullptr;
	}

	const UOrganizationFocusAsset* Completed = FindFocus(Entities->CompleteCurrentFocus(InOrganizationId));
	if (Completed)
	{
		OnFocusChanged.Broadcast(InOrganizationId);
	}
	return Completed;
}

void UOrganizationFocuses::CompleteFocusImmediately(const FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus)
{
	UOrganizationFocusEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr || InFocus == nullptr)
	{
		return;
	}

	Entities->AddCompletedFocus(InOrganizationId, FTopLevelAssetPath(InFocus));
	OnFocusChanged.Broadcast(InOrganizationId);
}

void UOrganizationFocuses::SetPendingEvent(const FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus)
{
	UOrganizationFocusEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr || InFocus == nullptr)
	{
		return;
	}

	Entities->SetPendingEventFocus(InOrganizationId, FTopLevelAssetPath(InFocus));
	OnPendingEventChanged.Broadcast(InOrganizationId);
}

void UOrganizationFocuses::ClearPendingEvent(const FOrganizationId InOrganizationId)
{
	UOrganizationFocusEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->SetPendingEventFocus(InOrganizationId, FTopLevelAssetPath());
	OnPendingEventChanged.Broadcast(InOrganizationId);
}
