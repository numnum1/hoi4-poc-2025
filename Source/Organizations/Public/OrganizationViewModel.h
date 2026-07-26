#pragma once

#include "CoreMinimal.h"
#include "OrganizationInfo.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "OrganizationViewModel.generated.h"

class UOrganizations;

/**
 * A read-only C++ MVVM view model for a single organization, addressed by the same
 * FOrganizationId every other organization API uses. See UCharacterViewModel (Characters
 * module) for the pattern this follows: Initialize(Container, Id), flat FieldNotify properties
 * fed from the container's getters, one shared HandleChanged(Id) bound to every change event.
 *
 * FOrganizationInfo and FOrganizationAlignment are taken apart into one property per member —
 * UMG binds single values, not structs.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class ORGANIZATIONS_API UOrganizationViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Organization Viewmodel")
	void Initialize(UOrganizations* InOrganizations, FOrganizationId InOrganizationId);

	/** One branch of the published alignment, or 0 if the index is out of range. */
	UFUNCTION(BlueprintPure, Category="Organization Viewmodel|Alignment")
	float GetAlignmentBranch(int32 InIndex) const;

	/** The nation this organization settled in at InIndex, or an invalid id if out of range. */
	UFUNCTION(BlueprintPure, Category="Organization Viewmodel|Nations")
	FNationId GetSettledNationId(int32 InIndex) const;

protected:
	//~ Identity ---------------------------------------------------------------------------------

	/** False once the organization's entity has been dissolved. The name and logo stay behind. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Organization Viewmodel", meta=(AllowPrivateAccess))
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel", meta=(AllowPrivateAccess))
	FOrganizationId OrganizationId;

	/** OrganizationId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel", meta=(AllowPrivateAccess))
	int32 OrganizationIdValue = INDEX_NONE;

	//~ FOrganizationInfo ------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Display", meta=(AllowPrivateAccess))
	FText OrganizationName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Display", meta=(AllowPrivateAccess))
	FSlateBrush Logo;

	//~ FOrganizationAlignment -------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsAlignmentPublished, Category="Organization Viewmodel|Alignment", meta=(AllowPrivateAccess))
	bool bAlignmentPublished = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Alignment", meta=(AllowPrivateAccess))
	TArray<float> AlignmentBranches;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Alignment", meta=(AllowPrivateAccess))
	int32 AlignmentBranchCount = 0;

	//~ Settled nations --------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Nations", meta=(AllowPrivateAccess))
	TArray<FNationId> SettledNationIds;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Nations", meta=(AllowPrivateAccess))
	TArray<int32> SettledNationIdValues;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Nations", meta=(AllowPrivateAccess))
	int32 SettledNationCount = 0;

public:
	bool IsValid() const { return bIsValid; }
	FOrganizationId GetOrganizationId() const { return OrganizationId; }
	int32 GetOrganizationIdValue() const { return OrganizationIdValue; }

	FText GetOrganizationName() const { return OrganizationName; }
	FSlateBrush GetLogo() const { return Logo; }

	bool IsAlignmentPublished() const { return bAlignmentPublished; }
	const TArray<float>& GetAlignmentBranches() const { return AlignmentBranches; }
	int32 GetAlignmentBranchCount() const { return AlignmentBranchCount; }

	const TArray<FNationId>& GetSettledNationIds() const { return SettledNationIds; }
	const TArray<int32>& GetSettledNationIdValues() const { return SettledNationIdValues; }
	int32 GetSettledNationCount() const { return SettledNationCount; }

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void Refresh();
	void Unbind();

	UFUNCTION()
	void HandleOrganizationEvent(FOrganizationId InOrganizationId);

	UPROPERTY()
	TObjectPtr<UOrganizations> OrganizationsContainer;
};
