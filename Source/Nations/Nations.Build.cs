using UnrealBuildTool;

public class Nations : ModuleRules
{
    public Nations(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RtsCore",
                "MassCore",
                "MassEntity"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                // UNationActions::RegisterOrganization settles an organization in a nation, which
                // is an Organizations-facade mutation (FOrganizationSettledNationsFragment).
                "Organizations"
            }
        );
    }
}