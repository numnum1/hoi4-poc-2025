#include "OrganizationViewModel.h"

#include "Banking.h"
#include "CharacterInfo.h"
#include "Characters.h"
#include "Departments.h"
#include "DepartmentViewModel.h"
#include "MemberViewModel.h"
#include "Organizations.h"
#include "Roles.h"
#include "RtsFacadeSubsystem.h"
#include "RtsPresence.h"

void UOrganizationViewModel::Initialize(const FOrganizationId InOrganizationId)
{
	UOrganizations* NewOrganizations = nullptr;
	UCharacters* NewCharacters = nullptr;
	UDepartments* NewDepartments = nullptr;
	URoles* NewRoles = nullptr;
	UBanking* NewBanking = nullptr;
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		NewOrganizations = Facades->GetOrganizationsContainer();
		NewCharacters = Facades->GetCharactersContainer();
		NewDepartments = Facades->GetDepartmentsContainer();
		NewRoles = Facades->GetRolesContainer();
		NewBanking = Facades->GetBankingContainer();
	}

	if (Organizations == NewOrganizations && OrganizationId == InOrganizationId)
	{
		return;
	}

	Unbind();

	Organizations = NewOrganizations;
	Characters = NewCharacters;
	DepartmentsFacade = NewDepartments;
	Roles = NewRoles;
	Banking = NewBanking;

	OrganizationId = InOrganizationId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationIdValue, OrganizationId.Value);

	if (Organizations)
	{
		Organizations->OnNationSettled.AddDynamic(this, &UOrganizationViewModel::HandleOrganizationEvent);
		Organizations->OnAlignmentPublished.AddDynamic(this, &UOrganizationViewModel::HandleOrganizationEvent);
		Organizations->OnOrganizationDissolved.AddDynamic(this, &UOrganizationViewModel::HandleOrganizationEvent);
	}

	if (Characters)
	{
		Characters->OnPositionChanged.AddDynamic(this, &UOrganizationViewModel::HandleMemberEvent);
	}

	if (DepartmentsFacade)
	{
		DepartmentsFacade->OnDepartmentCreated.AddDynamic(this, &UOrganizationViewModel::HandleDepartmentEvent);
		DepartmentsFacade->OnDepartmentDissolved.AddDynamic(this, &UOrganizationViewModel::HandleDepartmentEvent);
	}

	if (Banking)
	{
		Banking->OnBalanceChanged.AddDynamic(this, &UOrganizationViewModel::HandleBalanceChanged);
	}

	Refresh();
}

void UOrganizationViewModel::HandleOrganizationEvent(const FOrganizationId InOrganizationId)
{
	if (InOrganizationId == OrganizationId)
	{
		Refresh();
	}
}

void UOrganizationViewModel::HandleMemberEvent(const FCharacterId InCharacterId)
{
	Refresh();
}

void UOrganizationViewModel::HandleDepartmentEvent(const FDepartmentId InDepartmentId)
{
	Refresh();
}

void UOrganizationViewModel::HandleBalanceChanged(const FBankAccountId InAccountId)
{
	Refresh();
}

