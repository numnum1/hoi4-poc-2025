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
	return ComputeSkill([](const FSkillSet& T)
	{
		return T.Strength;
	});
}

int32 UCharacterBuild::ComputeDexterity() const
{
	return ComputeSkill([](const FSkillSet& T)
	{
		return T.Dexterity;
	});
}

int32 UCharacterBuild::ComputeConstitution() const
{
	return ComputeSkill([](const FSkillSet& T)
	{
		return T.Constitution;
	});
}

int32 UCharacterBuild::ComputeIntelligence() const
{
	return ComputeSkill([](const FSkillSet& T)
	{
		return T.Intelligence;
	});
}

int32 UCharacterBuild::ComputeWisdom() const
{
	return ComputeSkill([](const FSkillSet& T)
	{
		return T.Wisdom;
	});
}

int32 UCharacterBuild::ComputeCharisma() const
{
	return ComputeSkill([](const FSkillSet& T)
	{
		return T.Charisma;
	});
}
