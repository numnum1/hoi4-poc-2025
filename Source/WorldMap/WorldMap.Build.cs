using UnrealBuildTool;

public class WorldMap : ModuleRules
{
	public WorldMap(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// FProvinceId
				"RtsCore",
				// The widget blueprint deriving from UWorldMapWidget binds through MVVM
				"ModelViewViewModel",
				// UWorldMapWidget derives from UUserWidget
				"UMG",
				// ARtsWorldMap derives from ABA_Map and ARtsMapPawn from ABA_MapPawn (BloodAnvil
				// plugin) — the base classes need the full type in the header, not just a forward
				// declare
				"BloodAnvil",
				// ARtsMapPawn binds its own handler to the plugin's InputSelect action rather than
				// implementing ABA_MapPawn::Select, which is Blueprint-only
				"EnhancedInput"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				// Both directions of the province lookup go through UProvinces: ARtsMapPawn turns a
				// clicked pixel into an id via FindByMapPixelColor, ARtsWorldMap turns an id back
				// into the pixel color the plugin's lookup tables are keyed on. Resolved off the
				// game state via FindRtsGameStateComponent (RtsCore) in the .cpp only — this module
				// still knows nothing about RtsUI, see UWorldMapWidget's class comment.
				"Provinces"
			}
		);
	}
}
