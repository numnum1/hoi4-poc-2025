#include "Nations.h"

#include "NationEntitySubsystem.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "FNationsModule"

void FNationsModule::StartupModule()
{

}

void FNationsModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNationsModule, Nations)

UNations::UNations()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UNationEntitySubsystem* UNations::GetEntities() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UNationEntitySubsystem>() : nullptr;
}

bool UNations::IsAlive(const FNationId InNationId) const
{
	const UNationEntitySubsystem* Entities = GetEntities();
	return Entities && Entities->IsAlive(InNationId);
}

FNationInfo UNations::GetNationInfo(const FNationId InNationId) const
{
	return Infos.IsValidIndex(InNationId.Value) ? Infos[InNationId.Value] : FNationInfo();
}

FNationId UNations::Create(const FNationInfo& NewInfo)
{
	UNationEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return FNationId();
	}

	// The entity claims the id; the display row must line up with it.
	const FNationId NewId = Entities->CreateNation();
	check(NewId.Value == Infos.Num());
	Infos.Add(NewInfo);

	return NewId;
}

void UNations::Setup(const TArray<FNationInfo>& NewInfos)
{
	Infos.Reset(NewInfos.Num());

	for (const FNationInfo& Info : NewInfos)
	{
		Create(Info);
	}
}

void UNations::Destroy(const FNationId InNationId)
{
	UNationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->DestroyNation(InNationId);
	OnNationDestroyed.Broadcast(InNationId);
}
