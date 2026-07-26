#include "NationProxy.h"

#include "CharacterProxy.h"
#include "NationInfo.h"
#include "Nations.h"
#include "OrganizationProxy.h"
#include "Organizations.h"
#include "ProvinceProxy.h"
#include "Provinces.h"
#include "RtsProxySubsystem.h"

void UNationProxy::Setup(URtsProxySubsystem* InProxies, const FNationId InNationId, UNations* InNations)
{
	Proxies = InProxies;
	NationId = InNationId;
	IdValue = InNationId.Value;
	Nations = InNations;
	BindEvents();
}

void UNationProxy::BindEvents()
{
	if (Nations == nullptr)
	{
		return;
	}

	Nations->OnNationDestroyed.AddDynamic(this, &UNationProxy::HandleNationEvent);
}

void UNationProxy::UnbindEvents()
{
	if (Nations == nullptr)
	{
		return;
	}

	Nations->OnNationDestroyed.RemoveAll(this);
}

void UNationProxy::HandleNationEvent(const FNationId InNationId)
{
	if (InNationId == NationId)
	{
		NotifyChanged();
	}
}

bool UNationProxy::IsAlive() const
{
	return Nations != nullptr && Nations->IsAlive(NationId);
}

FText UNationProxy::GetDisplayName() const
{
	return Nations ? Nations->GetNationInfo(NationId).NationName : FText::GetEmpty();
}

FSlateBrush UNationProxy::GetFlag() const
{
	return Nations ? Nations->GetNationInfo(NationId).Flag : FSlateBrush();
}

TArray<UProvinceProxy*> UNationProxy::GetProvinces() const
{
	TArray<UProvinceProxy*> Result;
	if (Proxies == nullptr)
	{
		return Result;
	}

	UProvinces* ProvincesContainer = Proxies->GetProvincesContainer();
	if (ProvincesContainer == nullptr)
	{
		return Result;
	}

	const TArray<FProvinceId> Ids = ProvincesContainer->CollectProvincesOfNation(NationId);
	Result.Reserve(Ids.Num());
	for (const FProvinceId Id : Ids)
	{
		if (UProvinceProxy* Province = Proxies->GetProvince(Id))
		{
			Result.Add(Province);
		}
	}
	return Result;
}

int32 UNationProxy::GetProvinceCount() const
{
	if (Proxies == nullptr)
	{
		return 0;
	}

	const UProvinces* ProvincesContainer = Proxies->GetProvincesContainer();
	return ProvincesContainer ? ProvincesContainer->CollectProvincesOfNation(NationId).Num() : 0;
}

TArray<UCharacterProxy*> UNationProxy::GetCharacters() const
{
	TArray<UCharacterProxy*> Result;
	for (const UProvinceProxy* Province : GetProvinces())
	{
		if (Province)
		{
			Result.Append(Province->GetCharacters());
		}
	}
	return Result;
}

TArray<UOrganizationProxy*> UNationProxy::GetSettledOrganizations() const
{
	TArray<UOrganizationProxy*> Result;
	if (Proxies == nullptr)
	{
		return Result;
	}

	UOrganizations* OrganizationsContainer = Proxies->GetOrganizationsContainer();
	if (OrganizationsContainer == nullptr)
	{
		return Result;
	}

	const TArray<FOrganizationId> Ids = OrganizationsContainer->CollectOrganizationsSettledIn(NationId);
	Result.Reserve(Ids.Num());
	for (const FOrganizationId Id : Ids)
	{
		if (UOrganizationProxy* Organization = Proxies->GetOrganization(Id))
		{
			Result.Add(Organization);
		}
	}
	return Result;
}

void UNationProxy::DestroyNation()
{
	if (Nations)
	{
		Nations->Destroy(NationId);
	}
}
