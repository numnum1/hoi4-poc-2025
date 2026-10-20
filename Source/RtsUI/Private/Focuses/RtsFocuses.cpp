#include "Focuses/RtsFocuses.h"

#include "Actions/RtsActions.h"
#include "Banking.h"
#include "Departments.h"
#include "EventAsset.h"
#include "FocusExclusiveGroup.h"
#include "FocusPrerequisiteGroup.h"
#include "Focuses.h"
#include "OrganizationFocusAsset.h"
#include "OrganizationFocusTreeAsset.h"
#include "Organizations.h"
#include "RequirementAsset.h"
#include "RolePermission.h"
#include "Time.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsFocusRules, Log, All);

#define LOCTEXT_NAMESPACE "RtsFocuses"

namespace
{
	constexpr int32 MaxRequirementDepth = 8;

	bool EvaluateRequirementAtDepth(const URequirementAsset* InRequirement, FOrganizationId InOrganizationId, const FRtsFocusContext& InContext, int32 InDepth);

	bool EvaluateCondition(const FRequirementCondition& InCondition, const FOrganizationId InOrganizationId, const FRtsFocusContext& InContext, const int32 InDepth)
	{
		switch (InCondition.Type)
		{
		case ERequirementConditionType::RCT_Nested:
			return EvaluateRequirementAtDepth(InCondition.Nested, InOrganizationId, InContext, InDepth + 1);

		case ERequirementConditionType::RCT_MinOrganizationFunds:
		{
			if (InContext.Banking == nullptr)
			{
				return false;
			}
			const FBankAccountId AccountId = InContext.Banking->FindAccountForOrganization(InOrganizationId);
			const int32 Balance = AccountId.IsValid() ? InContext.Banking->GetBalance(AccountId) : 0;
			return Balance >= InCondition.Amount;
		}

		case ERequirementConditionType::RCT_HasDepartmentFunction:
		{
			if (InContext.Departments == nullptr)
			{
				return false;
			}
			for (const FDepartmentId DepartmentId : InContext.Departments->CollectDepartmentsOf(InOrganizationId))
			{
				if (InContext.Departments->GetFunction(DepartmentId) == InCondition.Function)
				{
					return true;
				}
			}
			return false;
		}
		}

		return false;
	}

	bool EvaluateRequirementAtDepth(const URequirementAsset* InRequirement, const FOrganizationId InOrganizationId, const FRtsFocusContext& InContext, const int32 InDepth)
	{
		if (InRequirement == nullptr)
		{
			return true;
		}

		if (InDepth > MaxRequirementDepth)
		{
			UE_LOG(LogRtsFocusRules, Error, TEXT("Requirement %s nests deeper than %d levels, treated as unmet"), *InRequirement->GetName(), MaxRequirementDepth);
			return false;
		}

		const TArray<FRequirementCondition>& Conditions = InRequirement->GetConditions();
		if (Conditions.IsEmpty())
		{
			return true;
		}

		const bool bRequiresAll = InRequirement->GetMode() == ERequirementMode::RM_All;
		for (const FRequirementCondition& Condition : Conditions)
		{
			const bool bMet = EvaluateCondition(Condition, InOrganizationId, InContext, InDepth) != Condition.bNegate;
			if (bRequiresAll && !bMet)
			{
				return false;
			}
			if (!bRequiresAll && bMet)
			{
				return true;
			}
		}
		return bRequiresAll;
	}

	bool IsRunning(const FRtsFocusContext& InContext, const FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus)
	{
		return InContext.OrganizationFocuses && InContext.OrganizationFocuses->GetCurrentFocus(InOrganizationId) == InFocus;
	}

	bool ArePrerequisitesMet(const FRtsFocusContext& InContext, const FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus)
	{
		for (const FFocusPrerequisiteGroup& Group : InFocus->GetPrerequisites())
		{
			const bool bGroupMet = Group.AnyOf.ContainsByPredicate([&InContext, InOrganizationId](const UOrganizationFocusAsset* InPrerequisite)
			{
				return InContext.OrganizationFocuses->IsCompleted(InOrganizationId, InPrerequisite);
			});

			if (!bGroupMet)
			{
				return false;
			}
		}
		return true;
	}

