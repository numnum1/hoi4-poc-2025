#include "Characters.h"

#include "CharacterEntitySubsystem.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "FCharactersModule"

void FCharactersModule::StartupModule()
{

}

void FCharactersModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCharactersModule, Characters)

UCharacters::UCharacters()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UCharacterEntitySubsystem* UCharacters::GetEntities() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UCharacterEntitySubsystem>() : nullptr;
}

void UCharacters::SetupDisplay(const TArray<FCharacterSpawnData>& InSpawnData)
{
	Display.Reset(InSpawnData.Num());
	for (const FCharacterSpawnData& Data : InSpawnData)
	{
		Display.Add(Data.Display);
	}
}

FCharacterDisplayInfo UCharacters::GetDisplayInfo(const int32 InCharacterIndex) const
{
	return Display.IsValidIndex(InCharacterIndex) ? Display[InCharacterIndex] : FCharacterDisplayInfo();
}

int32 UCharacters::GetCurrentProvince(const int32 InCharacterIndex) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetCurrentProvince(InCharacterIndex) : INDEX_NONE;
}

FAbilities UCharacters::GetAbilities(const int32 InCharacterIndex) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetAbilities(InCharacterIndex) : FAbilities();
}

TArray<FPosition> UCharacters::GetPositions(const int32 InCharacterIndex) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetPositions(InCharacterIndex) : TArray<FPosition>();
}

FSchedule UCharacters::GetSchedule(const int32 InCharacterIndex) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetSchedule(InCharacterIndex) : FSchedule();
}

void UCharacters::AddPosition(const int32 InCharacterIndex, const int32 InAtOrganizationIndex)
{
	UCharacterEntitySubsystem* Entities = GetEntities();
	if (!Entities) return;

	Entities->AddPosition(InCharacterIndex, InAtOrganizationIndex);
	OnPositionChanged.Broadcast(InCharacterIndex);
}

void UCharacters::MoveCharacter(const int32 InCharacterIndex, const int32 NewCurrentProvinceIndex,
                                const int32 NewTimezoneOffset)
{
	UCharacterEntitySubsystem* Entities = GetEntities();
	if (!Entities) return;

	Entities->MoveCharacter(InCharacterIndex, NewCurrentProvinceIndex, NewTimezoneOffset);
	OnCurrentProvinceChanged.Broadcast(InCharacterIndex);
}
