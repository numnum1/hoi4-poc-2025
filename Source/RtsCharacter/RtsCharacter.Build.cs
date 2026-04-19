using UnrealBuildTool;

public class RtsCharacter : ModuleRules
{
    public RtsCharacter(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "JuristicPerson",
                "Organisation"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "State",
                "CharacterRole",
                "Schedule",
                "Race",
                "CharacterBuild"
            }
        );
    }
}