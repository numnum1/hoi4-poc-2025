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
                // Several view models here read one or more facades directly and some hand data
                // back across domain boundaries (a character view model shows the owning nation's
                // name, and so on) — this module is the only place allowed to know all of them at
                // once. RtsActions.h/PositionViewModel.h include Characters.h on their own public
                // header, so this needs to be public, not just private.
                "Characters",
                // UBuildingType is a property on UPlayerCharacterViewModel's header: the
                // headquarters the found-organization modal is collecting.
                "Buildings",
                // The view models derive from UMVVMViewModelBase on their public headers.
                "ModelViewViewModel",
                // URtsMapModes::BuildVisuals and URtsHUDViewModel::GetProvinceVisuals both return
                // FProvinceVisuals on public headers, so this cannot be private. Does not create a
                // cycle: WorldMap does not depend on RtsUI — the map is handed colors and knows
                // nothing about why a province is that color.
                "WorldMap"
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
                // SelectProvince plays the province's InspectionSound and reads the province name
                // for the modal's site label; resolved via UProvinces in the .cpp only, not
                // exposed on the header
                "Provinces",
                // NationViewModel/ProvinceViewModel read UNations for the owning nation's name
                "Nations",
                // The ethnicity map mode reads UPopulations; FRtsMapModeContext only forward-declares
                // it, so this stays private
                "Populations",
                // RefreshJoinableOrganizations reads UOrganizations::GetOrganizationInfo directly
                // to name each joinable row's organization
                "Organizations",
                // MakeContext hands UBanking to URtsActions so founding can be paid for
                "Banking",
                // UScheduleViewModel reads USchedules directly
                "Schedules",
                // UTimezoneViewModel shows the clock, resolved off the game state like any other
                // component there.
                "Time",
                // UProvinceViewModel passes UEcology* through
                "Ecology",
                // Only to resolve ARtsMapPawn in the .cpp and bind OnRightMouseButtonClicked; the
                // header forward-declares it.
                "MapCamera"
            }
        );
    }
}
