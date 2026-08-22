#include "WorldMapSubsystem.h"

#include "BA_DataFunctions.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogWorldMapSubsystem, Log, All);

UWorldMapSubsystem* UWorldMapSubsystem::Get(const UObject* InWorldContextObject)
{
	const UWorld* World = InWorldContextObject ? InWorldContextObject->GetWorld() : nullptr;
	return World ? World->GetSubsystem<UWorldMapSubsystem>() : nullptr;
}

void UWorldMapSubsystem::Configure(UDataTable* InProvinceMapData)
{
	if (ProvinceMapData == InProvinceMapData)
	{
		return;
	}

	ProvinceMapData = InProvinceMapData;

	MapIdToPixelId.Empty();
	ProvinceIdToPixelId.Empty();
	PixelIdToProvinceId.Empty();
	ProvinceAdjacencies.Empty();
	ProvincesAdjacentToUnknown.Empty();
	ProvinceUnitPositions.Empty();
	HighestPixelRow = 0;
	bMissingTableLogged = false;
	bNoMatchingProvincesLogged = false;
}

bool UWorldMapSubsystem::HasMapData() const
{
	EnsureMapDataLookup();
	return !MapIdToPixelId.IsEmpty();
}

void UWorldMapSubsystem::EnsureMapDataLookup() const
{
	if (!MapIdToPixelId.IsEmpty())
	{
		return;
	}

	if (ProvinceMapData == nullptr)
	{
		if (!bMissingTableLogged)
		{
			bMissingTableLogged = true;
			UE_LOG(LogWorldMapSubsystem, Error,
			       TEXT("No province map table configured, so no province can be resolved or painted. "
			            "ARtsWorldMap hands it over, so check that the level has a map actor with ProvinceMapData assigned."));
		}
		return;
	}

	for (const TPair<FName, uint8*>& Row : ProvinceMapData->GetRowMap())
	{
		const FProvinceMapDataGame* MapData = reinterpret_cast<FProvinceMapDataGame*>(Row.Value);
		if (MapData == nullptr)
		{
			continue;
		}

		FMapPixelId PixelId;
		PixelId.R = MapData->Color.R;
		PixelId.G = MapData->Color.G;

		// Keyed on the row name, not the row's own Name field: that field holds the map editor's
		// label ("Arnhem"), while a province's MapId is the row key ("1893").
		MapIdToPixelId.Add(Row.Key, PixelId);
		HighestPixelRow = FMath::Max(HighestPixelRow, static_cast<int32>(PixelId.G));
	}
}

void UWorldMapSubsystem::EnsureProvinceLookup() const
{
	EnsureMapDataLookup();

	if (!ProvinceIdToPixelId.IsEmpty() || MapIdToPixelId.IsEmpty())
	{
		return;
	}

	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (Provinces == nullptr)
	{
		return;
	}

	for (const TPair<FName, FMapPixelId>& Entry : MapIdToPixelId)
	{
		const FProvinceId ProvinceId = Provinces->FindByMapId(Entry.Key);
		if (ProvinceId.IsValid())
		{
			ProvinceIdToPixelId.Add(ProvinceId, Entry.Value);
			PixelIdToProvinceId.Add(Entry.Value, ProvinceId);
		}
	}

	// A table whose names match no province leaves the map unpainted with nothing else to show for
	// it, so the mismatch is called out rather than left to look like a rendering problem.
	if (ProvinceIdToPixelId.IsEmpty() && !bNoMatchingProvincesLogged)
	{
		bNoMatchingProvincesLogged = true;
		UE_LOG(LogWorldMapSubsystem, Error,
		       TEXT("None of the %d names in the province map table match a province MapId of the loaded scenario."),
		       MapIdToPixelId.Num());
	}
}

void UWorldMapSubsystem::EnsureAdjacencyLookup() const
{
	EnsureProvinceLookup();

	if (!ProvinceAdjacencies.IsEmpty() || ProvinceIdToPixelId.IsEmpty() || ProvinceMapData == nullptr)
	{
		return;
	}

	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (Provinces == nullptr)
	{
		return;
	}

	for (const TPair<FName, uint8*>& Row : ProvinceMapData->GetRowMap())
	{
		const FProvinceMapDataGame* MapData = reinterpret_cast<FProvinceMapDataGame*>(Row.Value);
		if (MapData == nullptr)
		{
			continue;
		}

		const FProvinceId ProvinceId = Provinces->FindByMapId(Row.Key);
		if (!ProvinceId.IsValid())
		{
			continue;
		}

		TArray<FProvinceId>& Neighbours = ProvinceAdjacencies.Add(ProvinceId);
		for (const FName& AdjacentMapId : MapData->Adjacencies.Adjacency)
		{
			const FProvinceId AdjacentId = Provinces->FindByMapId(AdjacentMapId);
			if (AdjacentId.IsValid())
			{
				Neighbours.Add(AdjacentId);
				continue;
			}

			// Recorded rather than silently dropped: the map table covers the whole world while a
			// scenario loads a corner of it, so most neighbours resolve to nothing — and a province
			// whose neighbour the scenario does not know is exactly a province on the outer edge,
			// which is what the zone border test is looking for.
			ProvincesAdjacentToUnknown.Add(ProvinceId);
		}
	}
}

