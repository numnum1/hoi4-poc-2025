#include "ProvinceViewModel.h"

#include "NationInfo.h"
#include "Nations.h"
#include "Provinces.h"
#include "RtsFacadeSubsystem.h"

void UProvinceViewModel::Initialize(const FProvinceId InProvinceId)
{
	UProvinces* NewProvinces = nullptr;
	UNations* NewNations = nullptr;
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		NewProvinces = Facades->GetProvincesContainer();
		NewNations = Facades->GetNationsContainer();
	}

	if (Provinces == NewProvinces && ProvinceId == InProvinceId)
	{
		return;
	}

	Unbind();

	Provinces = NewProvinces;
	Nations = NewNations;

	ProvinceId = InProvinceId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceIdValue, ProvinceId.Value);

	if (Provinces)
	{
		Provinces->OnOwningNationChanged.AddDynamic(this, &UProvinceViewModel::HandleProvinceEvent);
		Provinces->OnProvinceDestroyed.AddDynamic(this, &UProvinceViewModel::HandleProvinceEvent);
	}

	Refresh();
}

void UProvinceViewModel::HandleProvinceEvent(const FProvinceId InProvinceId)
{
	if (InProvinceId == ProvinceId)
	{
		Refresh();
	}
}

void UProvinceViewModel::Refresh_Implementation()
{
	if (Provinces == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Provinces->IsAlive(ProvinceId));

	const FProvinceDisplayInfo DisplayInfo = Provinces->GetDisplayInfo(ProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(MapId, DisplayInfo.MapId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceName, DisplayInfo.Name);
	UE_MVVM_SET_PROPERTY_VALUE(Picture, DisplayInfo.Picture);
	UE_MVVM_SET_PROPERTY_VALUE(InspectionSound, DisplayInfo.InspectionSound);
	UE_MVVM_SET_PROPERTY_VALUE(Ecology, DisplayInfo.Ecology);

	const FNationId NewOwningNationId = Provinces->GetOwningNation(ProvinceId);
	const bool bHasOwner = Nations != nullptr && NewOwningNationId.IsValid();

	UE_MVVM_SET_PROPERTY_VALUE(OwningNationId, NewOwningNationId);
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationIdValue, NewOwningNationId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationName, bHasOwner ? Nations->GetNationInfo(NewOwningNationId).NationName : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationFlag, bHasOwner ? Nations->GetNationInfo(NewOwningNationId).Flag : FSlateNoResource());

	UE_MVVM_SET_PROPERTY_VALUE(TimezoneOffset, Provinces->GetTimezoneOffset(ProvinceId));
}

void UProvinceViewModel::Unbind()
{
	if (Provinces)
	{
		Provinces->OnOwningNationChanged.RemoveAll(this);
		Provinces->OnProvinceDestroyed.RemoveAll(this);
	}
}

void UProvinceViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
