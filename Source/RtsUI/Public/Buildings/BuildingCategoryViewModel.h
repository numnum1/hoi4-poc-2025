#pragma once

#include "CoreMinimal.h"
#include "BuildingCategory.h"
#include "MVVMViewModelBase.h"
#include "BuildingCategoryViewModel.generated.h"

class UBuildingKindViewModel;

/**
 * One section of the build menu: a heading and the building kinds catalogued under it. Manual
 * creation because there is one instance per section in the owning view model's array, the same as
 * UMapModeEntryViewModel.
 *
 * It owns nothing and decides nothing — the rows in Kinds belong to URtsHUDViewModel, which creates
 * them, keeps their affordability and selection current and is what their OnClicked is bound to.
 * This view model only groups them for display, which is why it has no dispatcher of its own: a
 * click on a building is a click on that building, not on its section.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UBuildingCategoryViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Records which section this row stands for and pulls its heading out of
	 * URtsBuildingCategories. */
	void SetCategory(EBuildingCategory InCategory);

	/** Pushed by the owning view model on every refresh — the rows it created for this section,
	 * already carrying their own cost and affordability. */
	void SetKinds(const TArray<TObjectPtr<UBuildingKindViewModel>>& InKinds);

	EBuildingCategory GetCategory() const { return Category; }

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Category", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Category", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UBuildingKindViewModel>> Kinds;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Category", meta=(AllowPrivateAccess))
	int32 KindCount = 0;

public:
	FText GetDisplayName() const { return DisplayName; }
	const TArray<TObjectPtr<UBuildingKindViewModel>>& GetKinds() const { return Kinds; }
	int32 GetKindCount() const { return KindCount; }

private:
	EBuildingCategory Category = EBuildingCategory::BC_Administration;
};
