#include "OrganizationViewModel.h"

#include "Organizations.h"

void UOrganizationViewModel::Initialize(UOrganizations* InOrganizations, const FOrganizationId InOrganizationId)
{
	if (OrganizationsContainer == InOrganizations && OrganizationId == InOrganizationId)
	{
		return;
	}

	Unbind();

	OrganizationsContainer = InOrganizations;

	OrganizationId = InOrganizationId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationIdValue, OrganizationId.Value);

	if (OrganizationsContainer)
	{
		OrganizationsContainer->OnNationSettled.AddDynamic(this, &UOrganizationViewModel::HandleOrganizationEvent);
		OrganizationsContainer->OnAlignmentPublished.AddDynamic(this, &UOrganizationViewModel::HandleOrganizationEvent);
		OrganizationsContainer->OnOrganizationDissolved.AddDynamic(this, &UOrganizationViewModel::HandleOrganizationEvent);
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

void UOrganizationViewModel::Refresh()
{
	if (OrganizationsContainer == nullptr || !OrganizationId.IsValid())
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, OrganizationsContainer->IsAlive(OrganizationId));

	const FOrganizationInfo Info = OrganizationsContainer->GetOrganizationInfo(OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, Info.OrganizationName);
	UE_MVVM_SET_PROPERTY_VALUE(Logo, Info.Logo);

	const FOrganizationAlignment Alignment = OrganizationsContainer->GetAlignment(OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(bAlignmentPublished, Alignment.bPublished);
	UE_MVVM_SET_PROPERTY_VALUE(AlignmentBranches, Alignment.Branches);
	UE_MVVM_SET_PROPERTY_VALUE(AlignmentBranchCount, Alignment.Branches.Num());

	const TArray<FNationId> NewSettledNations = OrganizationsContainer->GetSettledNations(OrganizationId);

	TArray<int32> NewSettledNationValues;
	NewSettledNationValues.Reserve(NewSettledNations.Num());
	for (const FNationId& NationId : NewSettledNations)
	{
		NewSettledNationValues.Add(NationId.Value);
	}

	UE_MVVM_SET_PROPERTY_VALUE(SettledNationIds, NewSettledNations);
	UE_MVVM_SET_PROPERTY_VALUE(SettledNationIdValues, NewSettledNationValues);
	UE_MVVM_SET_PROPERTY_VALUE(SettledNationCount, NewSettledNations.Num());
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
	if (OrganizationsContainer)
	{
		OrganizationsContainer->OnNationSettled.RemoveAll(this);
		OrganizationsContainer->OnAlignmentPublished.RemoveAll(this);
		OrganizationsContainer->OnOrganizationDissolved.RemoveAll(this);
	}
}

void UOrganizationViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