void UWorldMapSubsystem::EnsureUnitPositionLookup() const
{
	if (!ProvinceUnitPositions.IsEmpty() || ProvinceMapData == nullptr)
	{
		return;
	}

	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (Provinces == nullptr)
	{
		return;
	}

	for (const TPair<FName, uint8*>& Row : ProvinceMapData->GetRowMap())
	{
		const FProvinceMapDataGame* MapData = reinterpret_cast<FProvinceMapDataGame*>(Row.Value);
		if (MapData == nullptr)
		{
			continue;
		}

		const FProvinceId ProvinceId = Provinces->FindByMapId(Row.Key);
		if (ProvinceId.IsValid())
		{
			ProvinceUnitPositions.Add(ProvinceId, MapData->UnitPosition);
		}
	}
}

bool UWorldMapSubsystem::FindPixelId(const FProvinceId InProvinceId, FMapPixelId& OutPixelId) const
{
	EnsureProvinceLookup();

	const FMapPixelId* PixelId = ProvinceIdToPixelId.Find(InProvinceId);
	if (PixelId == nullptr)
	{
		return false;
	}

	OutPixelId = *PixelId;
	return true;
}

TArray<FMapLutEntry> UWorldMapSubsystem::BuildLutRows(const TMap<FProvinceId, FProvinceVisuals>& InVisuals) const
{
	EnsureProvinceLookup();

	TArray<FMapLutEntry> Rows;
	Rows.Reserve(InVisuals.Num());

	for (const TPair<FProvinceId, FProvinceVisuals>& Entry : InVisuals)
	{
		const FMapPixelId* PixelId = ProvinceIdToPixelId.Find(Entry.Key);
		if (PixelId == nullptr)
		{
			continue;
		}

		FMapLutEntry Row;
		Row.Id = *PixelId;
		Row.Primary = Entry.Value.PrimaryColor;
		Row.Secondary = Entry.Value.GetEffectiveSecondaryColor();
		Row.National = Entry.Value.GetEffectiveNationalColor();
		Rows.Add(Row);
	}

	return Rows;
}

FProvinceId UWorldMapSubsystem::FindProvince(const FMapPixelId& InPixelId) const
{
	EnsureProvinceLookup();

	const FProvinceId* ProvinceId = PixelIdToProvinceId.Find(InPixelId);
	LogResolvedPixel(InPixelId, ProvinceId ? *ProvinceId : FProvinceId());
	return ProvinceId ? *ProvinceId : FProvinceId();
}

void UWorldMapSubsystem::LogResolvedPixel(const FMapPixelId& InPixelId, const FProvinceId InProvinceId) const
{
	const FName* MapId = nullptr;
	for (const TPair<FName, FMapPixelId>& Entry : MapIdToPixelId)
	{
		if (Entry.Value == InPixelId)
		{
			MapId = &Entry.Key;
			break;
		}
	}

	if (MapId == nullptr)
	{
		UE_LOG(LogWorldMapSubsystem, Warning,
		       TEXT("Clicked pixel (%d, %d): no row of the province map table has this color (%d rows). "
		            "Either the click landed on water or a border, or the ID texture and the table are "
		            "from different map generations."),
		       InPixelId.R, InPixelId.G, MapIdToPixelId.Num());
		return;
	}

	FString Label;
	if (ProvinceMapData != nullptr)
	{
		if (const FProvinceMapDataGame* Row = ProvinceMapData->FindRow<FProvinceMapDataGame>(*MapId, TEXT("LogResolvedPixel"), false))
		{
			Label = Row->Name.ToString();
		}
	}

	if (!InProvinceId.IsValid())
	{
		UE_LOG(LogWorldMapSubsystem, Warning,
		       TEXT("Clicked pixel (%d, %d) is map row '%s' (\"%s\"), but no province of the loaded "
		            "scenario carries that MapId."),
		       InPixelId.R, InPixelId.G, *MapId->ToString(), *Label);
		return;
	}

	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	const FString ProvinceName = Provinces ? Provinces->GetDisplayInfo(InProvinceId).Name.ToString() : FString();
	UE_LOG(LogWorldMapSubsystem, Display,
	       TEXT("Clicked pixel (%d, %d) -> map row '%s' (\"%s\") -> province %d (\"%s\")."),
	       InPixelId.R, InPixelId.G, *MapId->ToString(), *Label, InProvinceId.Value, *ProvinceName);
}