	bool IsExcluded(const FRtsFocusContext& InContext, const FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus)
	{
		const UOrganizationFocusTreeAsset* Tree = InContext.OrganizationFocuses->FindTreeOf(InFocus);
		if (Tree == nullptr)
		{
			return false;
		}

		for (const FFocusExclusiveGroup& Group : Tree->GetExclusiveGroups())
		{
			if (!Group.Focuses.Contains(InFocus))
			{
				continue;
			}

			for (const UOrganizationFocusAsset* Member : Group.Focuses)
			{
				if (Member == nullptr || Member == InFocus)
				{
					continue;
				}

				if (InContext.OrganizationFocuses->IsCompleted(InOrganizationId, Member) || IsRunning(InContext, InOrganizationId, Member))
				{
					return true;
				}
			}
		}
		return false;
	}

	bool IsTreeAvailable(const FRtsFocusContext& InContext, const FOrganizationId InOrganizationId, const UOrganizationFocusTreeAsset* InTree)
	{
		return InTree && URtsFocuses::EvaluateRequirement(InTree->GetRequirement(), InOrganizationId, InContext);
	}

	void ApplyEffect(const FEventEffect& InEffect, const FOrganizationId InOrganizationId, const FRtsFocusContext& InContext)
	{
		switch (InEffect.Type)
		{
		case EEventEffectType::EET_ChangeOrganizationFunds:
		{
			if (InContext.Banking == nullptr)
			{
				return;
			}

			const FBankAccountId AccountId = InContext.Banking->FindAccountForOrganization(InOrganizationId);
			if (!AccountId.IsValid())
			{
				UE_LOG(LogRtsFocusRules, Warning, TEXT("Organization %d has no account, funds effect skipped"), InOrganizationId.Value);
				return;
			}

			if (InEffect.Amount > 0)
			{
				InContext.Banking->Deposit(AccountId, InEffect.Amount);
				return;
			}

			const int32 Withdrawal = FMath::Min(-InEffect.Amount, InContext.Banking->GetBalance(AccountId));
			if (Withdrawal > 0)
			{
				InContext.Banking->Withdraw(AccountId, Withdrawal);
			}
			return;
		}
		}
	}

	void ApplyOption(const FEventOption& InOption, const FOrganizationId InOrganizationId, const FRtsFocusContext& InContext)
	{
		for (const FEventEffect& Effect : InOption.Effects)
		{
			ApplyEffect(Effect, InOrganizationId, InContext);
		}
	}

	void ResolveCompletedFocus(const FRtsFocusContext& InContext, const FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InCompleted)
	{
		const UEventAsset* Event = InCompleted->GetEvent();
		if (Event == nullptr || Event->GetOptions().IsEmpty())
		{
			return;
		}

		if (Event->GetOptions().Num() == 1)
		{
			ApplyOption(Event->GetOptions()[0], InOrganizationId, InContext);
			return;
		}

		InContext.OrganizationFocuses->SetPendingEvent(InOrganizationId, InCompleted);
	}

	void CompleteCurrentFocus(const FRtsFocusContext& InContext, const FOrganizationId InOrganizationId)
	{
		if (const UOrganizationFocusAsset* Completed = InContext.OrganizationFocuses->CompleteCurrentFocus(InOrganizationId))
		{
			ResolveCompletedFocus(InContext, InOrganizationId, Completed);
		}
	}

	const UEventAsset* FindPendingEvent(const FRtsFocusContext& InContext, const FOrganizationId InOrganizationId)
	{
		const UOrganizationFocusAsset* PendingFocus = InContext.OrganizationFocuses ? InContext.OrganizationFocuses->GetPendingEventFocus(InOrganizationId) : nullptr;
		return PendingFocus ? PendingFocus->GetEvent() : nullptr;
	}

