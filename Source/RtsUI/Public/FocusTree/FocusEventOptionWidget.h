#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FocusEventOptionWidget.generated.h"

class UFocusEventOptionViewModel;
class URtsHUDViewModel;

UCLASS(Abstract)
class RTSUI_API UFocusEventOptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetOption(UFocusEventOptionViewModel* InOption, URtsHUDViewModel* InHUDViewModel);

	UFUNCTION(BlueprintPure, Category="Focus Event Option")
	UFocusEventOptionViewModel* GetOption() const { return Option; }

	UFUNCTION(BlueprintCallable, Category="Focus Event Option")
	void Click();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Focus Event Option")
	void OnOptionSet(UFocusEventOptionViewModel* InOption);

private:
	UPROPERTY(Transient)
	TObjectPtr<UFocusEventOptionViewModel> Option;

	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;
};
