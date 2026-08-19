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
				// ARtsWorldMap derives from AActor and ARtsMapPawn from APawn, both in public headers
				"CoreUObject",
				"Engine",
				// FProvinceId
				"RtsCore",
				// The widget blueprint deriving from UWorldMapWidget binds through MVVM
				"ModelViewViewModel",
				// UWorldMapWidget derives from UUserWidget
				"UMG"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ARtsWorldMap writes its lookup textures from a render command
				"RenderCore",
				"RHI",
				// ARtsMapPawn binds the select action itself; only the .cpp needs the input types
				"EnhancedInput",
				// All that is left of the BloodAnvil plugin dependency: the DataTable row format its
				// map editor writes (FProvinceMapDataGame) and the pixel read that resolves a click
				// against the province-ID texture. The map and camera classes used to derive from the
				// plugin's and no longer do — see ARtsWorldMap's class comment. Removable once the
				// scenario converts that table into project data at load time.
				"BloodAnvil",
				// Both directions of ARtsWorldMap's province lookup go through
				// FProvinceDisplayInfo::MapId — an id to the pixel color the lookup tables are keyed
				// on, and a clicked pixel back to an id. Resolved off the game state via
				// FindRtsGameStateComponent (RtsCore) in the .cpp only — this module still knows
				// nothing about RtsUI, see UWorldMapWidget's class comment.
				"Provinces"
			}
		);
	}
}
