#pragma once

#include "CoreMinimal.h"
#include "BankAccountOwnerType.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "BankAccountEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

/**
 * Owns the bank account entities and bridges them to the stable FBankAccountId.
 *
 * Same shape as the other domain subsystems. Ownership is not a stored enum: exactly one of
 * FBankAccountCharacterOwnerFragment / FBankAccountOrganizationOwnerFragment is present on an
 * entity, and GetOwnerType reads that back from fragment presence — see UBuildingEntitySubsystem
 * for the same pattern.
 *
 * A character can hold any number of accounts — OwnerCharacterToAccounts is a TMultiMap, and
 * OpenAccountForCharacter always opens a new one. An organization still has at most one:
 * OpenAccountForOrganization returns the existing account instead of opening a second one. Both
 * maps are proper reverse indexes — this is simulation-layer bookkeeping, not the linear
 * "Sammel-Getter" the UI layer is allowed but not required to use.
 */
UCLASS()
class BANKING_API UBankAccountEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Always opens a new account at InBankId for the character — a character can hold several. */
	FBankAccountId OpenAccountForCharacter(FBankId InBankId, FCharacterId InOwnerId);

	/** Opens a new account at InBankId for the organization, or returns the one it already has. */
	FBankAccountId OpenAccountForOrganization(FBankId InBankId, FOrganizationId InOwnerId);

	/** Destroys the entity. The id's slot stays reserved and reports not-alive from then on. */
	void CloseAccount(FBankAccountId InAccountId);

	bool IsAlive(FBankAccountId InAccountId) const { return GetEntity(InAccountId).IsValid(); }

	FMassEntityHandle GetEntity(FBankAccountId InAccountId) const
	{
		return EntityByIndex.IsValidIndex(InAccountId.Value) ? EntityByIndex[InAccountId.Value] : FMassEntityHandle();
	}

	/** Every id ever handed out, alive or not. */
	int32 NumSlots() const { return EntityByIndex.Num(); }

	/** Ids currently alive, in creation order. */
	const TArray<FBankAccountId>& GetAliveIds() const { return AliveIds; }

	FBankId GetBankId(FBankAccountId InAccountId) const;
	int32 GetBalance(FBankAccountId InAccountId) const;
	/** Derived from which owner fragment is present. BAOT_None if InAccountId is not alive. */
	EBankAccountOwnerType GetOwnerType(FBankAccountId InAccountId) const;
	/** Valid iff GetOwnerType() == BAOT_Character. */
	FCharacterId GetOwnerCharacter(FBankAccountId InAccountId) const;
	/** Valid iff GetOwnerType() == BAOT_Organization. */
	FOrganizationId GetOwnerOrganization(FBankAccountId InAccountId) const;

	/** Every account the character currently holds, across every bank. Empty if it has none. */
	TArray<FBankAccountId> GetAccountsForCharacter(FCharacterId InOwnerId) const;
	/** The organization's account, or an invalid id if it has never opened one. */
	FBankAccountId FindAccountForOrganization(FOrganizationId InOwnerId) const;

	/** A pure value write, always succeeds while the account is alive. */
	void Deposit(FBankAccountId InAccountId, int32 InAmount);
	/** Fails (returns false, no-op) if InAmount is more than the current balance — no overdraft. */
	bool Withdraw(FBankAccountId InAccountId, int32 InAmount);
	/** Withdraws from InFromId and deposits to InToId, only if the withdrawal would succeed. */
	bool Transfer(FBankAccountId InFromId, FBankAccountId InToId, int32 InAmount);

	/** Logs one account's fragments. Backs the Rts.DumpBankAccounts console command. */
	void LogAccount(FBankAccountId InAccountId) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	/** Id -> entity. The inverse lives on each entity as FBankAccountIndexFragment. An invalid
	 * handle means that slot's account is closed; the slot itself is never removed. */
	TArray<FMassEntityHandle> EntityByIndex;

	/** Ids currently alive, in creation order. */
	TArray<FBankAccountId> AliveIds;

	/** Owner -> every account it holds. A character can have several; GetAccountsForCharacter is
	 * O(1) via MultiFind. Entries are removed on CloseAccount. */
	TMultiMap<FCharacterId, FBankAccountId> OwnerCharacterToAccounts;

	/** Owner -> account, so OpenAccountForOrganization can hand back an existing account instead
	 * of opening a second one, and FindAccountForOrganization is O(1). Entries are removed on
	 * CloseAccount. */
	TMap<FOrganizationId, FBankAccountId> OwnerOrganizationToAccount;
};
