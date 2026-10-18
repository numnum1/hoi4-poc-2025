using UnrealBuildTool;

public class Focuses : ModuleRules
{
	public Focuses(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"RtsCore",
				"MassCore",
				"MassEntity",
				// The focus, tree, requirement and event assets derive from UPrimaryDataAsset and
				// carry FSlateBrush on their public headers
				"Engine",
				"SlateCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate"
			}
		);
	}
}
