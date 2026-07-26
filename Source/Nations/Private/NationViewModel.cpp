#include "NationViewModel.h"

#include "Nations.h"

void UNationViewModel::Initialize(UNations* InNations, const FNationId InNationId)
{
	if (NationsContainer == InNations && NationId == InNationId)
	{
		return;
	}

	Unbind();

	NationsContainer = InNations;

	NationId = InNationId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(NationId);
	UE_MVVM_SET_PROPERTY_VALUE(NationIdValue, NationId.Value);

	if (NationsContainer)
	{
		NationsContainer->OnNationDestroyed.AddDynamic(this, &UNationViewModel::HandleNationEvent);
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

void UNationViewModel::Refresh()
{
	if (NationsContainer == nullptr || !NationId.IsValid())
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, NationsContainer->IsAlive(NationId));

	const FNationInfo Info = NationsContainer->GetNationInfo(NationId);
	UE_MVVM_SET_PROPERTY_VALUE(NationName, Info.NationName);
	UE_MVVM_SET_PROPERTY_VALUE(Flag, Info.Flag);
}

void UNationViewModel::Unbind()
{
	if (NationsContainer)
	{
		NationsContainer->OnNationDestroyed.RemoveAll(this);
	}
}

void UNationViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
