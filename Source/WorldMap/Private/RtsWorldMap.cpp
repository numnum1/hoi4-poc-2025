#include "RtsWorldMap.h"

#include "BA_DataFunctions.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "Nations.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"
#include "WorldMapWidget.h"

void ARtsWorldMap::BeginPlay()
{
	Super::BeginPlay();

	// The political map is derived, not pushed: whoever moves a province between nations does not
	// have to know the map exists. Scenario load fires OnProvincesSetup once, after which the ProvID
	// lookup can be built at all.
	if (UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this))
	{
		Provinces->OnProvincesSetup.AddDynamic(this, &ARtsWorldMap::HandleProvincesChanged);
		Provinces->OnOwningNationChanged.AddDynamic(this, &ARtsWorldMap::HandleOwningNationChanged);
	}
}

void ARtsWorldMap::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this))
	{
		Provinces->OnProvincesSetup.RemoveAll(this);
		Provinces->OnOwningNationChanged.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ARtsWorldMap::SetWorldMapWidget(UWorldMapWidget* InWidget)
{
	WorldMapWidget = InWidget;

	// Paint straight away with whatever the widget's view model already holds — registration is the
	// first moment both sides exist.
	if (WorldMapWidget)
	{
		WorldMapWidget->ApplyOverriddenVisuals();
	}
}

void ARtsWorldMap::SelectProvinceAt(const FVector2D InUV)
{
	if (WorldMapWidget)
	{
		WorldMapWidget->HandleProvinceClicked(ResolveProvinceAt(InUV));
	}
}

void ARtsWorldMap::SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals)
{
	OverriddenVisuals = InVisuals;
	Repaint();
}

void ARtsWorldMap::SetProvinceOverlayColors(const TMap<FProvinceId, FColor>& InColors)
{
	TMap<FProvinceId, FProvinceVisuals> Visuals;
	Visuals.Reserve(InColors.Num());

	for (const TPair<FProvinceId, FColor>& Entry : InColors)
	{
		FProvinceVisuals& Visual = Visuals.Add(Entry.Key);
		Visual.PrimaryColor = Entry.Value;
	}

	SetProvinceVisuals(Visuals);
}

void ARtsWorldMap::ClearProvinceOverlayColors()
{
	SetProvinceVisuals(TMap<FProvinceId, FProvinceVisuals>());
}

void ARtsWorldMap::Repaint()
{
	EnsureProvinceColorLookup();

	// Before the scenario is loaded there is nothing to paint — writing an empty LUT would clear the
	// map to black instead, which is exactly the state this repaint exists to avoid.
	if (ProvinceIdToColorRG.IsEmpty())
	{
		return;
	}

	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	const UNations* Nations = FindRtsGameStateComponent<UNations>(this);

	TArray<FDataLUT> DataLUT;
	DataLUT.Reserve(ProvinceIdToColorRG.Num());

	for (const TPair<FProvinceId, FColorRG>& Entry : ProvinceIdToColorRG)
	{
		FProvinceVisuals Visuals;
		Visuals.PrimaryColor = UnownedProvinceColor;

		if (const FProvinceVisuals* Override = OverriddenVisuals.Find(Entry.Key))
		{
			Visuals = *Override;
		}
		else if (Provinces && Nations)
		{
			const FNationId OwningNationId = Provinces->GetOwningNation(Entry.Key);
			if (OwningNationId.IsValid())
			{
				Visuals.PrimaryColor = Nations->GetNationInfo(OwningNationId).MapColor;
			}
		}

		FDataLUT Row;
		Row.ID = Entry.Value;
		Row.Primary = Visuals.PrimaryColor;
		Row.Secondary = Visuals.GetEffectiveSecondaryColor();
		DataLUT.Add(Row);
	}

	WriteToLUTs(DataLUT, /*ClearLUTs=*/true, /*UpdateNational=*/false);
}

void ARtsWorldMap::HandleProvincesChanged()
{
	Repaint();
}

void ARtsWorldMap::HandleOwningNationChanged(FProvinceId InProvinceId)
{
	Repaint();
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

void ARtsWorldMap::EnsureProvinceColorLookup()
{
	if (ProvinceIdToColorRG.Num() > 0 || ProvinceMapData == nullptr)
	{
		return;
	}

	UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
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

		const FProvinceId Id = Provinces->FindByMapId(MapData->Name);
		if (Id.IsValid())
		{
			ProvinceIdToColorRG.Add(Id, MapData->Color);
		}
	}
}

FProvinceId ARtsWorldMap::ResolveProvinceAt(const FVector2D InUV) const
{
	if (ProvinceIdTexture == nullptr || ProvinceMapData == nullptr)
	{
		return FProvinceId();
	}

	const int32 Width = ProvinceIdTexture->GetSizeX();
	const int32 Height = ProvinceIdTexture->GetSizeY();
	const int32 X = FMath::Clamp(FMath::FloorToInt(InUV.X * Width), 0, Width - 1);
	const int32 Y = FMath::Clamp(FMath::FloorToInt(InUV.Y * Height), 0, Height - 1);

	FColor UnusedRGBA;
	const FColorRG PixelColor = UBA_DataFunctions::FindColor(ProvinceIdTexture, X, Y, UnusedRGBA);

	FName ProvinceMapId = NAME_None;
	for (const TPair<FName, uint8*>& Row : ProvinceMapData->GetRowMap())
	{
		const FProvinceMapDataGame* MapData = reinterpret_cast<FProvinceMapDataGame*>(Row.Value);
		if (MapData && MapData->Color == PixelColor)
		{
			ProvinceMapId = MapData->Name;
			break;
		}
	}

	if (ProvinceMapId.IsNone())
	{
		return FProvinceId();
	}

	UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	return Provinces ? Provinces->FindByMapId(ProvinceMapId) : FProvinceId();
}
