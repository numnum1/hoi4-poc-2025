#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EventOption.h"
#include "Styling/SlateBrush.h"
#include "EventAsset.generated.h"

UCLASS(BlueprintType)
class FOCUSES_API UEventAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess, MultiLine))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSlateBrush Image;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FEventOption> Options;

public:
	FORCEINLINE const FText& GetTitle() const { return Title; }
	FORCEINLINE const FText& GetDescription() const { return Description; }
	FORCEINLINE const FSlateBrush& GetImage() const { return Image; }
	FORCEINLINE const TArray<FEventOption>& GetOptions() const { return Options; }
};
