#include "BankAccountEntitySubsystem.h"

#include "BankAccountFragments.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsBanking, Log, All);

bool UBankAccountEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UBankAccountEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();
}

void UBankAccountEntitySubsystem::Deinitialize()
{
	EntityByIndex.Reset();
	AliveIds.Reset();
	OwnerCharacterToAccounts.Reset();
	OwnerOrganizationToAccount.Reset();
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UBankAccountEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

FBankAccountId UBankAccountEntitySubsystem::OpenAccountForCharacter(const FBankId InBankId, const FCharacterId InOwnerId)
{
	const FBankAccountId NewId(EntityByIndex.Num());

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FBankAccountIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	FBankAccountBankFragment BankFragment;
	BankFragment.BankId = InBankId;

	FBankAccountCharacterOwnerFragment OwnerFragment;
	OwnerFragment.OwnerId = InOwnerId;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FBankAccountTag>()
	       .Add<FBankAccountIndexFragment>(IndexFragment)
	       .Add<FBankAccountBankFragment>(BankFragment)
	       .Add<FBankAccountBalanceFragment>()
	       .Add<FBankAccountCharacterOwnerFragment>(OwnerFragment);

	EntityByIndex.Add(Builder.Commit());
	AliveIds.Add(NewId);
	OwnerCharacterToAccounts.Add(InOwnerId, NewId);
	return NewId;
}

FBankAccountId UBankAccountEntitySubsystem::OpenAccountForOrganization(const FBankId InBankId, const FOrganizationId InOwnerId)
{
	if (const FBankAccountId* Existing = OwnerOrganizationToAccount.Find(InOwnerId))
	{
		return *Existing;
	}

	const FBankAccountId NewId(EntityByIndex.Num());

	FBankAccountIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	FBankAccountBankFragment BankFragment;
	BankFragment.BankId = InBankId;

	FBankAccountOrganizationOwnerFragment OwnerFragment;
	OwnerFragment.OwnerId = InOwnerId;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FBankAccountTag>()
	       .Add<FBankAccountIndexFragment>(IndexFragment)
	       .Add<FBankAccountBankFragment>(BankFragment)
	       .Add<FBankAccountBalanceFragment>()
	       .Add<FBankAccountOrganizationOwnerFragment>(OwnerFragment);

	EntityByIndex.Add(Builder.Commit());
	AliveIds.Add(NewId);
	OwnerOrganizationToAccount.Add(InOwnerId, NewId);
	return NewId;
}

void UBankAccountEntitySubsystem::CloseAccount(const FBankAccountId InAccountId)
{
	const FMassEntityHandle Entity = GetEntity(InAccountId);
	if (!Entity.IsValid())
	{
		return;
	}

	switch (GetOwnerType(InAccountId))
	{
	case EBankAccountOwnerType::BAOT_Character:
		// RemoveSingle, not Remove — a character can hold several accounts, so removing by key
		// alone would close every account it has, not just this one.
		OwnerCharacterToAccounts.RemoveSingle(GetOwnerCharacter(InAccountId), InAccountId);
		break;
	case EBankAccountOwnerType::BAOT_Organization:
		OwnerOrganizationToAccount.Remove(GetOwnerOrganization(InAccountId));
		break;
	default:
		break;
	}

	GetEntityManager().DestroyEntity(Entity);
	EntityByIndex[InAccountId.Value] = FMassEntityHandle();
	AliveIds.RemoveSingle(InAccountId);
}

// -----------------------------------------------------------------------------
// Read
// -----------------------------------------------------------------------------

FBankId UBankAccountEntitySubsystem::GetBankId(const FBankAccountId InAccountId) const
{
	const FMassEntityHandle Entity = GetEntity(InAccountId);
	if (!Entity.IsValid())
	{
		return FBankId();
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBankAccountBankFragment>().BankId;
}

int32 UBankAccountEntitySubsystem::GetBalance(const FBankAccountId InAccountId) const
{
	const FMassEntityHandle Entity = GetEntity(InAccountId);
	if (!Entity.IsValid())
	{
		return 0;
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBankAccountBalanceFragment>().Balance;
}

EBankAccountOwnerType UBankAccountEntitySubsystem::GetOwnerType(const FBankAccountId InAccountId) const
{
	const FMassEntityHandle Entity = GetEntity(InAccountId);
	if (!Entity.IsValid())
	{
		return EBankAccountOwnerType::BAOT_None;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	if (View.HasFragment<FBankAccountCharacterOwnerFragment>())
	{
		return EBankAccountOwnerType::BAOT_Character;
	}
	if (View.HasFragment<FBankAccountOrganizationOwnerFragment>())
	{
		return EBankAccountOwnerType::BAOT_Organization;
	}
	return EBankAccountOwnerType::BAOT_None;
}

FCharacterId UBankAccountEntitySubsystem::GetOwnerCharacter(const FBankAccountId InAccountId) const
{
	const FMassEntityHandle Entity = GetEntity(InAccountId);
	if (!Entity.IsValid())
	{
		return FCharacterId();
	}

	const FBankAccountCharacterOwnerFragment* OwnerFragment =
		FMassEntityView(GetEntityManager(), Entity).GetFragmentDataPtr<FBankAccountCharacterOwnerFragment>();
	return OwnerFragment ? OwnerFragment->OwnerId : FCharacterId();
}

FOrganizationId UBankAccountEntitySubsystem::GetOwnerOrganization(const FBankAccountId InAccountId) const
{
	const FMassEntityHandle Entity = GetEntity(InAccountId);
	if (!Entity.IsValid())
	{
		return FOrganizationId();
	}

	const FBankAccountOrganizationOwnerFragment* OwnerFragment =
		FMassEntityView(GetEntityManager(), Entity).GetFragmentDataPtr<FBankAccountOrganizationOwnerFragment>();
	return OwnerFragment ? OwnerFragment->OwnerId : FOrganizationId();
}

TArray<FBankAccountId> UBankAccountEntitySubsystem::GetAccountsForCharacter(const FCharacterId InOwnerId) const
{
	TArray<FBankAccountId> Result;
	OwnerCharacterToAccounts.MultiFind(InOwnerId, Result);
	return Result;
}

FBankAccountId UBankAccountEntitySubsystem::FindAccountForOrganization(const FOrganizationId InOwnerId) const
{
	const FBankAccountId* Found = OwnerOrganizationToAccount.Find(InOwnerId);
	return Found ? *Found : FBankAccountId();
}

// -----------------------------------------------------------------------------
// Write
// -----------------------------------------------------------------------------

void UBankAccountEntitySubsystem::Deposit(const FBankAccountId InAccountId, const int32 InAmount)
{
	const FMassEntityHandle Entity = GetEntity(InAccountId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("Deposit: account %d is not alive"), InAccountId.Value))
	{
		return;
	}

	FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBankAccountBalanceFragment>().Balance += InAmount;
}

bool UBankAccountEntitySubsystem::Withdraw(const FBankAccountId InAccountId, const int32 InAmount)
{
	const FMassEntityHandle Entity = GetEntity(InAccountId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("Withdraw: account %d is not alive"), InAccountId.Value))
	{
		return false;
	}

	int32& Balance = FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBankAccountBalanceFragment>().Balance;
	if (InAmount > Balance)
	{
		return false;
	}

	Balance -= InAmount;
	return true;
}

bool UBankAccountEntitySubsystem::Transfer(const FBankAccountId InFromId, const FBankAccountId InToId, const int32 InAmount)
{
	if (!Withdraw(InFromId, InAmount))
	{
		return false;
	}

	Deposit(InToId, InAmount);
	return true;
}

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------

void UBankAccountEntitySubsystem::LogAccount(const FBankAccountId InAccountId) const
{
	const FMassEntityHandle Entity = GetEntity(InAccountId);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsBanking, Warning, TEXT("[%d] no entity"), InAccountId.Value);
		return;
	}

	FString OwnerDescription = TEXT("None");
	switch (GetOwnerType(InAccountId))
	{
	case EBankAccountOwnerType::BAOT_Character:
		OwnerDescription = FString::Printf(TEXT("Character %d"), GetOwnerCharacter(InAccountId).Value);
		break;
	case EBankAccountOwnerType::BAOT_Organization:
		OwnerDescription = FString::Printf(TEXT("Organization %d"), GetOwnerOrganization(InAccountId).Value);
		break;
	default:
		break;
	}

	UE_LOG(LogRtsBanking, Log, TEXT("[%d] Bank=%d Balance=%d Owner=%s"),
		InAccountId.Value, GetBankId(InAccountId).Value, GetBalance(InAccountId), *OwnerDescription);
}

static FAutoConsoleCommandWithWorldAndArgs GDumpBankAccountsCommand(
	TEXT("Rts.DumpBankAccounts"),
	TEXT("Logs bank account entity state. Optional argument: a single account index."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const UBankAccountEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UBankAccountEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsBanking, Warning, TEXT("No UBankAccountEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() > 0)
			{
				Subsystem->LogAccount(FBankAccountId(FCString::Atoi(*Args[0])));
				return;
			}

			UE_LOG(LogRtsBanking, Log, TEXT("=== Alive=%d Slots=%d ==="),
				Subsystem->GetAliveIds().Num(), Subsystem->NumSlots());
			for (int32 Index = 0; Index < Subsystem->NumSlots(); ++Index)
			{
				Subsystem->LogAccount(FBankAccountId(Index));
			}
		}));
