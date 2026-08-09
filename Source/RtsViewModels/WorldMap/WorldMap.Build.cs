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
				"ModelViewViewModel"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
			}
		);
	}
}
