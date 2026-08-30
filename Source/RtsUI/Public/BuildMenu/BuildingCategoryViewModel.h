#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Styling/SlateBrush.h"
#include "BuildingCategoryViewModel.generated.h"

class UBuildMenuEntryViewModel;

UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UBuildingCategoryViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void SetDisplay(const FText& InDisplayName, const FText& InDescription, const FSlateBrush& InIcon,const TArray<UBuildMenuEntryViewModel*> InEntries);

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Category", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Category", meta=(AllowPrivateAccess))
	FText Description;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Category", meta=(AllowPrivateAccess))
	FSlateBrush Icon;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Category", meta=(AllowPrivateAccess))
	TArray<UBuildMenuEntryViewModel*> Entries;

public:
	const FText& GetDisplayName() const { return DisplayName; }
	const FText& GetDescription() const { return Description; }
	const FSlateBrush& GetIcon() const { return Icon; }
	const TArray<UBuildMenuEntryViewModel*>& GetEntries() const { return Entries; }
};
