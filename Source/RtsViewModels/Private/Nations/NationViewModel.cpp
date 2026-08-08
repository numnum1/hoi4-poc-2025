#include "Nations/NationViewModel.h"

#include "Nations.h"
#include "RtsFacadeSubsystem.h"

void UNationViewModel::Initialize(const FNationId InNationId)
{
	UNations* NewNations = nullptr;
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		NewNations = Facades->GetNationsContainer();
	}

	if (Nations == NewNations && NationId == InNationId)
	{
		return;
	}

	Unbind();

	Nations = NewNations;

	NationId = InNationId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(NationId);
	UE_MVVM_SET_PROPERTY_VALUE(NationIdValue, NationId.Value);

	if (Nations)
	{
		Nations->OnNationDestroyed.AddDynamic(this, &UNationViewModel::HandleNationEvent);
	}

	Refresh();
}

void UNationViewModel::HandleNationEvent(const FNationId InNationId)
{
	if (InNationId == NationId)
	{
		Refresh();
	}
}

void UNationViewModel::Refresh_Implementation()
{
	if (Nations == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Nations->IsAlive(NationId));

	const FNationInfo Info = Nations->GetNationInfo(NationId);
	UE_MVVM_SET_PROPERTY_VALUE(NationName, Info.NationName);
	UE_MVVM_SET_PROPERTY_VALUE(Flag, Info.Flag);
}

void UNationViewModel::Unbind()
{
	if (Nations)
	{
		Nations->OnNationDestroyed.RemoveAll(this);
	}
}

void UNationViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
