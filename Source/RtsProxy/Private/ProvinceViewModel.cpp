#include "ProvinceViewModel.h"

#include "NationProxy.h"
#include "ProvinceProxy.h"
#include "RtsProxySubsystem.h"

void UProvinceViewModel::Initialize(const FProvinceId InProvinceId)
{
	UProvinceProxy* NewProxy = nullptr;
	if (URtsProxySubsystem* ProxySubsystem = URtsProxySubsystem::Get(this))
	{
		NewProxy = ProxySubsystem->GetProvince(InProvinceId);
	}

	if (Proxy == NewProxy && ProvinceId == InProvinceId)
	{
		return;
	}

	Unbind();

	Proxy = NewProxy;

	ProvinceId = InProvinceId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceIdValue, ProvinceId.Value);

	if (Proxy)
	{
		Proxy->OnChanged.AddDynamic(this, &UProvinceViewModel::HandleProxyChanged);
	}

	Refresh();
}

void UProvinceViewModel::HandleProxyChanged()
{
	Refresh();
}

void UProvinceViewModel::Refresh()
{
	if (Proxy == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Proxy->IsAlive());

	UE_MVVM_SET_PROPERTY_VALUE(MapId, Proxy->GetMapId());
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceName, Proxy->GetDisplayName());
	UE_MVVM_SET_PROPERTY_VALUE(Picture, Proxy->GetPicture());
	UE_MVVM_SET_PROPERTY_VALUE(InspectionSound, Proxy->GetInspectionSound());
	UE_MVVM_SET_PROPERTY_VALUE(Ecology, Proxy->GetEcology());

	const UNationProxy* Owner = Proxy->GetOwner();
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationId, Owner ? Owner->GetId() : FNationId());
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationIdValue, Owner ? Owner->GetId().Value : INDEX_NONE);
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationName, Owner ? Owner->GetDisplayName() : FText::GetEmpty());

	UE_MVVM_SET_PROPERTY_VALUE(TimezoneOffset, Proxy->GetTimezoneOffset());
}

void UProvinceViewModel::Unbind()
{
	if (Proxy)
	{
		Proxy->OnChanged.RemoveAll(this);
	}
}

void UProvinceViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
