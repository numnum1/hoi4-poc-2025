using UnrealBuildTool;

public class BasicCharacterActions : ModuleRules
{
    public BasicCharacterActions(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "JuristicPerson", 
                "CharacterAction",
                "Organisation",
                "UMG"
            }
        );
    }
}