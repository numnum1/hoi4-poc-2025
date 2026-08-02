#include "BankEntitySubsystem.h"

#include "BankFragments.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsBanking, Log, All);

bool UBankEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UBankEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();
}

void UBankEntitySubsystem::Deinitialize()
{
	EntityByIndex.Reset();
	AliveIds.Reset();
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UBankEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

FBankId UBankEntitySubsystem::CreateBank()
{
	const FBankId NewId(EntityByIndex.Num());

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FBankIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FBankTag>()
	       .Add<FBankIndexFragment>(IndexFragment);

	EntityByIndex.Add(Builder.Commit());
	AliveIds.Add(NewId);
	return NewId;
}

void UBankEntitySubsystem::DestroyBank(const FBankId InBankId)
{
	const FMassEntityHandle Entity = GetEntity(InBankId);
	if (!Entity.IsValid())
	{
		return;
	}

	GetEntityManager().DestroyEntity(Entity);
	EntityByIndex[InBankId.Value] = FMassEntityHandle();
	AliveIds.RemoveSingle(InBankId);
}

void UBankEntitySubsystem::LogBank(const FBankId InBankId) const
{
	const FMassEntityHandle Entity = GetEntity(InBankId);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsBanking, Warning, TEXT("[%d] no entity"), InBankId.Value);
		return;
	}

	UE_LOG(LogRtsBanking, Log, TEXT("[%d] alive"), InBankId.Value);
}

static FAutoConsoleCommandWithWorldAndArgs GDumpBanksCommand(
	TEXT("Rts.DumpBanks"),
	TEXT("Logs bank entity state. Optional argument: a single bank index."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const UBankEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UBankEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsBanking, Warning, TEXT("No UBankEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() > 0)
			{
				Subsystem->LogBank(FBankId(FCString::Atoi(*Args[0])));
				return;
			}

			UE_LOG(LogRtsBanking, Log, TEXT("=== Alive=%d Slots=%d ==="),
				Subsystem->GetAliveIds().Num(), Subsystem->NumSlots());
			for (int32 Index = 0; Index < Subsystem->NumSlots(); ++Index)
			{
				Subsystem->LogBank(FBankId(Index));
			}
		}));
