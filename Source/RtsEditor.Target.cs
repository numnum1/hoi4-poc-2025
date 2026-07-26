// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class RtsEditorTarget : TargetRules
{
	public RtsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;

		ExtraModuleNames.AddRange( new string[] { "Rts" } );
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new string[] { "Buildings", "Provinces", "Nations", "Races", "Characters", "RtsPlayer", "Scenario", "Ecology", "Time", "Organizations", "Ideologies", "Schedules", "RtsProxy" });
	}
}
