#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "SelectableCharacterViewModel.generated.h"

/**
 * One entry in a list of characters — the recruit modal's candidate row. Same shape as
 * USelectableOrganizationViewModel/USelectablePositionViewModel: pure display data plus a click,
 * the row decides nothing, the owning view model decides what picking it means.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSVIEWMODELS_API USelectableCharacterViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Pushed by the owning view model: which character this row stands for and how to show it. */
	void SetSource(FCharacterId InCharacterId, const FText& InDisplayName, const FSlateBrush& InPortrait);

	FCharacterId GetCharacterId() const { return CharacterId; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterClicked, USelectableCharacterViewModel*, ClickedCharacter);

	/** The owning view model binds one handler here for every row it creates; that handler decides
	 * what the click means. Bind a button's OnClicked to CallOnClicked() below, not to this. */
	UPROPERTY(BlueprintAssignable, Category="Selectable Character")
	FOnCharacterClicked OnClicked;

	/** Bind a button's OnClicked to this; it broadcasts OnClicked(this) in turn. */
	UFUNCTION(BlueprintCallable, Category="Selectable Character")
	void CallOnClicked();

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Selectable Character", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Selectable Character", meta=(AllowPrivateAccess))
	FSlateBrush Portrait;

public:
	FText GetDisplayName() const { return DisplayName; }
	FSlateBrush GetPortrait() const { return Portrait; }

private:
	FCharacterId CharacterId;
};
