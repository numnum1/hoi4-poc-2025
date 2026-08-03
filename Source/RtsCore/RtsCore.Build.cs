using UnrealBuildTool;

public class RtsCore : ModuleRules
{
    public RtsCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                // Public: RtsIdLibrary.h is a UBlueprintFunctionLibrary, so its header pulls in
                // UObject/Kismet types that every including module needs too.
                "CoreUObject",
                "Engine",
            }
        );
    }
}
