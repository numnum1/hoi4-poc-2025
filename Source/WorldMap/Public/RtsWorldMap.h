#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapTypes.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "RtsWorldMap.generated.h"

class UCanvasRenderTarget2D;
class UDataTable;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UStaticMesh;
class UStaticMeshComponent;
class UTexture2D;

class ARtsWorldMap;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapTypeChanged, ARtsWorldMap*, Map);

/**
 * The map actor: the single source of truth for the province-ID lookup, and a renderer for colors
 * someone else decided on.
 *
 * This used to derive from the BloodAnvil plugin's ABA_Map. The parts of it this game actually needs
 * — the mesh and material setup, the UV projection, and the lookup textures the map material samples
 * — were moved in here so they sit in Source/ where they are versioned and reviewable; Plugins/ is
 * not in this repository. The plugin stays installed for its material graph, its meshes and its map
 * authoring tools, and the only code still read from it is the DataTable row format those tools
 * write (see EnsureMapDataLookup).
 *
 * It derives nothing beyond the province lookup. It has no opinion on who owns a province, which map
 * mode is showing, or what a click means — all of that is decided in RtsUI and pushed down through
 * UWorldMapWidget.
 */
UCLASS()
class WORLDMAP_API ARtsWorldMap : public AActor
{
	GENERATED_BODY()

public:
	ARtsWorldMap();

	/** The province at a map UV, or an invalid id for open water and anything else no province
	 * claims. Reads the province-ID texture's pixel and resolves it through the province table —
	 * the reverse of the translation SetProvinceVisuals does. */
	UFUNCTION(BlueprintCallable, Category="Province Lookup")
	FProvinceId ResolveProvinceAt(FVector2D InUV);

	/** Paints the given provinces and leaves every other province untouched. A province the table
	 * does not know is skipped rather than painted black. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals);

	/** Where a world-space hit lands in map UV space. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	FVector2D VectorToUV(FVector InImpactPoint) const;

	/** The map actor of InWorldContextObject's world. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map", meta=(WorldContext="WorldContextObject"))
	static ARtsWorldMap* GetRtsWorldMap(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetMapType(EMapType InMapType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	EMapType GetMapType() const { return MapType; }

	/** The camera pawn follows this to reset its framing when the projection changes. */
	UPROPERTY(BlueprintAssignable, Category="World Map")
	FMapTypeChanged MapTypeChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MapMesh;

	/** Sampled per pixel by the map material to tell provinces apart, and read on the CPU to resolve
	 * clicks. Must stay uncompressed and CPU-readable (VectorDisplacementmap, sRGB off, no mips) —
	 * ResolveProvinceAt reads its mip 0 directly. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UTexture2D> ProvinceIdTexture;

	/** Rows of FProvinceMapDataGame as the plugin's map editor writes them, keyed by the same names
	 * FProvinceDisplayInfo::MapId carries. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UDataTable> ProvinceMapData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	EMapType MapType = EMapType::Globe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	TObjectPtr<UStaticMesh> GlobeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	TObjectPtr<UStaticMesh> FlatMesh;

	/** Use r = sqrt((x*y)/4*pi) to give the globe the same surface area as the plane. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings", meta=(Units="Centimeters"))
	float MapRadius = 19947.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings", meta=(Units="Centimeters"))
	FVector2D MapDimensions = FVector2D(100000.0f, 50000.0f);

	/** One per material slot on the mesh; anything beyond that is kept for transition or distance
	 * variants. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	TArray<TObjectPtr<UMaterialInterface>> MapMaterials;

	UPROPERTY(BlueprintReadOnly, Category="Shading")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> MapDynamicMaterials;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	/** Creates the lookup textures and binds them, along with the province-ID texture, to the map
	 * material. Runs once, and only once the province table can be read — the textures' height comes
	 * out of it. */
	bool EnsureLuts();

	/** Reads the province table once. Nothing authored there changes at runtime, so this never has
	 * to be rebuilt. */
	void EnsureMapDataLookup();

	/** Resolves the table against the loaded scenario, which is the only reason this is separate:
	 * the table can be read the moment the map exists, but province ids only exist once the
	 * scenario has been loaded. Stays empty and is retried until then. */
	void EnsureProvinceLookup();

	void RebuildDynamicMaterials();

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> PrimaryLut;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> SecondaryLut;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> NationalLut;

	/** Nothing writes this one. It is still created and bound because the map material samples it,
	 * and leaving the parameter unset would fall back to whatever default the material carries
	 * instead of the black it has always been handed. */
	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> VfxLut;

	/** MapId -> the province-ID pixel the lookup textures key their rows on. */
	TMap<FName, FMapPixelId> MapIdToPixelId;

	/** The reverse, keyed on the red and green channels packed together: those two carry the id,
	 * and a pixel sampled off the texture brings along a blue and alpha the table never declares. */
	TMap<int32, FName> PixelIdToMapId;

	/** What painting actually goes through, so a repaint does not copy a whole FProvinceDisplayInfo
	 * — FText, FSlateBrush and two object pointers — once per province just to read its MapId. */
	TMap<FProvinceId, FMapPixelId> ProvinceIdToPixelId;

	/** The tallest row any province occupies, which is what decides the lookup textures' height. */
	int32 HighestPixelRow = 0;

	/** So a map missing its assets is reported once, not per click. */
	bool bLookupFailureLogged = false;
};
