#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FocusNodeWidget.generated.h"

class UFocusNodeViewModel;
class URtsHUDViewModel;

UCLASS(Abstract)
class RTSUI_API UFocusNodeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetNode(UFocusNodeViewModel* InNode, URtsHUDViewModel* InHUDViewModel);

	UFUNCTION(BlueprintPure, Category="Focus Node")
	UFocusNodeViewModel* GetNode() const { return Node; }

	UFUNCTION(BlueprintCallable, Category="Focus Node")
	void Click();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Focus Node")
	void OnNodeSet(UFocusNodeViewModel* InNode);

private:
	UPROPERTY(Transient)
	TObjectPtr<UFocusNodeViewModel> Node;

	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;
};