void UOrganizationViewModel::Refresh_Implementation()
{
	if (Organizations == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Organizations->IsAlive(OrganizationId));

	const FOrganizationInfo Info = Organizations->GetOrganizationInfo(OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, Info.OrganizationName);
	UE_MVVM_SET_PROPERTY_VALUE(Logo, Info.Logo);

	const FCharacterId NewOwnerId = Organizations->GetOwnerCharacter(OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerId, NewOwnerId);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerIdValue, NewOwnerId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerName, Characters && NewOwnerId.IsValid()
		? Characters->GetDisplayInfo(NewOwnerId).CharacterName : FText::GetEmpty());

	const FOrganizationAlignment Alignment = Organizations->GetAlignment(OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(bAlignmentPublished, Alignment.bPublished);
	UE_MVVM_SET_PROPERTY_VALUE(AlignmentBranches, Alignment.Branches);
	UE_MVVM_SET_PROPERTY_VALUE(AlignmentBranchCount, Alignment.Branches.Num());

	const TArray<FNationId> NewSettledNationIds = Organizations->GetSettledNations(OrganizationId);

	TArray<int32> NewSettledNationValues;
	NewSettledNationValues.Reserve(NewSettledNationIds.Num());
	for (const FNationId Id : NewSettledNationIds)
	{
		NewSettledNationValues.Add(Id.Value);
	}

	UE_MVVM_SET_PROPERTY_VALUE(SettledNationIds, NewSettledNationIds);
	UE_MVVM_SET_PROPERTY_VALUE(SettledNationIdValues, NewSettledNationValues);
	UE_MVVM_SET_PROPERTY_VALUE(SettledNationCount, NewSettledNationIds.Num());

	// --- Members ---

	const TArray<FCharacterId> MemberIds = URtsPresence::CollectMembers(OrganizationId, Characters);

	TArray<TObjectPtr<UMemberViewModel>> NewMembers;
	NewMembers.Reserve(MemberIds.Num());

	for (const FCharacterId MemberId : MemberIds)
	{
		TObjectPtr<UMemberViewModel>& Entry = MemberCache.FindOrAdd(MemberId);
		if (Entry == nullptr)
		{
			Entry = NewObject<UMemberViewModel>(this);
		}

		const FCharacterDisplayInfo Display = Characters->GetDisplayInfo(MemberId);

		FText MemberRoleName = FText::GetEmpty();
		for (const FPosition& Position : Characters->GetPositions(MemberId))
		{
			if (Position.OrganizationId == OrganizationId)
			{
				MemberRoleName = Roles ? Roles->GetRoleName(Position.RoleId) : FText::GetEmpty();
				break;
			}
		}

		Entry->SetSource(MemberId, Display.CharacterName, Display.Portrait, MemberRoleName);
		NewMembers.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Members, NewMembers);
	UE_MVVM_SET_PROPERTY_VALUE(MemberCount, NewMembers.Num());
	UE_MVVM_SET_PROPERTY_VALUE(MemberCapacity, URtsPresence::GetCapacity(OrganizationId, DepartmentsFacade));

	// --- Departments ---

	const TArray<FDepartmentId> DepartmentIds = DepartmentsFacade ? DepartmentsFacade->CollectDepartmentsOf(OrganizationId) : TArray<FDepartmentId>();

	TArray<TObjectPtr<UDepartmentViewModel>> NewDepartments;
	NewDepartments.Reserve(DepartmentIds.Num());

	for (const FDepartmentId DeptId : DepartmentIds)
	{
		TObjectPtr<UDepartmentViewModel>& Entry = DepartmentCache.FindOrAdd(DeptId);
		if (Entry == nullptr)
		{
			Entry = NewObject<UDepartmentViewModel>(this);
		}

		Entry->SetSource(DeptId);
		NewDepartments.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Departments, NewDepartments);
	UE_MVVM_SET_PROPERTY_VALUE(DepartmentCount, NewDepartments.Num());

	// --- Banking ---

	const FBankAccountId AccountId = Banking ? Banking->FindAccountForOrganization(OrganizationId) : FBankAccountId();
	UE_MVVM_SET_PROPERTY_VALUE(Balance, Banking ? Banking->GetBalance(AccountId) : 0);
}

float UOrganizationViewModel::GetAlignmentBranch(const int32 InIndex) const
{
	return AlignmentBranches.IsValidIndex(InIndex) ? AlignmentBranches[InIndex] : 0.f;
}

FNationId UOrganizationViewModel::GetSettledNationId(const int32 InIndex) const
{
	return SettledNationIds.IsValidIndex(InIndex) ? SettledNationIds[InIndex] : FNationId();
}

void UOrganizationViewModel::Unbind()
{
	if (Organizations)
	{
		Organizations->OnNationSettled.RemoveAll(this);
		Organizations->OnAlignmentPublished.RemoveAll(this);
		Organizations->OnOrganizationDissolved.RemoveAll(this);
	}

	if (Characters)
	{
		Characters->OnPositionChanged.RemoveAll(this);
	}

	if (DepartmentsFacade)
	{
		DepartmentsFacade->OnDepartmentCreated.RemoveAll(this);
		DepartmentsFacade->OnDepartmentDissolved.RemoveAll(this);
	}

	if (Banking)
	{
		Banking->OnBalanceChanged.RemoveAll(this);
	}
}

void UOrganizationViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
