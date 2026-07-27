#include "RtsActions.h"

#include "ActionViewModel.h"
#include "FoundOrganizationActionViewModel.h"

void URtsActions::CollectClassesForScope(const ERtsActionScope InScope, TArray<TSubclassOf<UActionViewModel>>& OutClasses)
{
	OutClasses.Reset();

	switch (InScope)
	{
	case ERtsActionScope::RAS_Self:
		OutClasses.Add(UFoundOrganizationActionViewModel::StaticClass());
		break;

	case ERtsActionScope::RAS_Position:
		// No position-scoped action exists yet.
		break;
	}
}
