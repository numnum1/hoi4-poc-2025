#include "Banking.h"

#include "BankAccountEntitySubsystem.h"
#include "BankEntitySubsystem.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsBankingFacade, Log, All);

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

FBankId UBanking::GetBank(const FBankAccountId InAccountId) const
{
	const UBankAccountEntitySubsystem* Accounts = GetAccounts();
	return Accounts ? Accounts->GetBankId(InAccountId) : FBankId();
}

TArray<FBankAccountId> UBanking::GetAccountsForCharacter(const FCharacterId InOwnerId) const
{
	const UBankAccountEntitySubsystem* Accounts = GetAccounts();
	return Accounts ? Accounts->GetAccountsForCharacter(InOwnerId) : TArray<FBankAccountId>();
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

namespace
{
	/** No InAmount clamp beyond what Withdraw already refuses (an overdraft) — the point of this
	 * command is to force an account negative-relative-to-obligations for testing URtsEconomy's
	 * insolvency path, not to model a real transaction. */
	static FAutoConsoleCommandWithWorldAndArgs GSetBalanceCommand(
		TEXT("Rts.SetBalance"),
		TEXT("Rts.SetBalance <AccountIdx> <Amount> - sets a bank account's balance through UBanking."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
			[](const TArray<FString>& Args, UWorld* World)
			{
				const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
				UBanking* Banking = GameState ? GameState->FindComponentByClass<UBanking>() : nullptr;
				if (!Banking)
				{
					UE_LOG(LogRtsBankingFacade, Warning, TEXT("No UBanking component in this world"));
					return;
				}

				if (Args.Num() < 2)
				{
					UE_LOG(LogRtsBankingFacade, Warning, TEXT("Usage: Rts.SetBalance <AccountIdx> <Amount>"));
					return;
				}

				const FBankAccountId AccountId(FCString::Atoi(*Args[0]));
				const int32 TargetAmount = FCString::Atoi(*Args[1]);
				const int32 Delta = TargetAmount - Banking->GetBalance(AccountId);

				if (Delta > 0)
				{
					Banking->Deposit(AccountId, Delta);
				}
				else if (Delta < 0)
				{
					Banking->Withdraw(AccountId, -Delta);
				}

				UE_LOG(LogRtsBankingFacade, Log, TEXT("Account %d balance is now %d"), AccountId.Value, Banking->GetBalance(AccountId));
			}));
}
