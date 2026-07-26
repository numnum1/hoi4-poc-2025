using UnrealBuildTool;

public class RtsProxy : ModuleRules
{
    public RtsProxy(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RtsCore",
                // Every proxy wraps one of these facades, and they hand each other back across
                // domain boundaries (a character proxy returns a province proxy, and so on).
                // This module is the only place allowed to know all of them at once — the domain
                // modules themselves stay unaware of one another.
                "Characters",
                "Provinces",
                "Nations",
                "Organizations",
                "Buildings",
                "Schedules",
                // The view models moved here with the proxies; they derive from UMVVMViewModelBase
                "ModelViewViewModel"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                // UProvinceViewModel/UProvinceProxy pass UEcology* through
                "Ecology"
            }
        );
    }
}
