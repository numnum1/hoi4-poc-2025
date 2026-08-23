#include "RtsWorldMapViewModel.h"

#include "MapProjection.h"
#include "RtsWorldMap.h"
#include "WorldMapSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsWorldMapViewModel, Log, All);

ARtsWorldMap* URtsWorldMapViewModel::ResolveWorldMap()
{
	if (WorldMap)
	{
		return WorldMap;
	}

	WorldMap = ARtsWorldMap::GetRtsWorldMap(this);
	if (WorldMap == nullptr && !bMissingMapLogged)
	{
		// Worth saying out loud: without the map actor everything below still runs, it just reaches
		// nothing, which looks like a rendering problem rather than a level missing its map.
		bMissingMapLogged = true;
		UE_LOG(LogRtsWorldMapViewModel, Error,
		       TEXT("%s found no ARtsWorldMap in the level, so nothing it is handed is ever drawn."),
		       *GetName());
	}

	return WorldMap;
}

void URtsWorldMapViewModel::SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals)
{
	ProvinceVisuals = InVisuals;

	ARtsWorldMap* Map = ResolveWorldMap();
	const UWorldMapSubsystem* Subsystem = UWorldMapSubsystem::Get(this);
	if (Map == nullptr || Subsystem == nullptr)
	{
		return;
	}

	const TArray<FMapLutEntry> Rows = Subsystem->BuildLutRows(ProvinceVisuals);

	if (Rows.IsEmpty() && !ProvinceVisuals.IsEmpty() && !bNoMatchingProvincesLogged)
	{
		bNoMatchingProvincesLogged = true;
		UE_LOG(LogRtsWorldMapViewModel, Error,
		       TEXT("%s: none of the %d provinces handed in are in the province-ID table."),
		       *GetName(), ProvinceVisuals.Num());
	}

	Map->PaintProvinces(Rows);
}

void URtsWorldMapViewModel::SetHoverHighlightEnabled(const bool bInEnabled)
{
	if (bHoverHighlightEnabled == bInEnabled)
	{
		return;
	}

	bHoverHighlightEnabled = bInEnabled;

	if (ARtsWorldMap* Map = ResolveWorldMap())
	{
		Map->SetHoverHighlightEnabled(bHoverHighlightEnabled);
	}
}

void URtsWorldMapViewModel::SetNationData(const TMap<FNationId, FWorldMapNationData>& InNationData)
{
	NationData = InNationData;

	ARtsWorldMap* Map = ResolveWorldMap();
	const UWorldMapSubsystem* Subsystem = UWorldMapSubsystem::Get(this);
	if (Map == nullptr || Subsystem == nullptr)
	{
		return;
	}

	// Translated here rather than looked up per traced point: the label Blueprint asks who owns a
	// pixel hundreds of times while it lays a name out along a country.
	TMap<FMapPixelId, FNationId> Owners;
	for (const TPair<FNationId, FWorldMapNationData>& Entry : NationData)
	{
		for (const FProvinceId& OwnedProvince : Entry.Value.OwnedProvinces)
		{
			FMapPixelId PixelId;
			if (Subsystem->FindPixelId(OwnedProvince, PixelId))
			{
				Owners.Add(PixelId, Entry.Key);
			}
		}
	}

	Map->SetProvinceOwners(Owners);
	Map->SpawnCountryLabels(BuildLabels(*Subsystem));
}

TArray<FWorldMapLabel> URtsWorldMapViewModel::BuildLabels(const UWorldMapSubsystem& InSubsystem) const
{
	TArray<FWorldMapLabel> Labels;

	for (const TPair<FNationId, FWorldMapNationData>& Entry : NationData)
	{
		// Which nations go unlabelled — too small to fit a name, a subject drawn under its overlord —
		// is not decided yet, and neither is folding a subject's provinces into its overlord's zones.
		const TArray<FNationId> TraceOverTags = { Entry.Key };

		TArray<FMapNameZone> Zones;
		if (!InSubsystem.BuildNameZones(Entry.Value.OwnedProvinces, Zones))
		{
			UE_LOG(LogRtsWorldMapViewModel, Warning, TEXT("Could not create map name zones."));
			continue;
		}

		for (const FMapNameZone& Zone : Zones)
		{
			FWorldMapLabel Label;
			if (BuildLabelForZone(InSubsystem, Zone, Entry.Value.Name, TraceOverTags, Label))
			{
				Labels.Add(MoveTemp(Label));
			}
		}
	}

	return Labels;
}

bool URtsWorldMapViewModel::BuildLabelForZone(
	const UWorldMapSubsystem& InSubsystem,
	const FMapNameZone& InZone,
	const FText& InName,
	const TArray<FNationId>& InTraceOverTags,
	FWorldMapLabel& OutLabel)
{
	// A zone with no border at all (every neighbour is the owner's own) still has to be labelled, so
	// it falls back to the whole zone rather than dividing by an empty Points below — that produced a
	// NaN average, and the NaN survived all the way into a texture lookup as an out-of-range index.
	const TArray<FProvinceId>& Outline = InZone.Bordering.IsEmpty() ? InZone.Provinces : InZone.Bordering;
	if (Outline.IsEmpty())
	{
		return false;
	}

	TArray<FVector2D> Points;
	Points.Reserve(Outline.Num());
	for (const FProvinceId& Bordering : Outline)
	{
		FVector2D UnitPosition;
		if (!InSubsystem.FindUnitPosition(Bordering, UnitPosition))
		{
			UE_LOG(LogRtsWorldMapViewModel, Warning, TEXT("Could not find a province's unit position."));
			return false;
		}

		Points.Add(UnitPosition);
	}

	FVector2D Average = FVector2D::ZeroVector;
	FVector2D LargestPoint01 = FVector2D::ZeroVector;
	FVector2D LargestPoint02 = FVector2D::ZeroVector;
	double LargestDistance = 0.0;

	for (const FVector2D& Point : Points)
	{
		Average = Point + Average;
		for (const FVector2D& InnerPoint : Points)
		{
			// Measured on the unit sphere: scaling every distance by the globe's radius cannot change
			// which pair is furthest apart, which is how the layout gets by without a radius. Straight
			// UV distance would not do — it stretches towards the poles and wraps at the seam.
			const double Squared = FVector::DistSquared(
				MapProjection::UVToLocation(InnerPoint, 1.0f),
				MapProjection::UVToLocation(Point, 1.0f));

			if (Squared > LargestDistance)
			{
				LargestDistance = Squared;
				LargestPoint01 = InnerPoint;
				LargestPoint02 = Point;
			}
		}
	}

	Average = Average / FVector2D(Points.Num(), Points.Num());

	// Laid out west to east, so the name never reads backwards.
	const bool bOneSmallerTwo = LargestPoint01.X < LargestPoint02.X;
	OutLabel.UVPoints = {
		bOneSmallerTwo ? LargestPoint02 : LargestPoint01,
		Average,
		bOneSmallerTwo ? LargestPoint01 : LargestPoint02
	};
	OutLabel.Name = InName;
	OutLabel.OwnerTags = InTraceOverTags;
	return true;
}
