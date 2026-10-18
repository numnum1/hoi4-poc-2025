#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FocusPrerequisiteGroup.h"
#include "Styling/SlateBrush.h"
#include "OrganizationFocusAsset.generated.h"

class UEventAsset;
class URequirementAsset;

UCLASS(BlueprintType)
class FOCUSES_API UOrganizationFocusAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText FocusName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess, MultiLine))
	FText Tooltip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSlateBrush Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess, ClampMin=0))
	int32 DurationDays = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FFocusPrerequisiteGroup> Prerequisites;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<URequirementAsset> Requirement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UEventAsset> Event;

public:
	FORCEINLINE const FText& GetFocusName() const { return FocusName; }
	FORCEINLINE const FText& GetTooltip() const { return Tooltip; }
	FORCEINLINE const FSlateBrush& GetIcon() const { return Icon; }
	FORCEINLINE int32 GetDurationDays() const { return DurationDays; }
	FORCEINLINE const TArray<FFocusPrerequisiteGroup>& GetPrerequisites() const { return Prerequisites; }
	FORCEINLINE const URequirementAsset* GetRequirement() const { return Requirement; }
	FORCEINLINE const UEventAsset* GetEvent() const { return Event; }
};
