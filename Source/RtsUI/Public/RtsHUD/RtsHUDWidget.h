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

	/** Optional so a HUD Blueprint without a map on screen still works. */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UWorldMapWidget> WorldMapWidget;

private:
	void HandleProvinceVisualsChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId);

	void PushProvinceVisuals();

	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;
};
