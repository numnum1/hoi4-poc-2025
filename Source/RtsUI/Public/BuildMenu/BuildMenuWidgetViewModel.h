#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "BuildMenuWidgetViewModel.generated.h"

class UBuildingCategoryViewModel;
class UBuildingType;

UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UBuildMenuWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, FieldNotify)
	TArray<UBuildingType*> Buildings;

	UPROPERTY()
	TArray<TObjectPtr<UBuildingCategoryViewModel>> Categories;

	void RebuildCategories();

public:
	UFUNCTION(BlueprintCallable)
	void Fill(const TArray<UBuildingType*>& InBuildings);
	
	UFUNCTION(BlueprintCallable, FieldNotify)
	TArray<UBuildingCategoryViewModel*> ComputeCategories() const;
	
};
