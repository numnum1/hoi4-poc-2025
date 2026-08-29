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
                "Schedules",
                // UScenario authors the building catalogue as UBuildingType* in a public header
                "Buildings"
                // UEthnicityAsset needs no domain module: it authors plain data, and the conversion
                // to FEthnicityInfo/FPopSpawnData happens in ScenarioLoading.h like every other asset
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