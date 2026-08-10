#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "MemberViewModel.generated.h"

/**
 * One row in an organization's member list — the organization's view of one character it has a
 * position for. The mirror image of UPositionViewModel, which is the character's view of the same
 * relationship. Same shape as USelectablePositionViewModel/USelectableOrganizationViewModel: pure
 * display data plus a click, the row decides nothing.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UMemberViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Pushed by the owning view model: which character this row stands for and how to show it. */
	void SetSource(FCharacterId InCharacterId, const FText& InCharacterName, const FSlateBrush& InPortrait, const FText& InRoleName);

	FCharacterId GetCharacterId() const { return CharacterId; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemberClicked, UMemberViewModel*, ClickedMember);

	/** Nothing in this pass binds a handler here — it exists so a future member-detail flow has
	 * somewhere to attach without changing this row's shape. */
	UPROPERTY(BlueprintAssignable, Category="Member")
	FOnMemberClicked OnClicked;

	/** Bind a button's OnClicked to this; it broadcasts OnClicked(this) in turn. */
	UFUNCTION(BlueprintCallable, Category="Member")
	void CallOnClicked();

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Member", meta=(AllowPrivateAccess))
	FText CharacterName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Member", meta=(AllowPrivateAccess))
	FSlateBrush Portrait;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Member", meta=(AllowPrivateAccess))
	FText RoleName;

public:
	FText GetCharacterName() const { return CharacterName; }
	FSlateBrush GetPortrait() const { return Portrait; }
	FText GetRoleName() const { return RoleName; }

private:
	FCharacterId CharacterId;
};
