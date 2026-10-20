#pragma once

#include "CoreMinimal.h"
#include "Actions/RtsActionContext.h"
#include "Focuses/FocusTreeEdge.h"
#include "Focuses/FocusTreeLayoutCell.h"
#include "Focuses/OrganizationFocusState.h"
#include "Focuses/RtsFocusContext.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RtsFocuses.generated.h"

class UOrganizationFocusAsset;
class UOrganizationFocusTreeAsset;
class URequirementAsset;

UCLASS()
class RTSUI_API URtsFocuses : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FRtsFocusContext MakeFocusContext(const FRtsActionContext& InContext);

	static bool EvaluateRequirement(const URequirementAsset* InRequirement, FOrganizationId InOrganizationId, const FRtsFocusContext& InContext);

	static TArray<UOrganizationFocusTreeAsset*> CollectAvailableTrees(FOrganizationId InOrganizationId, const FRtsFocusContext& InContext);

	static EOrganizationFocusState GetFocusState(const UOrganizationFocusAsset* InFocus, FOrganizationId InOrganizationId, const FRtsFocusContext& InContext);

	static FText GetFocusStateReason(const UOrganizationFocusAsset* InFocus, FOrganizationId InOrganizationId, const FRtsFocusContext& InContext);

	static float GetFocusProgress(const UOrganizationFocusAsset* InFocus, FOrganizationId InOrganizationId, const FRtsFocusContext& InContext);

	static TArray<FFocusTreeLayoutCell> LayoutTree(const UOrganizationFocusTreeAsset* InTree);

	static TArray<FFocusTreeEdge> CollectEdges(const UOrganizationFocusTreeAsset* InTree);

	static FText GetStartFocusBlockedReason(const FRtsActionContext& InContext, const UOrganizationFocusAsset* InFocus);
	static void ExecuteStartFocus(const FRtsActionContext& InContext, const UOrganizationFocusAsset* InFocus);

	static FText GetChooseEventOptionBlockedReason(const FRtsActionContext& InContext, int32 InOptionIndex);
	static void ExecuteChooseEventOption(const FRtsActionContext& InContext, int32 InOptionIndex);

	static void AdvanceDay(const FRtsFocusContext& InContext);
};
