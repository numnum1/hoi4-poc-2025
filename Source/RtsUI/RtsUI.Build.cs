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
                // UPlayerCharacterViewModel derives from UCharacterViewModel and reaches its
                // facades (Characters/Organizations/Roles) through the base class's protected
                // members — the same reason RtsViewModels needs its facades.
                "RtsViewModels",
                // UBuildingType is a property on UPlayerCharacterViewModel's header: the
                // headquarters the found-organization modal is collecting.
                "Buildings"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                // Widget base classes (UUserWidget) live here — see "Widgets sind C++-Klassen"
                // in CLAUDE.md.
                "UMG",
                // Only to resolve ARtsPlayer in the .cpp; the header forward-declares it.
                "RtsPlayer",
                // FCharacterId/FRtsActionContext plumbing needs the facade types directly
                "Characters",
                // SelectProvince plays the province's InspectionSound and reads the province name
                // for the modal's site label; resolved via UProvinces in the .cpp only, not
                // exposed on the header
                "Provinces",
                // MakeContext hands UBanking to URtsActions so founding can be paid for
                "Banking",
                // UPlayerCharacterViewModel derives from UMVVMViewModelBase (via UCharacterViewModel);
                // the base class's generated FieldNotify machinery needs this linked directly here too.
                "ModelViewViewModel"
            }
        );
    }
}
