#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FocusExclusiveGroup.h"
#include "Styling/SlateBrush.h"
#include "OrganizationFocusTreeAsset.generated.h"

class UOrganizationFocusAsset;
class URequirementAsset;

UCLASS(BlueprintType)
class FOCUSES_API UOrganizationFocusTreeAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText TreeName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSlateBrush Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<URequirementAsset> Requirement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UOrganizationFocusAsset>> Focuses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FFocusExclusiveGroup> ExclusiveGroups;

public:
	FORCEINLINE const FText& GetTreeName() const { return TreeName; }
	FORCEINLINE const FSlateBrush& GetIcon() const { return Icon; }
	FORCEINLINE const URequirementAsset* GetRequirement() const { return Requirement; }
	FORCEINLINE const TArray<TObjectPtr<UOrganizationFocusAsset>>& GetFocuses() const { return Focuses; }
	FORCEINLINE const TArray<FFocusExclusiveGroup>& GetExclusiveGroups() const { return ExclusiveGroups; }
};
