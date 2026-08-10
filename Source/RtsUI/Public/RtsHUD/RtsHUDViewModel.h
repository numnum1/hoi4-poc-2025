#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "PlayedCharacterView/PlayerCharacterViewModel.h"
#include "RtsHUDViewModel.generated.h"

/**
 * Root view model for the HUD, owned by the HUD's root widget the same way UPlayedCharacterView
 * owns UPlayerCharacterViewModel — widget state the HUD itself invents, not derived from any one
 * domain.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API URtsHUDViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	URtsHUDViewModel();

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetControlledCharacterId(FCharacterId InControlledCharacterId);

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetActiveModal(EPlayerModal InActiveModal);

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetSelectedPositionIndex(int32 InSelectedPositionIndex);

private:
	// TODO: Make this computed
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FCharacterId ControlledCharacterId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	EPlayerModal ActiveModal;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	int32 SelectedPositionIndex;

public:
	FCharacterId GetControlledCharacterId() const { return ControlledCharacterId; }
	EPlayerModal GetActiveModal() const { return ActiveModal; }
	int32 GetSelectedPositionIndex() const { return SelectedPositionIndex; }
};
