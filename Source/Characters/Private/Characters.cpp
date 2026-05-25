#include "Characters.h"

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
	
	SetIsReplicatedByDefault(true);
}

void UCharacters::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	InitValuesFromInfos();
}

void UCharacters::InitValuesFromInfos()
{
	// Set containers to exact sizes
	CharacterIds.Empty(Infos.Num());
	CurrentProvinces.SetNumUninitialized(Infos.Num());
	Schedules.Empty(Infos.Num());
	
	// For each info initialize fitting entry
	for (int I = 0; I < Infos.Num(); ++I)
	{
		CharacterIds.Emplace(Infos[I].Guid, I);
		CurrentProvinces[I] = Infos[I].CurrentProvince;
	}
}
