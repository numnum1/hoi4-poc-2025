#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FieldNotificationId.h"
#include "RtsHUDWidget.generated.h"

class UFocusEventWidget;
class UFocusTreeWidget;
class UFoundOrganizationModalWidget;
class UJoinOrganizationModalWidget;
class UOrganizationDetailWidget;
class UProvinceDetailWidget;
class URecruitCharacterModalWidget;
class URtsHUDViewModel;
class UWorldMapWidget;

/**
 * The HUD's root widget. Owns URtsHUDViewModel — created here in NativeOnInitialized — and is the
 * single place that hands it down: to the map, which it watches ProvinceVisuals for, and to every
 * modal and detail widget under it, which keep it only as the source their Blueprint bindings read
 * from. Nothing below this reaches for the view model itself.
 *
 * The map direction is not a style choice. UWorldMapWidget lives in WorldMap, which must not know
 * RtsUI (RtsUI already depends on WorldMap), so the map widget cannot bind to a view model here. It
 * gets pushed to instead, and stays free of any knowledge about map modes. Its clicks come back the
 * same way: bound here in C++ rather than in a Blueprint graph, since this class holds both ends.
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

	/** Optional so this class still initializes cleanly before the Blueprint has placed the modal
	 * (or for a derived HUD that never needs it). Pushed to as soon as both exist. */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UFoundOrganizationModalWidget> FoundOrganizationModal;

	/** Same optionality reasoning as FoundOrganizationModal. */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UJoinOrganizationModalWidget> JoinOrganizationModal;

	/** Same optionality reasoning as FoundOrganizationModal. */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProvinceDetailWidget> ProvinceDetail;

	/** Same optionality reasoning as FoundOrganizationModal. */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UOrganizationDetailWidget> OrganizationDetail;

	/** Same optionality reasoning as FoundOrganizationModal. */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<URecruitCharacterModalWidget> RecruitCharacterModal;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UFocusTreeWidget> FocusTree;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UFocusEventWidget> FocusEvent;

private:
	void HandleProvinceVisualsChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId);

	void HandleNationDataChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId);

	void PushProvinceVisuals();

	void PushNationData();

	/** The map widget, or null with a one-time error. Shared by both pushes so a HUD Blueprint whose
	 * map widget is misnamed reports the cause once rather than once per pushed value. */
	UWorldMapWidget* ResolveMapWidget();

	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;

	/** One repaint per frame at most. Province ownership changes arrive one province at a time, and
	 * each repaint recomputes the whole mode and makes the plugin read back and rewrite its lookup
	 * textures in full — so a nation losing twenty provinces in one tick must not cost twenty of
	 * those. */
	bool bProvinceVisualsDirty = false;

	/** Same coalescing as bProvinceVisualsDirty, and for the same reason: rebuilding the country
	 * labels walks every nation's provinces. */
	bool bNationDataDirty = false;

	/** So the missing-map-widget error is reported once, not per repaint. */
	bool bMissingMapWidgetLogged = false;
};
