#pragma once

#include "CoreMinimal.h"
#include "OrganizationInfo.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "OrganizationViewModel.generated.h"

class UBanking;
class UCharacters;
class UDepartments;
class UDepartmentViewModel;
class UMemberViewModel;
class UOrganizations;
class URoles;

/**
 * A read-only C++ MVVM view model for a single organization, addressed by the same
 * FOrganizationId every other organization API uses. See UCharacterViewModel for the pattern
 * this follows: Initialize(Id) takes a single parameter so it stays usable as an MVVM function
 * binding target, resolves UOrganizations off the game state, binds directly to its events
 * (filtered to this id), and refreshes flat FieldNotify properties on every change.
 *
 * FOrganizationInfo and FOrganizationAlignment are taken apart into one property per member —
 * UMG binds single values, not structs.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UOrganizationViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
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

	//~ Owner ------------------------------------------------------------------------------------
	// The founder, read through UCharacters — the same cross-domain hop UCharacterViewModel makes
	// for the nation of the province it stands in, and the reason these view models live here.

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Owner", meta=(AllowPrivateAccess))
	FCharacterId OwnerId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Owner", meta=(AllowPrivateAccess))
	int32 OwnerIdValue = INDEX_NONE;

	/** Empty for an organization nobody founded. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Owner", meta=(AllowPrivateAccess))
	FText OwnerName;

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

	//~ Members ----------------------------------------------------------------------------------
	// Rows cached per character id, same shape URtsHUDViewModel::SelectablePositionCache uses.

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Members", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UMemberViewModel>> Members;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Members", meta=(AllowPrivateAccess))
	int32 MemberCount = 0;

	/** The sum of the space every department claims — see URtsPresence::GetCapacity. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Members", meta=(AllowPrivateAccess))
	int32 MemberCapacity = 0;

	//~ Departments --------------------------------------------------------------------------------

	/** One row per department the organization is made of, root first — see
	 * UDepartmentEntitySubsystem::CollectDepartmentsOf. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Departments", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UDepartmentViewModel>> Departments;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Departments", meta=(AllowPrivateAccess))
	int32 DepartmentCount = 0;

	//~ Banking ----------------------------------------------------------------------------------

	/** The organization's account balance, 0 if it has no account — see UBanking::GetBalance. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Organization Viewmodel|Banking", meta=(AllowPrivateAccess))
	int32 Balance = 0;

public:
	bool IsValid() const { return bIsValid; }
	FOrganizationId GetOrganizationId() const { return OrganizationId; }
	int32 GetOrganizationIdValue() const { return OrganizationIdValue; }

	FText GetOrganizationName() const { return OrganizationName; }
	FSlateBrush GetLogo() const { return Logo; }

	FCharacterId GetOwnerId() const { return OwnerId; }
	int32 GetOwnerIdValue() const { return OwnerIdValue; }
	FText GetOwnerName() const { return OwnerName; }

	bool IsAlignmentPublished() const { return bAlignmentPublished; }
	const TArray<float>& GetAlignmentBranches() const { return AlignmentBranches; }
	int32 GetAlignmentBranchCount() const { return AlignmentBranchCount; }

	const TArray<FNationId>& GetSettledNationIds() const { return SettledNationIds; }
	const TArray<int32>& GetSettledNationIdValues() const { return SettledNationIdValues; }
	int32 GetSettledNationCount() const { return SettledNationCount; }

	const TArray<TObjectPtr<UMemberViewModel>>& GetMembers() const { return Members; }
	int32 GetMemberCount() const { return MemberCount; }
	int32 GetMemberCapacity() const { return MemberCapacity; }

	const TArray<TObjectPtr<UDepartmentViewModel>>& GetDepartments() const { return Departments; }
	int32 GetDepartmentCount() const { return DepartmentCount; }

	int32 GetBalance() const { return Balance; }

protected:
	UFUNCTION(BlueprintCallable, Category="Organization Viewmodel")
	void Initialize(FOrganizationId InOrganizationId);

	UFUNCTION(BlueprintNativeEvent, Category="Organization Viewmodel")
	void Refresh();

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void Unbind();

	UFUNCTION()
	void HandleOrganizationEvent(FOrganizationId InOrganizationId);

	/** Bound to UCharacters::OnPositionChanged unfiltered — it carries the character's id, not the
	 * organization's, so there is no cheaper check than re-deriving the member list. */
	UFUNCTION()
	void HandleMemberEvent(FCharacterId InCharacterId);

	/** Bound to UDepartments::OnDepartmentCreated/OnDepartmentDissolved, same unfiltered reasoning. */
	UFUNCTION()
	void HandleDepartmentEvent(FDepartmentId InDepartmentId);

	/** Bound to UBanking::OnBalanceChanged, same unfiltered reasoning — it carries the account id,
	 * not the organization's. */
	UFUNCTION()
	void HandleBalanceChanged(FBankAccountId InAccountId);

	UPROPERTY()
	TObjectPtr<UOrganizations> Organizations;

	UPROPERTY()
	TObjectPtr<UCharacters> Characters;

	UPROPERTY()
	TObjectPtr<UDepartments> DepartmentsFacade;

	UPROPERTY()
	TObjectPtr<URoles> Roles;

	UPROPERTY()
	TObjectPtr<UBanking> Banking;

	/** Reused across refreshes so a rebuild that offers the same members keeps the same row objects. */
	UPROPERTY()
	TMap<FCharacterId, TObjectPtr<UMemberViewModel>> MemberCache;

	/** Same idea, keyed by department id. */
	UPROPERTY()
	TMap<FDepartmentId, TObjectPtr<UDepartmentViewModel>> DepartmentCache;
};