	FDateTime ComputeCompletionTime(const FRtsFocusContext& InContext, const FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus)
	{
		return InContext.OrganizationFocuses->GetStartedAt(InOrganizationId) + FTimespan::FromDays(InFocus->GetDurationDays());
	}
}

FRtsFocusContext URtsFocuses::MakeFocusContext(const FRtsActionContext& InContext)
{
	FRtsFocusContext FocusContext;
	FocusContext.Organizations = InContext.Organizations;
	FocusContext.Departments = InContext.Departments;
	FocusContext.Banking = InContext.Banking;
	FocusContext.OrganizationFocuses = InContext.OrganizationFocuses;
	FocusContext.Now = InContext.Time ? InContext.Time->GetCurrentTime() : FDateTime();
	return FocusContext;
}

bool URtsFocuses::EvaluateRequirement(const URequirementAsset* InRequirement, const FOrganizationId InOrganizationId, const FRtsFocusContext& InContext)
{
	return EvaluateRequirementAtDepth(InRequirement, InOrganizationId, InContext, 0);
}

TArray<UOrganizationFocusTreeAsset*> URtsFocuses::CollectAvailableTrees(const FOrganizationId InOrganizationId, const FRtsFocusContext& InContext)
{
	TArray<UOrganizationFocusTreeAsset*> Result;
	if (InContext.OrganizationFocuses == nullptr || !InOrganizationId.IsValid())
	{
		return Result;
	}

	for (UOrganizationFocusTreeAsset* Tree : InContext.OrganizationFocuses->GetTrees())
	{
		if (IsTreeAvailable(InContext, InOrganizationId, Tree))
		{
			Result.Add(Tree);
		}
	}
	return Result;
}

EOrganizationFocusState URtsFocuses::GetFocusState(const UOrganizationFocusAsset* InFocus, const FOrganizationId InOrganizationId, const FRtsFocusContext& InContext)
{
	if (InFocus == nullptr || InContext.OrganizationFocuses == nullptr)
	{
		return EOrganizationFocusState::OFS_Unavailable;
	}

	if (InContext.OrganizationFocuses->IsCompleted(InOrganizationId, InFocus))
	{
		return EOrganizationFocusState::OFS_Completed;
	}

	if (IsRunning(InContext, InOrganizationId, InFocus))
	{
		return EOrganizationFocusState::OFS_InProgress;
	}

	if (IsExcluded(InContext, InOrganizationId, InFocus))
	{
		return EOrganizationFocusState::OFS_Excluded;
	}

	if (!ArePrerequisitesMet(InContext, InOrganizationId, InFocus))
	{
		return EOrganizationFocusState::OFS_Locked;
	}

	if (!EvaluateRequirement(InFocus->GetRequirement(), InOrganizationId, InContext))
	{
		return EOrganizationFocusState::OFS_Unavailable;
	}

	return EOrganizationFocusState::OFS_Available;
}

FText URtsFocuses::GetFocusStateReason(const UOrganizationFocusAsset* InFocus, const FOrganizationId InOrganizationId, const FRtsFocusContext& InContext)
{
	switch (GetFocusState(InFocus, InOrganizationId, InContext))
	{
	case EOrganizationFocusState::OFS_Excluded:
		return LOCTEXT("StateExcluded", "Ruled out by another choice.");
	case EOrganizationFocusState::OFS_Locked:
		return LOCTEXT("StateLocked", "Requires earlier foci.");
	case EOrganizationFocusState::OFS_Unavailable:
		return LOCTEXT("StateUnavailable", "Conditions are not met.");
	default:
		return FText::GetEmpty();
	}
}