bool UWorldMapSubsystem::FindUnitPosition(const FProvinceId InProvinceId, FVector2D& OutUnitPosition) const
{
	EnsureUnitPositionLookup();

	const FVector2D* Position = ProvinceUnitPositions.Find(InProvinceId);
	if (Position == nullptr)
	{
		return false;
	}

	OutUnitPosition = *Position;
	return true;
}

int32 UWorldMapSubsystem::GetHighestPixelRow() const
{
	EnsureMapDataLookup();
	return HighestPixelRow;
}

bool UWorldMapSubsystem::BuildNameZones(const TArray<FProvinceId>& InProvinces, TArray<FMapNameZone>& OutZones) const
{
	EnsureAdjacencyLookup();
	if (ProvinceAdjacencies.IsEmpty())
	{
		return false;
	}

	TMap<FProvinceId, int32> ProvinceToZone;

	// Not TSet<FProvinceId>(InProvinces): that constructor form trips a known MSVC bug, see the note
	// atop Containers/Set.h.
	TSet<FProvinceId> ProvinceSet = TSet<FProvinceId>(TArray<FProvinceId>(InProvinces));

	for (int32 ZoneIndex = 0; ZoneIndex < OutZones.Num(); ++ZoneIndex)
	{
		for (const FProvinceId& Province : OutZones[ZoneIndex].Provinces)
		{
			ProvinceToZone.Add(Province, ZoneIndex);
		}
	}

	for (const FProvinceId& Province : InProvinces)
	{
		if (ProvinceToZone.Contains(Province))
		{
			continue;
		}

		TArray<int32> NeighbourZones;

		// A neighbour outside the loaded scenario has no id to test against ProvinceSet below, so that
		// case is answered from the set EnsureAdjacencyLookup filled instead.
		bool bIsBordering = ProvincesAdjacentToUnknown.Contains(Province);

		if (const TArray<FProvinceId>* Neighbours = ProvinceAdjacencies.Find(Province))
		{
			for (const FProvinceId& Neighbour : *Neighbours)
			{
				if (const int32* ZoneIndex = ProvinceToZone.Find(Neighbour))
				{
					NeighbourZones.AddUnique(*ZoneIndex);
				}

				if (!ProvinceSet.Contains(Neighbour))
				{
					bIsBordering = true;
				}
			}
		}

		if (NeighbourZones.IsEmpty())
		{
			FMapNameZone NewZone;
			NewZone.Provinces.Add(Province);
			if (bIsBordering)
			{
				NewZone.Bordering.Add(Province);
			}

			ProvinceToZone.Add(Province, OutZones.Num());
			OutZones.Add(MoveTemp(NewZone));
			continue;
		}

		// Bridging two zones that were only separate because nothing had joined them yet: the province
		// joins the first and every other zone it touches is folded into that one.
		const int32 PrimaryZone = NeighbourZones[0];
		OutZones[PrimaryZone].Provinces.Add(Province);
		ProvinceToZone.Add(Province, PrimaryZone);

		if (bIsBordering)
		{
			OutZones[PrimaryZone].Bordering.Add(Province);
		}

		for (int32 i = 1; i < NeighbourZones.Num(); ++i)
		{
			const int32 MergeZone = NeighbourZones[i];
			if (!OutZones.IsValidIndex(MergeZone) || MergeZone == PrimaryZone)
			{
				continue;
			}

			for (const FProvinceId& MergedProvince : OutZones[MergeZone].Provinces)
			{
				OutZones[PrimaryZone].Provinces.Add(MergedProvince);
				ProvinceToZone[MergedProvince] = PrimaryZone;
			}

			OutZones[PrimaryZone].Bordering.Append(OutZones[MergeZone].Bordering);

			// Emptied rather than removed: the indices in ProvinceToZone and NeighbourZones would all
			// shift. The empty zones are dropped in one pass at the end.
			OutZones[MergeZone].Provinces.Empty();
			OutZones[MergeZone].Bordering.Empty();
		}
	}

	OutZones.RemoveAll([](const FMapNameZone& InZone) { return InZone.Provinces.IsEmpty(); });
	return true;
}
