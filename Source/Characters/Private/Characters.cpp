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
}

void UCharacters::Setup(const TArray<FCharacterInfo>& NewInfos, const TArray<int32>& NewCurrentProvinceIndexes, const TArray<int32>& NewScheduleCharacterIndexMapping)
{
	Infos = NewInfos;
	CurrentProvinceIndexes = NewCurrentProvinceIndexes;
	ScheduleCharacterIndexMapping = NewScheduleCharacterIndexMapping;
	Roles.Init({}, Infos.Num());
}

void UCharacters::AddRole(const int32 InCharacterIndex, const int32 InAtOrganizationIndex)
{
	Roles[InCharacterIndex].OrganizationIndexes.Add(InAtOrganizationIndex);
	OnRolesChanged.Broadcast(InCharacterIndex);
}