float URtsFocuses::GetFocusProgress(const UOrganizationFocusAsset* InFocus, const FOrganizationId InOrganizationId, const FRtsFocusContext& InContext)
{
	if (InFocus == nullptr || InContext.OrganizationFocuses == nullptr)
	{
		return 0.f;
	}

	if (InContext.OrganizationFocuses->IsCompleted(InOrganizationId, InFocus))
	{
		return 1.f;
	}

	if (!IsRunning(InContext, InOrganizationId, InFocus))
	{
		return 0.f;
	}

	if (InFocus->GetDurationDays() <= 0)
	{
		return 1.f;
	}

	const double ElapsedDays = (InContext.Now - InContext.OrganizationFocuses->GetStartedAt(InOrganizationId)).GetTotalDays();
	return FMath::Clamp(static_cast<float>(ElapsedDays / InFocus->GetDurationDays()), 0.f, 1.f);
}

TArray<FFocusTreeLayoutCell> URtsFocuses::LayoutTree(const UOrganizationFocusTreeAsset* InTree)
{
	TArray<FFocusTreeLayoutCell> Cells;
	if (InTree == nullptr)
	{
		return Cells;
	}

	const TArray<TObjectPtr<UOrganizationFocusAsset>>& Focuses = InTree->GetFocuses();
	const int32 NumFocuses = Focuses.Num();
	Cells.SetNum(NumFocuses);

	TMap<const UOrganizationFocusAsset*, int32> IndexOfFocus;
	for (int32 Index = 0; Index < NumFocuses; ++Index)
	{
		if (Focuses[Index])
		{
			IndexOfFocus.Add(Focuses[Index], Index);
		}
	}

	TArray<TArray<int32>> ParentsOf;
	ParentsOf.SetNum(NumFocuses);
	for (int32 Index = 0; Index < NumFocuses; ++Index)
	{
		if (Focuses[Index] == nullptr)
		{
			continue;
		}

		for (const FFocusPrerequisiteGroup& Group : Focuses[Index]->GetPrerequisites())
		{
			for (const UOrganizationFocusAsset* Prerequisite : Group.AnyOf)
			{
				if (const int32* ParentIndex = IndexOfFocus.Find(Prerequisite))
				{
					ParentsOf[Index].AddUnique(*ParentIndex);
				}
			}
		}
	}

	TArray<int32> Rows;
	Rows.Init(INDEX_NONE, NumFocuses);
	TArray<bool> bIsVisiting;
	bIsVisiting.Init(false, NumFocuses);

	TFunction<int32(int32)> ComputeRow = [&](const int32 InIndex) -> int32
	{
		if (Rows[InIndex] != INDEX_NONE)
		{
			return Rows[InIndex];
		}

		if (bIsVisiting[InIndex])
		{
			return 0;
		}

		bIsVisiting[InIndex] = true;
		int32 Row = 0;
		for (const int32 ParentIndex : ParentsOf[InIndex])
		{
			Row = FMath::Max(Row, ComputeRow(ParentIndex) + 1);
		}
		bIsVisiting[InIndex] = false;

		Rows[InIndex] = Row;
		return Row;
	};

	int32 MaxRow = 0;
	for (int32 Index = 0; Index < NumFocuses; ++Index)
	{
		MaxRow = FMath::Max(MaxRow, ComputeRow(Index));
	}

	TArray<float> Columns;
	Columns.Init(0.f, NumFocuses);

	for (int32 Row = 0; Row <= MaxRow; ++Row)
	{
		TArray<int32> RowMembers;
		TArray<float> Desired;
		Desired.Init(0.f, NumFocuses);

		float NextRootColumn = 0.f;
		for (int32 Index = 0; Index < NumFocuses; ++Index)
		{
			if (Rows[Index] != Row)
			{
				continue;
			}

			RowMembers.Add(Index);
			if (ParentsOf[Index].IsEmpty())
			{
				Desired[Index] = NextRootColumn;
				NextRootColumn += 1.f;
				continue;
			}

			float Sum = 0.f;
			for (const int32 ParentIndex : ParentsOf[Index])
			{
				Sum += Columns[ParentIndex];
			}
			Desired[Index] = Sum / ParentsOf[Index].Num();
		}

		RowMembers.StableSort([&Desired](const int32 A, const int32 B)
		{
			return Desired[A] < Desired[B];
		});

		float LastColumn = -TNumericLimits<float>::Max();
		int32 GroupStart = 0;
		while (GroupStart < RowMembers.Num())
		{
			int32 GroupEnd = GroupStart + 1;
			while (GroupEnd < RowMembers.Num() && FMath::IsNearlyEqual(Desired[RowMembers[GroupEnd]], Desired[RowMembers[GroupStart]]))
			{
				++GroupEnd;
			}

			const int32 GroupSize = GroupEnd - GroupStart;
			float Column = FMath::Max(Desired[RowMembers[GroupStart]] - (GroupSize - 1) * 0.5f, LastColumn + 1.f);
			for (int32 Member = GroupStart; Member < GroupEnd; ++Member)
			{
				Columns[RowMembers[Member]] = Column;
				LastColumn = Column;
				Column += 1.f;
			}

			GroupStart = GroupEnd;
		}
	}

	float MinColumn = 0.f;
	for (int32 Index = 0; Index < NumFocuses; ++Index)
	{
		MinColumn = FMath::Min(MinColumn, Columns[Index]);
	}

	for (int32 Index = 0; Index < NumFocuses; ++Index)
	{
		Cells[Index].Row = Rows[Index];
		Cells[Index].Column = Columns[Index] - MinColumn;
	}
	return Cells;
}

