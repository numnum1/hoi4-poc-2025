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
				// UWorldMapViewModel derives from UMVVMViewModelBase
				"ModelViewViewModel",
				// UWorldMapWidget derives from UUserWidget
				"UMG",
				// ARtsWorldMap derives from ABA_Map (BloodAnvil plugin) — the base class needs the
				// full type in the header, not just a forward declare
				"BloodAnvil"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				// ARtsWorldMap::ResolveProvinceAt resolves the clicked province by name via
				// UProvinces::FindByMapId. Resolved off the game state via FindRtsGameStateComponent
				// (RtsCore), not a dependency on RtsUI or Rts: see UWorldMapWidget's class comment for
				// why the connection to UPlayerCharacterViewModel is a Blueprint binding, not a C++
				// dependency in either direction.
				"Provinces",
				// ARtsWorldMap::Repaint paints a province in its owning nation's FNationInfo::MapColor
				// when nothing overrides it. Same resolution path as Provinces above
				// (FindRtsGameStateComponent), no header dependency.
				"Nations"
			}
		);
	}
}
