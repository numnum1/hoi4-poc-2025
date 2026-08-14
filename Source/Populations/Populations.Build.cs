using UnrealBuildTool;

public class Populations : ModuleRules
{
	public Populations(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// FPopId, FEthnicityId, FProvinceId — a pop names its province by id, so this module
				// does not depend on Provinces
				"RtsCore",
				"MassCore",
				"MassEntity"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
			}
		);
	}
}
