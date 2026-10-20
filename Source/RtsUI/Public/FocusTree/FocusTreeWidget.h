#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FieldNotificationId.h"
#include "FocusTreeWidget.generated.h"

class UCanvasPanel;
class UFocusNodeViewModel;
class UFocusNodeWidget;
class URtsHUDViewModel;
class UUseOrganizationFocusTree;

UCLASS(Abstract)
class RTSUI_API UFocusTreeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Focus Tree")
	void SetHUDViewModel(URtsHUDViewModel* InHUDViewModel);

	UFUNCTION(BlueprintPure, Category="Focus Tree")
	URtsHUDViewModel* GetHUDViewModel() const { return HUDViewModel; }

	UFUNCTION(BlueprintCallable, Category="Focus Tree")
	void SelectTree(int32 InTreeIndex);

protected:
	virtual void NativeDestruct() override;

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UCanvasPanel> NodeCanvas;

	UPROPERTY(EditAnywhere, Category="Focus Tree")
	TSubclassOf<UFocusNodeWidget> NodeWidgetClass;

	UPROPERTY(EditAnywhere, Category="Focus Tree|Layout")
	FVector2D CellSize = FVector2D(180.0, 140.0);

	UPROPERTY(EditAnywhere, Category="Focus Tree|Layout")
	FVector2D NodeSize = FVector2D(140.0, 90.0);

	UPROPERTY(EditAnywhere, Category="Focus Tree|Lines")
	FLinearColor RequiredLineColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category="Focus Tree|Lines")
	FLinearColor AlternativeLineColor = FLinearColor(0.8f, 0.8f, 0.8f);

	UPROPERTY(EditAnywhere, Category="Focus Tree|Lines")
	FLinearColor ExclusiveLineColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, Category="Focus Tree|Lines")
	float LineThickness = 2.f;

	UPROPERTY(EditAnywhere, Category="Focus Tree|Lines")
	float DashLength = 8.f;

private:
	void HandleNodesChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId);

	void RebuildNodes();

	void UnbindFocusTree();

	FVector2f ComputeNodeTopLeft(const UFocusNodeViewModel* InNode) const;

	FVector2f CanvasToLocal(const FGeometry& InAllottedGeometry, const FVector2f& InCanvasPosition) const;

	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;

	UPROPERTY(Transient)
	TObjectPtr<UUseOrganizationFocusTree> BoundFocusTree;

	FDelegateHandle NodesChangedHandle;
};
