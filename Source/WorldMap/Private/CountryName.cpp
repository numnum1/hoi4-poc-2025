#include "CountryName.h"

#include "RtsIds.h"

ACountryName::ACountryName()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACountryName::Initialize(const TArray<FVector>& InPoints, const FText& InName, ARtsWorldMap* InMap, const TArray<FNationId>& InOwnerTags)
{
	Points = InPoints;
	Name = InName;
	Map = InMap;
	OwnerTags = InOwnerTags;
}
