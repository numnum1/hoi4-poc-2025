#pragma once

#include "CoreMinimal.h"
#include "NationInfo.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "NationViewModel.generated.h"

class UNationProxy;

/**
 * A read-only C++ MVVM view model for a single nation, addressed by the same FNationId every
 * other nation API uses. See UCharacterViewModel for the pattern this follows: Initialize(Id)
 * takes a single parameter so it stays usable as an MVVM function binding target, fetches the
 * matching UNationProxy from URtsProxySubsystem, and refreshes flat FieldNotify properties
 * whenever that proxy reports a change.
 *
 * FNationInfo is taken apart into one property per member — UMG binds single values, not structs.
 *
 * Deliberately does not publish a province count: nothing tells a nation when a province changes
 * hands, so a cached count would quietly drift. Walk UNationProxy::GetProvinces for that, which
 * reads through and cannot be wrong.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSPROXY_API UNationViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category="Nation Viewmodel")
	void Initialize(FNationId InNationId);

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

protected:
	UFUNCTION(BlueprintCallable, Category="Nation Viewmodel")
	void Refresh();

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void Unbind();

	UFUNCTION()
	void HandleProxyChanged();

	UPROPERTY()
	TObjectPtr<UNationProxy> Proxy;
};
