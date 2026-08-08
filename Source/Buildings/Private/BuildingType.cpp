#include "BuildingType.h"

UBuildingType::UBuildingType()
{
}

int32 UBuildingType::GetSpaceFor(const EBuildingFunction InFunction) const
{
	// Summed rather than "first match wins": an asset that lists a function twice should read as
	// the total room it offers, not silently drop the second entry.
	int32 Total = 0;
	for (const FBuildingFunctionSlot& Slot : Functions)
	{
		if (Slot.Function == InFunction)
		{
			Total += Slot.Space;
		}
	}
	return Total;
}

bool UBuildingType::GetPrimaryFunction(EBuildingFunction& OutFunction) const
{
	if (Functions.Num() == 0)
	{
		return false;
	}

	OutFunction = Functions[0].Function;
	return true;
}
