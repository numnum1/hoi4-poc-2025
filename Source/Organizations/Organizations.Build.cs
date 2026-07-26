using UnrealBuildTool;

public class Organizations : ModuleRules
{
    public Organizations(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RtsCore",
                "MassCore",
                "MassEntity",
                // UOrganizationViewModel derives from UMVVMViewModelBase
                "ModelViewViewModel"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}
