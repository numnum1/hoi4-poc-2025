#include "NationEntitySubsystem.h"

#include "NationFragments.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsNations, Log, All);

bool UNationEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UNationEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();
}

void UNationEntitySubsystem::Deinitialize()
{
	EntityByIndex.Reset();
	AliveIds.Reset();
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UNationEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

FNationId UNationEntitySubsystem::CreateNation()
{
	const FNationId NewId(EntityByIndex.Num());

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FNationIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FNationTag>()
	       .Add<FNationIndexFragment>(IndexFragment);

	EntityByIndex.Add(Builder.Commit());
	AliveIds.Add(NewId);
	return NewId;
}

void UNationEntitySubsystem::DestroyNation(const FNationId InNationId)
{
	const FMassEntityHandle Entity = GetEntity(InNationId);
	if (!Entity.IsValid())
	{
		return;
	}

	GetEntityManager().DestroyEntity(Entity);
	EntityByIndex[InNationId.Value] = FMassEntityHandle();
	AliveIds.RemoveSingle(InNationId);
}

void UNationEntitySubsystem::LogNation(const FNationId InNationId) const
{
	const FMassEntityHandle Entity = GetEntity(InNationId);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsNations, Warning, TEXT("[%d] no entity"), InNationId.Value);
		return;
	}

	UE_LOG(LogRtsNations, Log, TEXT("[%d] alive"), InNationId.Value);
}

static FAutoConsoleCommandWithWorldAndArgs GDumpNationsCommand(
	TEXT("Rts.DumpNations"),
	TEXT("Logs nation entity state. Optional argument: a single nation index."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const UNationEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UNationEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsNations, Warning, TEXT("No UNationEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() > 0)
			{
				Subsystem->LogNation(FNationId(FCString::Atoi(*Args[0])));
				return;
			}

			UE_LOG(LogRtsNations, Log, TEXT("=== Alive=%d Slots=%d ==="),
				Subsystem->GetAliveIds().Num(), Subsystem->NumSlots());
			for (int32 Index = 0; Index < Subsystem->NumSlots(); ++Index)
			{
				Subsystem->LogNation(FNationId(Index));
			}
		}));
