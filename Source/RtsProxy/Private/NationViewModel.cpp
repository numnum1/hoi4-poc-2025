#include "NationViewModel.h"

#include "NationProxy.h"
#include "RtsProxySubsystem.h"

void UNationViewModel::Initialize(const FNationId InNationId)
{
	UNationProxy* NewProxy = nullptr;
	if (URtsProxySubsystem* ProxySubsystem = URtsProxySubsystem::Get(this))
	{
		NewProxy = ProxySubsystem->GetNation(InNationId);
	}

	if (Proxy == NewProxy && NationId == InNationId)
	{
		return;
	}

	Unbind();

	Proxy = NewProxy;

	NationId = InNationId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(NationId);
	UE_MVVM_SET_PROPERTY_VALUE(NationIdValue, NationId.Value);

	if (Proxy)
	{
		Proxy->OnChanged.AddDynamic(this, &UNationViewModel::HandleProxyChanged);
	}

	Refresh();
}

void UNationViewModel::HandleProxyChanged()
{
	Refresh();
}

void UNationViewModel::Refresh()
{
	if (Proxy == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Proxy->IsAlive());
	UE_MVVM_SET_PROPERTY_VALUE(NationName, Proxy->GetDisplayName());
	UE_MVVM_SET_PROPERTY_VALUE(Flag, Proxy->GetFlag());
}

void UNationViewModel::Unbind()
{
	if (Proxy)
	{
		Proxy->OnChanged.RemoveAll(this);
	}
}

void UNationViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
