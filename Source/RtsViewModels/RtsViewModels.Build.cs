using UnrealBuildTool;

public class RtsViewModels : ModuleRules
{
    public RtsViewModels(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RtsCore",
                // Every view model here reads one or more of these facades directly, and some
                // hand data back across domain boundaries (a character view model shows the
                // owning nation's name, and so on). This module is the only place allowed to know
                // all of them at once — the domain modules themselves stay unaware of one another.
                "Characters",
                "Provinces",
                "Nations",
                "Organizations",
                "Buildings",
                "Schedules",
                // The view models derive from UMVVMViewModelBase
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
                // UProvinceViewModel passes UEcology* through
                "Ecology"
            }
        );
    }
}
