using UnrealBuildTool;

public class MapCamera : ModuleRules
{
	public MapCamera(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ARtsMapPawn derives from APawn in a public header
				"CoreUObject",
				"Engine"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ARtsMapPawn binds the select action itself; only the .cpp needs the input types
				"EnhancedInput",
				// The camera frames itself against the globe's radius and hands its clicks to
				// ARtsWorldMap. One direction only: WorldMap must not depend on this module, or the
				// two would be a cycle — which is why the click leaves as a call into the map rather
				// than as a delegate the map would have to bind.
				"WorldMap"
			}
		);
	}
}
