#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FieldNotificationId.h"
#include "RtsHUDWidget.generated.h"

class URtsHUDViewModel;
class UWorldMapWidget;

/**
 * The HUD's root widget. Owns URtsHUDViewModel — created here in NativeOnInitialized, the same
 * pattern UPlayedCharacterView uses for UPlayerCharacterViewModel — and is the one place that hands
 * the map its colors: it watches the view model's computed ProvinceVisuals and pushes the result
 * into the world map widget.
 *
 * That direction is not a style choice. UWorldMapWidget lives in WorldMap, which must not know
 * RtsUI (RtsUI already depends on WorldMap), so the map widget cannot bind to a view model here. It
 * gets pushed to instead, and stays free of any knowledge about map modes.
 *
 * The Blueprint deriving from this binds its own widgets to GetHUDViewModel through a "Property
 * Path" view model entry — never "Create Instance", which would spawn a second, uninitialized one.
 */
UCLASS(Abstract)
class RTSUI_API URtsHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Rts HUD")
	URtsHUDViewModel* GetHUDViewModel() const { return HUDViewModel; }

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	/** Where the coalesced repaint happens — see HandleProvinceVisualsChanged. */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Optional so a HUD Blueprint without a map on screen still works. */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UWorldMapWidget> WorldMapWidget;

private:
	void HandleProvinceVisualsChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId);

	void PushProvinceVisuals();

	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;

	/** One repaint per frame at most. Province ownership changes arrive one province at a time, and
	 * each repaint recomputes the whole mode and makes the plugin read back and rewrite its lookup
	 * textures in full — so a nation losing twenty provinces in one tick must not cost twenty of
	 * those. */
	bool bProvinceVisualsDirty = false;

	/** So the missing-map-widget error is reported once, not per repaint. */
	bool bMissingMapWidgetLogged = false;
};