TArray<FFocusTreeEdge> URtsFocuses::CollectEdges(const UOrganizationFocusTreeAsset* InTree)
{
	TArray<FFocusTreeEdge> Edges;
	if (InTree == nullptr)
	{
		return Edges;
	}

	const TArray<TObjectPtr<UOrganizationFocusAsset>>& Focuses = InTree->GetFocuses();
	for (int32 ToIndex = 0; ToIndex < Focuses.Num(); ++ToIndex)
	{
		if (Focuses[ToIndex] == nullptr)
		{
			continue;
		}

		for (const FFocusPrerequisiteGroup& Group : Focuses[ToIndex]->GetPrerequisites())
		{
			for (const UOrganizationFocusAsset* Prerequisite : Group.AnyOf)
			{
				const int32 FromIndex = Focuses.IndexOfByKey(Prerequisite);
				if (FromIndex == INDEX_NONE)
				{
					continue;
				}

				FFocusTreeEdge Edge;
				Edge.FromIndex = FromIndex;
				Edge.ToIndex = ToIndex;
				Edge.bIsAlternative = Group.AnyOf.Num() > 1;
				Edges.Add(Edge);
			}
		}
	}
	return Edges;
}

FText URtsFocuses::GetStartFocusBlockedReason(const FRtsActionContext& InContext, const UOrganizationFocusAsset* InFocus)
{
	const FText ScopeReason = URtsActions::GetPositionScopeReason(InContext, static_cast<int32>(ERolePermission::RP_ChooseFocus));
	if (!ScopeReason.IsEmpty())
	{
		return ScopeReason;
	}

	if (InContext.OrganizationFocuses == nullptr || InContext.Time == nullptr)
	{
		return LOCTEXT("NoFocusLibrary", "Focus library unavailable.");
	}

	if (InFocus == nullptr)
	{
		return LOCTEXT("NoFocus", "No focus picked.");
	}

	const FRtsFocusContext FocusContext = MakeFocusContext(InContext);
	const FOrganizationId OrganizationId = InContext.OrganizationId;

	if (!IsTreeAvailable(FocusContext, OrganizationId, InContext.OrganizationFocuses->FindTreeOf(InFocus)))
	{
		return LOCTEXT("TreeUnavailable", "This focus tree is not open to your organization.");
	}

	if (InContext.OrganizationFocuses->IsCompleted(OrganizationId, InFocus))
	{
		return LOCTEXT("AlreadyCompleted", "Already completed.");
	}

	if (FindPendingEvent(FocusContext, OrganizationId) != nullptr)
	{
		return LOCTEXT("EventPending", "An event is waiting for your decision.");
	}

	if (InFocus->GetDurationDays() > 0 && InContext.OrganizationFocuses->GetCurrentFocus(OrganizationId) != nullptr)
	{
		return LOCTEXT("FocusRunning", "Another focus is in progress.");
	}

	if (!ArePrerequisitesMet(FocusContext, OrganizationId, InFocus))
	{
		return LOCTEXT("PrerequisitesMissing", "Requires earlier foci.");
	}

	if (IsExcluded(FocusContext, OrganizationId, InFocus))
	{
		return LOCTEXT("Excluded", "Ruled out by another choice.");
	}

	if (!EvaluateRequirement(InFocus->GetRequirement(), OrganizationId, FocusContext))
	{
		return LOCTEXT("RequirementUnmet", "Conditions are not met.");
	}

	return FText::GetEmpty();
}

