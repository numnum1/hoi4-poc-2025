#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "GameFramework/Actor.h"
#include "MapTypes.h"
#include "RtsIds.h"
#include "RtsWorldMap.generated.h"

class ACountryName;
class UCanvasRenderTarget2D;
class UDataTable;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UStaticMesh;
class UStaticMeshComponent;
class UTexture2D;

/**
 * The globe, as a thing that draws: a mesh, the map material, the lookup textures its colors come
 * out of, and the province-ID texture a click is read from. Nothing else.
 *
 * It is a view, in the same sense a widget is. It does not know what a province is — colors arrive
 * already keyed by pixel (FMapLutEntry) and a click leaves as the pixel that was read. Turning
 * either into an FProvinceId is UWorldMapSubsystem's job, and deciding what a color or a click
 * means happens far above, in RtsUI. That is what URtsWorldMapViewModel and UWorldMapWidget are
 * for: they own what is shown, this owns how it is drawn.
 *
 * This used to derive from the BloodAnvil plugin's ABA_Map. The parts of it this game actually needs
 * were moved in here so they sit in Source/ where they are versioned and reviewable; Plugins/ is not
 * in this repository. The plugin stays installed for its material graph, its meshes and its map
 * authoring tools.
 */
UCLASS()
class WORLDMAP_API ARtsWorldMap : public AActor
{
	GENERATED_BODY()

public:
	ARtsWorldMap();

	UPROPERTY(BlueprintAssignable, Category="World Map")
	FWorldMapPixelClicked OnPixelClicked;

	UFUNCTION(BlueprintCallable, Category="World Map")
	void NotifyWorldClicked(const FHitResult& InHit);

	UFUNCTION(BlueprintCallable, Category="World Map")
	void PaintProvinces(const TArray<FMapLutEntry>& InRows);

	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceOwners(const TMap<FMapPixelId, FNationId>& InOwners);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	FNationId FindOwnerNationIdForProvinceColor(const FColor& InProvinceColor) const;

	UFUNCTION(BlueprintCallable, Category="World Map")
	void SpawnCountryLabels(const TArray<FWorldMapLabel>& InLabels);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	ACountryName* SpawnMapName();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	FVector2D VectorToUV(FVector InImpactPoint) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	FVector UVToLocation(FVector2D InUV) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map", meta=(WorldContext="WorldContextObject"))
	static ARtsWorldMap* GetRtsWorldMap(const UObject* WorldContextObject);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MapMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UTexture2D> ProvinceIdTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UTexture2D> ProvinceIdLookupTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UDataTable> ProvinceMapData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	TObjectPtr<UStaticMesh> GlobeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings", meta=(Units="Centimeters"))
	float MapRadius = 19947.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	TArray<TObjectPtr<UMaterialInterface>> MapMaterials;

	UPROPERTY(BlueprintReadOnly, Category="Shading")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> MapDynamicMaterials;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	void ConfigureMesh();

	bool EnsureLuts();

	void ConfigureSubsystem() const;

	bool ReadLookupPixel(FVector2D InUV, FColor& OutColor) const;

	bool IsLookupTextureReadable() const;

	void LogLookupTextureUnreadable(const TCHAR* InReason) const;

	void RebuildDynamicMaterials();

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> PrimaryLut;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> SecondaryLut;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> NationalLut;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> VfxLut;

	UPROPERTY(VisibleAnywhere, Transient, Category="World Map")
	TMap<FMapPixelId, FNationId> PixelOwnerNationId;

	mutable bool bLookupTextureFailureLogged = false;

	bool bPaintFailureLogged = false;
};
