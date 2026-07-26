using UnrealBuildTool;

public class Scenario : ModuleRules
{
    public Scenario(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                // ECharacterSimTier lives with the character types it describes
                "Characters",
                // UScheduleAsset authors an FSchedule in a public header
                "Schedules"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Ecology"
            }
        );
    }
}