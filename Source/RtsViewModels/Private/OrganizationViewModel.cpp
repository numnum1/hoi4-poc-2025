#include "OrganizationViewModel.h"

#include "Organizations.h"
#include "RtsFacadeSubsystem.h"

void UOrganizationViewModel::Initialize(const FOrganizationId InOrganizationId)
{
	UOrganizations* NewOrganizations = nullptr;
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		NewOrganizations = Facades->GetOrganizationsContainer();
	}

	if (Organizations == NewOrganizations && OrganizationId == InOrganizationId)
	{
		return;
	}

	Unbind();

	Organizations = NewOrganizations;

	OrganizationId = InOrganizationId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationIdValue, OrganizationId.Value);

	if (Organizations)
	{
		Organizations->OnNationSettled.AddDynamic(this, &UOrganizationViewModel::HandleOrganizationEvent);
		Organizations->OnAlignmentPublished.AddDynamic(this, &UOrganizationViewModel::HandleOrganizationEvent);
		Organizations->OnOrganizationDissolved.AddDynamic(this, &UOrganizationViewModel::HandleOrganizationEvent);
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
}

void UOrganizationViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
