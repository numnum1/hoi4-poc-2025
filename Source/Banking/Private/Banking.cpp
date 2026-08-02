#include "Banking.h"

#include "BankAccountEntitySubsystem.h"
#include "BankEntitySubsystem.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "FBankingModule"

void FBankingModule::StartupModule()
{
}

void FBankingModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBankingModule, Banking)

UBanking::UBanking()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

UBankEntitySubsystem* UBanking::GetBanks() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UBankEntitySubsystem>() : nullptr;
}

UBankAccountEntitySubsystem* UBanking::GetAccounts() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UBankAccountEntitySubsystem>() : nullptr;
}

bool UBanking::IsBankAlive(const FBankId InBankId) const
{
	const UBankEntitySubsystem* Banks = GetBanks();
	return Banks && Banks->IsAlive(InBankId);
}

FBankInfo UBanking::GetBankInfo(const FBankId InBankId) const
{
	return BankInfos.IsValidIndex(InBankId.Value) ? BankInfos[InBankId.Value] : FBankInfo();
}

FBankId UBanking::AddBank(const FBankInfo& NewBank)
{
	UBankEntitySubsystem* Banks = GetBanks();
	if (Banks == nullptr)
	{
		return FBankId();
	}

	// The entity claims the id; the display row must line up with it.
	const FBankId NewId = Banks->CreateBank();
	check(NewId.Value == BankInfos.Num());
	BankInfos.Add(NewBank);

	OnBankAdded.Broadcast();
	return NewId;
}

FBankAccountId UBanking::OpenAccountForCharacter(const FBankId InBankId, const FCharacterId InOwnerId)
{
	UBankAccountEntitySubsystem* Accounts = GetAccounts();
	if (Accounts == nullptr)
	{
		return FBankAccountId();
	}

	const FBankAccountId NewId = Accounts->OpenAccountForCharacter(InBankId, InOwnerId);
	OnAccountOpened.Broadcast(NewId);
	return NewId;
}

FBankAccountId UBanking::OpenAccountForOrganization(const FBankId InBankId, const FOrganizationId InOwnerId)
{
	UBankAccountEntitySubsystem* Accounts = GetAccounts();
	if (Accounts == nullptr)
	{
		return FBankAccountId();
	}

	const FBankAccountId NewId = Accounts->OpenAccountForOrganization(InBankId, InOwnerId);
	OnAccountOpened.Broadcast(NewId);
	return NewId;
}

int32 UBanking::GetBalance(const FBankAccountId InAccountId) const
{
	const UBankAccountEntitySubsystem* Accounts = GetAccounts();
	return Accounts ? Accounts->GetBalance(InAccountId) : 0;
}

FBankAccountId UBanking::FindAccountForCharacter(const FCharacterId InOwnerId) const
{
	const UBankAccountEntitySubsystem* Accounts = GetAccounts();
	return Accounts ? Accounts->FindAccountForCharacter(InOwnerId) : FBankAccountId();
}

FBankAccountId UBanking::FindAccountForOrganization(const FOrganizationId InOwnerId) const
{
	const UBankAccountEntitySubsystem* Accounts = GetAccounts();
	return Accounts ? Accounts->FindAccountForOrganization(InOwnerId) : FBankAccountId();
}

void UBanking::Deposit(const FBankAccountId InAccountId, const int32 InAmount)
{
	UBankAccountEntitySubsystem* Accounts = GetAccounts();
	if (Accounts == nullptr)
	{
		return;
	}

	Accounts->Deposit(InAccountId, InAmount);
	OnBalanceChanged.Broadcast(InAccountId);
}

bool UBanking::Withdraw(const FBankAccountId InAccountId, const int32 InAmount)
{
	UBankAccountEntitySubsystem* Accounts = GetAccounts();
	if (Accounts == nullptr)
	{
		return false;
	}

	if (!Accounts->Withdraw(InAccountId, InAmount))
	{
		return false;
	}

	OnBalanceChanged.Broadcast(InAccountId);
	return true;
}

bool UBanking::Transfer(const FBankAccountId InFromId, const FBankAccountId InToId, const int32 InAmount)
{
	UBankAccountEntitySubsystem* Accounts = GetAccounts();
	if (Accounts == nullptr)
	{
		return false;
	}

	if (!Accounts->Transfer(InFromId, InToId, InAmount))
	{
		return false;
	}

	OnBalanceChanged.Broadcast(InFromId);
	OnBalanceChanged.Broadcast(InToId);
	return true;
}
