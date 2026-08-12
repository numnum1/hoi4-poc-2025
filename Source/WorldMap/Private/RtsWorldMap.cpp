#include "RtsWorldMap.h"

#include "Kismet/GameplayStatics.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"

void ARtsWorldMap::OnConstruction(const FTransform& Transform)
{
	MapDynamicMaterials.Empty();

	Super::OnConstruction(Transform);
}

void ARtsWorldMap::SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals)
{
	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (Provinces == nullptr)
	{
		return;
	}

	TArray<FDataLUT> DataLUT;
	DataLUT.Reserve(InVisuals.Num());

	for (const TPair<FProvinceId, FProvinceVisuals>& Entry : InVisuals)
	{
		const FColor MapPixelColor = Provinces->GetMapPixelColor(Entry.Key);

		FDataLUT Row;
		Row.ID.R = MapPixelColor.R;
		Row.ID.G = MapPixelColor.G;
		Row.Primary = Entry.Value.PrimaryColor;
		Row.Secondary = Entry.Value.GetEffectiveSecondaryColor();
		Row.National = Entry.Value.GetEffectiveNationalColor();
		DataLUT.Add(Row);
	}

	if (DataLUT.IsEmpty())
	{
		return;
	}

	// Never clears: the caller sends the provinces it wants changed, not the whole world, and
	// clearing would black out every province missing from this batch.
	WriteToLUTs(DataLUT, /*ClearLUTs=*/false, /*UpdateNational=*/true);
}

ARtsWorldMap* ARtsWorldMap::GetRtsWorldMap(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	if (World == nullptr)
	{
		return nullptr;
	}

	// Not ABA_Map::GetMap: that caches the first ABA_Map it finds in a static shared across worlds
	// and only casts afterwards, so a stale or unrelated map hands back a null cast here.
	return Cast<ARtsWorldMap>(UGameplayStatics::GetActorOfClass(World, ARtsWorldMap::StaticClass()));
}
