#pragma once

#include "CoreMinimal.h"
#include "BankInfo.h"
#include "RtsIds.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "Banking.generated.h"

class UBankEntitySubsystem;
class UBankAccountEntitySubsystem;

class FBankingModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBanksEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBankAccountEvent, FBankAccountId, InAccountId);

/**
 * The UI-facing face of the banking data.
 *
 * Banks and accounts live in Mass entities owned by UBankEntitySubsystem /
 * UBankAccountEntitySubsystem; this component keeps only what does not belong in a fragment (bank
 * names, logos) and the change events view models bind to — the same split UOrganizations makes
 * for organizations.
 */
UCLASS(ClassGroup=(Rts), meta=(BlueprintSpawnableComponent))
class BANKING_API UBanking : public UActorComponent
{
	GENERATED_BODY()
public:
	UBanking();

	UPROPERTY(BlueprintAssignable)
	FBanksEvent OnBankAdded;

	UPROPERTY(BlueprintAssignable)
	FBankAccountEvent OnAccountOpened;

	/** Fired after Deposit, Withdraw, or Transfer changes InAccountId's balance. */
	UPROPERTY(BlueprintAssignable)
	FBankAccountEvent OnBalanceChanged;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FBankInfo> BankInfos;

public:
	UFUNCTION(BlueprintPure)
	int32 NumBanks() const { return BankInfos.Num(); }

	UFUNCTION(BlueprintPure)
	bool IsBankAlive(FBankId InBankId) const;

	UFUNCTION(BlueprintPure)
	FBankInfo GetBankInfo(FBankId InBankId) const;

	UFUNCTION(BlueprintCallable)
	FBankId AddBank(const FBankInfo& NewBank);

	/** Always opens a new account — a character can hold several. */
	UFUNCTION(BlueprintCallable)
	FBankAccountId OpenAccountForCharacter(FBankId InBankId, FCharacterId InOwnerId);

	/** Returns the existing account instead of opening a second one, if the organization has one. */
	UFUNCTION(BlueprintCallable)
	FBankAccountId OpenAccountForOrganization(FBankId InBankId, FOrganizationId InOwnerId);

	UFUNCTION(BlueprintPure)
	int32 GetBalance(FBankAccountId InAccountId) const;

	/** Which bank InAccountId is held at. Invalid if the account is not alive. */
	UFUNCTION(BlueprintPure)
	FBankId GetBank(FBankAccountId InAccountId) const;

	/** Every account the character currently holds, across every bank. Empty if it has none. */
	UFUNCTION(BlueprintPure)
	TArray<FBankAccountId> GetAccountsForCharacter(FCharacterId InOwnerId) const;

	UFUNCTION(BlueprintPure)
	FBankAccountId FindAccountForOrganization(FOrganizationId InOwnerId) const;

	UFUNCTION(BlueprintCallable)
	void Deposit(FBankAccountId InAccountId, int32 InAmount);

	/** Returns false and does nothing if InAmount is more than the current balance. */
	UFUNCTION(BlueprintCallable)
	bool Withdraw(FBankAccountId InAccountId, int32 InAmount);

	/** Returns false and does nothing if the withdrawal side would fail. */
	UFUNCTION(BlueprintCallable)
	bool Transfer(FBankAccountId InFromId, FBankAccountId InToId, int32 InAmount);

private:
	UBankEntitySubsystem* GetBanks() const;
	UBankAccountEntitySubsystem* GetAccounts() const;
};
