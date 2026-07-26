using UnrealBuildTool;

public class Characters : ModuleRules
{
    public Characters(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RtsCore",
                "Races",
                "MassCore",
                "MassEntity",
                // Public because FCharacterScheduleSharedFragment exposes FSchedule in a public header
                "Schedules",
                // UCharacterEntitySubsystem drives its processor off UTime::OnHourElapsed
                "Time"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                // MoveCharacter resolves the timezone offset of the destination province
                "Provinces"
            }
        );
    }
}