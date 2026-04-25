// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class RtsTarget : TargetRules
{
	public RtsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;

		ExtraModuleNames.AddRange( new string[] { "Rts" } );
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new string[] { "RtsCharacter", "State", "BasicCharacterActions", "JuristicPerson", "CharacterAction", "CharacterRole", "Organisation", "Nation", "Schedule", "CharacterBuild", "Race", "TimeSimulation", "RtsPlayer", "Flag", "Skills", "Economy", "Demography", "RoleResource", "BankingSystem", "Buildings" });
	}
}
