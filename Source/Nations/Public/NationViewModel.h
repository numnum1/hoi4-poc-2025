#pragma once

#include "CoreMinimal.h"
#include "NationInfo.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "NationViewModel.generated.h"

class UNations;

/**
 * A read-only C++ MVVM view model for a single nation, addressed by the same FNationId every
 * other nation API uses. See UCharacterViewModel (Characters module) for the pattern this
 * follows: Initialize(Container, Id), flat FieldNotify properties fed from the container's
 * getters, one shared HandleChanged(Id) bound to every change event.
 *
 * FNationInfo is taken apart into one property per member — UMG binds single values, not structs.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class NATIONS_API UNationViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Nation Viewmodel")
	void Initialize(UNations* InNations, FNationId InNationId);

protected:
	//~ Identity ---------------------------------------------------------------------------------

	/** False once the nation's entity has been destroyed. The name and flag stay behind. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Nation Viewmodel", meta=(AllowPrivateAccess))
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Nation Viewmodel", meta=(AllowPrivateAccess))
	FNationId NationId;

	/** NationId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Nation Viewmodel", meta=(AllowPrivateAccess))
	int32 NationIdValue = INDEX_NONE;

	//~ FNationInfo ------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Nation Viewmodel|Display", meta=(AllowPrivateAccess))
	FText NationName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Nation Viewmodel|Display", meta=(AllowPrivateAccess))
	FSlateBrush Flag;

public:
	bool IsValid() const { return bIsValid; }
	FNationId GetNationId() const { return NationId; }
	int32 GetNationIdValue() const { return NationIdValue; }

	FText GetNationName() const { return NationName; }
	FSlateBrush GetFlag() const { return Flag; }

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void Refresh();
	void Unbind();

	UFUNCTION()
	void HandleNationEvent(FNationId InNationId);

	UPROPERTY()
	TObjectPtr<UNations> NationsContainer;
};