void URtsFocuses::ExecuteStartFocus(const FRtsActionContext& InContext, const UOrganizationFocusAsset* InFocus)
{
	if (InContext.OrganizationFocuses == nullptr || InFocus == nullptr)
	{
		return;
	}

	const FRtsFocusContext FocusContext = MakeFocusContext(InContext);
	const FOrganizationId OrganizationId = InContext.OrganizationId;

	if (InFocus->GetDurationDays() > 0)
	{
		InContext.OrganizationFocuses->StartFocus(OrganizationId, InFocus, FocusContext.Now);
		return;
	}

	InContext.OrganizationFocuses->CompleteFocusImmediately(OrganizationId, InFocus);
	ResolveCompletedFocus(FocusContext, OrganizationId, InFocus);
}

FText URtsFocuses::GetChooseEventOptionBlockedReason(const FRtsActionContext& InContext, const int32 InOptionIndex)
{
	const FText ScopeReason = URtsActions::GetPositionScopeReason(InContext, static_cast<int32>(ERolePermission::RP_ChooseFocus));
	if (!ScopeReason.IsEmpty())
	{
		return ScopeReason;
	}

	if (InContext.OrganizationFocuses == nullptr)
	{
		return LOCTEXT("NoFocusLibrary", "Focus library unavailable.");
	}

	const UEventAsset* Event = FindPendingEvent(MakeFocusContext(InContext), InContext.OrganizationId);
	if (Event == nullptr)
	{
		return LOCTEXT("NoPendingEvent", "No event is waiting for a decision.");
	}

	if (!Event->GetOptions().IsValidIndex(InOptionIndex))
	{
		return LOCTEXT("InvalidOption", "That option does not exist.");
	}

	return FText::GetEmpty();
}

void URtsFocuses::ExecuteChooseEventOption(const FRtsActionContext& InContext, const int32 InOptionIndex)
{
	const FRtsFocusContext FocusContext = MakeFocusContext(InContext);
	const UEventAsset* Event = FindPendingEvent(FocusContext, InContext.OrganizationId);
	if (Event == nullptr || !Event->GetOptions().IsValidIndex(InOptionIndex))
	{
		return;
	}

	InContext.OrganizationFocuses->ClearPendingEvent(InContext.OrganizationId);
	ApplyOption(Event->GetOptions()[InOptionIndex], InContext.OrganizationId, FocusContext);
}

void URtsFocuses::AdvanceDay(const FRtsFocusContext& InContext)
{
	if (InContext.OrganizationFocuses == nullptr)
	{
		return;
	}

	for (const FOrganizationId OrganizationId : InContext.OrganizationFocuses->CollectOrganizationsWithRunningFocus())
	{
		if (InContext.Organizations && !InContext.Organizations->IsAlive(OrganizationId))
		{
			continue;
		}

		const UOrganizationFocusAsset* Focus = InContext.OrganizationFocuses->GetCurrentFocus(OrganizationId);
		if (Focus == nullptr)
		{
			continue;
		}

		if (ComputeCompletionTime(InContext, OrganizationId, Focus) <= InContext.Now)
		{
			CompleteCurrentFocus(InContext, OrganizationId);
		}
	}
}

#undef LOCTEXT_NAMESPACE
