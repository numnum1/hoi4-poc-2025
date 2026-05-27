#include "Characters.h"

#include "Net/UnrealNetwork.h"

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

void UCharacters::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCharacters, Infos);
	DOREPLIFETIME(UCharacters, CurrentProvinceIndexes);
	DOREPLIFETIME(UCharacters, ScheduleCharacterIndexMapping);
	DOREPLIFETIME(UCharacters, Schedules);
	
}

void UCharacters::Setup(const TArray<FCharacterInfo>& NewInfos, const TArray<int32>& NewCurrentProvinceIndexes, const TArray<int32>& NewScheduleCharacterIndexMapping, const TArray<FSchedule>& NewSchedules)
{
	Infos = NewInfos;
	CurrentProvinceIndexes = NewCurrentProvinceIndexes;
	ScheduleCharacterIndexMapping = NewScheduleCharacterIndexMapping;
	Schedules = NewSchedules;
}
