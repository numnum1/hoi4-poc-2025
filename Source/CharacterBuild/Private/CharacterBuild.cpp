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

int32 UCharacterBuild::ComputeStrength() const
{
	return 0;
}

int32 UCharacterBuild::ComputeDexterity() const
{
	return 0;
}

int32 UCharacterBuild::ComputeConstitution() const
{
	return 0;
}

int32 UCharacterBuild::ComputeIntelligence() const
{
	return 0;
}

int32 UCharacterBuild::ComputeWisdom() const
{
	return 0;
}

int32 UCharacterBuild::ComputeCharisma() const
{
	return 0;
}
