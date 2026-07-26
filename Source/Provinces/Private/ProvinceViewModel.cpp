#include "ProvinceViewModel.h"

#include "Provinces.h"

void UProvinceViewModel::Initialize(UProvinces* InProvinces, const FProvinceId InProvinceId)
{
	if (ProvincesContainer == InProvinces && ProvinceId == InProvinceId)
	{
		return;
	}

	Unbind();

	ProvincesContainer = InProvinces;

	ProvinceId = InProvinceId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceIdValue, ProvinceId.Value);

	if (ProvincesContainer)
	{
		ProvincesContainer->OnOwningNationChanged.AddDynamic(this, &UProvinceViewModel::HandleProvinceEvent);
		ProvincesContainer->OnProvinceDestroyed.AddDynamic(this, &UProvinceViewModel::HandleProvinceEvent);
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

void UProvinceViewModel::Refresh()
{
	if (ProvincesContainer == nullptr || !ProvinceId.IsValid())
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, ProvincesContainer->IsAlive(ProvinceId));

	const FProvinceDisplayInfo DisplayInfo = ProvincesContainer->GetDisplayInfo(ProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(MapId, DisplayInfo.MapId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceName, DisplayInfo.Name);
	UE_MVVM_SET_PROPERTY_VALUE(Picture, DisplayInfo.Picture);
	UE_MVVM_SET_PROPERTY_VALUE(InspectionSound, DisplayInfo.InspectionSound);
	UE_MVVM_SET_PROPERTY_VALUE(Ecology, DisplayInfo.Ecology);

	const FNationId NewOwningNationId = ProvincesContainer->GetOwningNation(ProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationId, NewOwningNationId);
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationIdValue, NewOwningNationId.Value);

	UE_MVVM_SET_PROPERTY_VALUE(TimezoneOffset, ProvincesContainer->GetTimezoneOffset(ProvinceId));
}

void UProvinceViewModel::Unbind()
{
	if (ProvincesContainer)
	{
		ProvincesContainer->OnOwningNationChanged.RemoveAll(this);
		ProvincesContainer->OnProvinceDestroyed.RemoveAll(this);
	}
}

void UProvinceViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
