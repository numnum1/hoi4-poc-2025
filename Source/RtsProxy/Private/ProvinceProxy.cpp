#include "ProvinceProxy.h"

#include "BuildingProxy.h"
#include "Buildings.h"
#include "CharacterProxy.h"
#include "Characters.h"
#include "NationProxy.h"
#include "ProvinceInfo.h"
#include "Provinces.h"
#include "RtsProxySubsystem.h"

void UProvinceProxy::Setup(URtsProxySubsystem* InProxies, const FProvinceId InProvinceId, UProvinces* InProvinces)
{
	Proxies = InProxies;
	ProvinceId = InProvinceId;
	IdValue = InProvinceId.Value;
	Provinces = InProvinces;
	BindEvents();
}

void UProvinceProxy::BindEvents()
{
	if (Provinces == nullptr)
	{
		return;
	}

	Provinces->OnOwningNationChanged.AddDynamic(this, &UProvinceProxy::HandleProvinceEvent);
	Provinces->OnProvinceDestroyed.AddDynamic(this, &UProvinceProxy::HandleProvinceEvent);
}

void UProvinceProxy::UnbindEvents()
{
	if (Provinces == nullptr)
	{
		return;
	}

	Provinces->OnOwningNationChanged.RemoveAll(this);
	Provinces->OnProvinceDestroyed.RemoveAll(this);
}

void UProvinceProxy::HandleProvinceEvent(const FProvinceId InProvinceId)
{
	if (InProvinceId == ProvinceId)
	{
		NotifyChanged();
	}
}

bool UProvinceProxy::IsAlive() const
{
	return Provinces != nullptr && Provinces->IsAlive(ProvinceId);
}

FText UProvinceProxy::GetDisplayName() const
{
	return Provinces ? Provinces->GetDisplayInfo(ProvinceId).Name : FText::GetEmpty();
}

FName UProvinceProxy::GetMapId() const
{
	return Provinces ? Provinces->GetDisplayInfo(ProvinceId).MapId : NAME_None;
}

FSlateBrush UProvinceProxy::GetPicture() const
{
	return Provinces ? Provinces->GetDisplayInfo(ProvinceId).Picture : FSlateBrush();
}

USoundBase* UProvinceProxy::GetInspectionSound() const
{
	return Provinces ? Provinces->GetDisplayInfo(ProvinceId).InspectionSound : nullptr;
}

UEcology* UProvinceProxy::GetEcology() const
{
	return Provinces ? Provinces->GetDisplayInfo(ProvinceId).Ecology : nullptr;
}

int32 UProvinceProxy::GetTimezoneOffset() const
{
	return Provinces ? Provinces->GetTimezoneOffset(ProvinceId) : 0;
}

UNationProxy* UProvinceProxy::GetOwner() const
{
	if (Provinces == nullptr || Proxies == nullptr)
	{
		return nullptr;
	}
	return Proxies->GetNation(Provinces->GetOwningNation(ProvinceId));
}

TArray<UCharacterProxy*> UProvinceProxy::GetCharacters() const
{
	TArray<UCharacterProxy*> Result;
	if (Proxies == nullptr)
	{
		return Result;
	}

	UCharacters* CharactersContainer = Proxies->GetCharactersContainer();
	if (CharactersContainer == nullptr)
	{
		return Result;
	}

	const TArray<FCharacterId> Ids = CharactersContainer->CollectCharactersInProvince(ProvinceId);
	Result.Reserve(Ids.Num());
	for (const FCharacterId Id : Ids)
	{
		if (UCharacterProxy* Character = Proxies->GetCharacter(Id))
		{
			Result.Add(Character);
		}
	}
	return Result;
}

TArray<UBuildingProxy*> UProvinceProxy::GetBuildings() const
{
	TArray<UBuildingProxy*> Result;
	if (Proxies == nullptr)
	{
		return Result;
	}

	UBuildings* BuildingsContainer = Proxies->GetBuildingsContainer();
	if (BuildingsContainer == nullptr)
	{
		return Result;
	}

	for (const FBuildingId BuildingId : BuildingsContainer->CollectBuildingsInProvince(ProvinceId))
	{
		if (UBuildingProxy* Building = Proxies->GetBuilding(BuildingId))
		{
			Result.Add(Building);
		}
	}
	return Result;
}

void UProvinceProxy::SetOwner(const UNationProxy* InNation)
{
	if (Provinces == nullptr || InNation == nullptr)
	{
		return;
	}
	Provinces->SetOwningNation(ProvinceId, InNation->GetId());
}

void UProvinceProxy::DestroyProvince()
{
	if (Provinces)
	{
		Provinces->Destroy(ProvinceId);
	}
}
