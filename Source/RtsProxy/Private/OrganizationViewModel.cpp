#include "OrganizationViewModel.h"

#include "NationProxy.h"
#include "OrganizationProxy.h"
#include "RtsProxySubsystem.h"

void UOrganizationViewModel::Initialize(const FOrganizationId InOrganizationId)
{
	UOrganizationProxy* NewProxy = nullptr;
	if (URtsProxySubsystem* ProxySubsystem = URtsProxySubsystem::Get(this))
	{
		NewProxy = ProxySubsystem->GetOrganization(InOrganizationId);
	}

	if (Proxy == NewProxy && OrganizationId == InOrganizationId)
	{
		return;
	}

	Unbind();

	Proxy = NewProxy;

	OrganizationId = InOrganizationId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationIdValue, OrganizationId.Value);

	if (Proxy)
	{
		Proxy->OnChanged.AddDynamic(this, &UOrganizationViewModel::HandleProxyChanged);
	}

	Refresh();
}

void UOrganizationViewModel::HandleProxyChanged()
{
	Refresh();
}

void UOrganizationViewModel::Refresh_Implementation()
{
	if (Proxy == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Proxy->IsAlive());

	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, Proxy->GetDisplayName());
	UE_MVVM_SET_PROPERTY_VALUE(Logo, Proxy->GetLogo());

	const FOrganizationAlignment Alignment = Proxy->GetAlignment();
	UE_MVVM_SET_PROPERTY_VALUE(bAlignmentPublished, Alignment.bPublished);
	UE_MVVM_SET_PROPERTY_VALUE(AlignmentBranches, Alignment.Branches);
	UE_MVVM_SET_PROPERTY_VALUE(AlignmentBranchCount, Alignment.Branches.Num());

	const TArray<UNationProxy*> SettledNations = Proxy->GetSettledNations();

	TArray<FNationId> NewSettledNationIds;
	TArray<int32> NewSettledNationValues;
	NewSettledNationIds.Reserve(SettledNations.Num());
	NewSettledNationValues.Reserve(SettledNations.Num());
	for (const UNationProxy* Nation : SettledNations)
	{
		if (Nation == nullptr)
		{
			continue;
		}
		NewSettledNationIds.Add(Nation->GetId());
		NewSettledNationValues.Add(Nation->GetId().Value);
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
	if (Proxy)
	{
		Proxy->OnChanged.RemoveAll(this);
	}
}

void UOrganizationViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
