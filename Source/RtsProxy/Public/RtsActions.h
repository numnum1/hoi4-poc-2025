#pragma once

#include "CoreMinimal.h"
#include "RtsActionContext.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RtsActions.generated.h"

class UActionViewModel;

/**
 * The action catalogue's entry point. CollectClassesForScope is the one method that determines
 * which UActionViewModel subclasses apply to a scope — see UFoundOrganizationActionViewModel for
 * the shape a new action follows, and add one line to the switch in RtsActions.cpp to offer it.
 *
 * A UBlueprintFunctionLibrary rather than a plain namespace function, the same shape
 * UScheduleStatics uses, so a widget can call this directly if it ever needs to (building a scope
 * picker, say) instead of only through UCharacterActionsViewModel.
 */
UCLASS()
class RTSPROXY_API URtsActions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Rts Actions")
	static void CollectClassesForScope(ERtsActionScope InScope, TArray<TSubclassOf<UActionViewModel>>& OutClasses);
};
