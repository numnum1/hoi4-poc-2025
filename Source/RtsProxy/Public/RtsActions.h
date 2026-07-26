#pragma once

#include "CoreMinimal.h"
#include "RtsActionContext.h"
#include "Templates/SubclassOf.h"

class UActionViewModel;

/**
 * Which action view model classes apply to a scope. This one method is the whole registry —
 * adding an action means writing a new UActionViewModel subclass (see
 * UFoundOrganizationActionViewModel) and adding one line to the matching case here.
 */
namespace RtsActions
{
	RTSPROXY_API void CollectClassesForScope(ERtsActionScope InScope, TArray<TSubclassOf<UActionViewModel>>& OutClasses);
}
