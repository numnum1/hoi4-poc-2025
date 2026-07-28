using UnrealBuildTool;

public class RtsUI : ModuleRules
{
    public RtsUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RtsCore",
                // UPlayerCharacterViewModel derives from UCharacterViewModel and reaches every
                // other view model/proxy through it — the same reason RtsProxy needs its facades.
                "RtsProxy"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                // Only to resolve ARtsPlayer in the .cpp; the header forward-declares it.
                "RtsPlayer",
                // FCharacterId/FRtsActionContext plumbing needs the facade types directly
                "Characters",
                // UPlayerCharacterViewModel derives from UMVVMViewModelBase (via UCharacterViewModel);
                // the base class's generated FieldNotify machinery needs this linked directly here too.
                "ModelViewViewModel"
            }
        );
    }
}
