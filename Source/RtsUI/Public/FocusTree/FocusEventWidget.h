#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FieldNotificationId.h"
#include "FocusEventWidget.generated.h"

class UFocusEventOptionWidget;
class UPanelWidget;
class URtsHUDViewModel;
class UUseOrganizationFocusTree;

UCLASS(Abstract)
class RTSUI_API UFocusEventWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Focus Event")
	void SetHUDViewModel(URtsHUDViewModel* InHUDViewModel);

	UFUNCTION(BlueprintPure, Category="Focus Event")
	URtsHUDViewModel* GetHUDViewModel() const { return HUDViewModel; }

protected:
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UPanelWidget> OptionsBox;

	UPROPERTY(EditAnywhere, Category="Focus Event")
	TSubclassOf<UFocusEventOptionWidget> OptionWidgetClass;

private:
	void HandleEventOptionsChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId);

	void RebuildOptions();

	void UnbindFocusTree();

	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;

	UPROPERTY(Transient)
	TObjectPtr<UUseOrganizationFocusTree> BoundFocusTree;

	FDelegateHandle EventOptionsChangedHandle;
};
