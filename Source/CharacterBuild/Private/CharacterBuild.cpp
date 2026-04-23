#include "CharacterBuild.h"

#include "Net/UnrealNetwork.h"

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
	SkillSet = {};
	
}

void UCharacterBuild::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCharacterBuild, RacialLevels);
	DOREPLIFETIME(UCharacterBuild, JobLevels);
	DOREPLIFETIME(UCharacterBuild, SkillSet);
}

void UCharacterBuild::PostInitProperties()
{
	Super::PostInitProperties();

	RecomputeValues();
}

void UCharacterBuild::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	RecomputeValues();
}

void UCharacterBuild::RecomputeValues()
{
	SkillSet.Strength = ComputeStrength();
	SkillSet.Dexterity = ComputeDexterity();
	SkillSet.Constitution = ComputeConstitution();
	SkillSet.Intelligence = ComputeIntelligence();
	SkillSet.Wisdom = ComputeWisdom();
	SkillSet.Charisma = ComputeCharisma();
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
