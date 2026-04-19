#include "CharacterBuild.h"

#define LOCTEXT_NAMESPACE "FCharacterBuildModule"

void FCharacterBuildModule::StartupModule()
{
    
}

void FCharacterBuildModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FCharacterBuildModule, CharacterBuild)

UCharacterBuild::UCharacterBuild()
{
	RacialLevels = {};
	JobLevels = {};
}