#include "ProvinceEntitySubsystem.h"

#include "ProvinceFragments.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsProvinces, Log, All);

bool UProvinceEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UProvinceEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();
}

void UProvinceEntitySubsystem::Deinitialize()
{
	EntityByIndex.Reset();
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UProvinceEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

void UProvinceEntitySubsystem::CreateProvinces(const TArray<FProvinceSpawnData>& InSpawnData)
{
	FMassEntityManager& EntityManager = GetEntityManager();

	if (EntityByIndex.Num() > 0)
	{
		EntityManager.BatchDestroyEntities(EntityByIndex);
		EntityByIndex.Reset();
	}

	EntityByIndex.Reserve(InSpawnData.Num());

	for (int32 ProvinceIndex = 0; ProvinceIndex < InSpawnData.Num(); ++ProvinceIndex)
	{
		// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
		FProvinceIndexFragment IndexFragment;
		IndexFragment.Index = ProvinceIndex;

		FProvinceOwnershipFragment OwnershipFragment;
		OwnershipFragment.OwningNationIndex = InSpawnData[ProvinceIndex].OwningNationIndex;

		UE::Mass::FEntityBuilder Builder = EntityManager.MakeEntityBuilder();
		Builder.Add<FProvinceTag>()
		       .Add<FProvinceIndexFragment>(IndexFragment)
		       .Add<FProvinceOwnershipFragment>(OwnershipFragment);

		EntityByIndex.Add(Builder.Commit());
	}

	UE_LOG(LogRtsProvinces, Log, TEXT("Created %d province entities"), EntityByIndex.Num());
}

// -----------------------------------------------------------------------------
// Read
// -----------------------------------------------------------------------------

int32 UProvinceEntitySubsystem::GetOwningNation(const int32 InProvinceIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InProvinceIndex);
	if (!Entity.IsValid())
	{
		return INDEX_NONE;
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FProvinceOwnershipFragment>().OwningNationIndex;
}

TArray<int32> UProvinceEntitySubsystem::CollectProvincesOfNation(const int32 InNationIndex) const
{
	TArray<int32> Result;
	for (int32 ProvinceIndex = 0; ProvinceIndex < EntityByIndex.Num(); ++ProvinceIndex)
	{
		if (GetOwningNation(ProvinceIndex) == InNationIndex)
		{
			Result.Add(ProvinceIndex);
		}
	}
	return Result;
}

// -----------------------------------------------------------------------------
// Write
// -----------------------------------------------------------------------------

void UProvinceEntitySubsystem::SetOwningNation(const int32 InProvinceIndex, const int32 InNationIndex)
{
	const FMassEntityHandle Entity = GetEntity(InProvinceIndex);
	if (!Entity.IsValid())
	{
		return;
	}

	FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FProvinceOwnershipFragment>().OwningNationIndex = InNationIndex;
}

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------

void UProvinceEntitySubsystem::LogProvince(const int32 InProvinceIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InProvinceIndex);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsProvinces, Warning, TEXT("[%d] no entity"), InProvinceIndex);
		return;
	}

	UE_LOG(LogRtsProvinces, Log, TEXT("[%d] OwningNation=%d"),
		InProvinceIndex, GetOwningNation(InProvinceIndex));
}

static FAutoConsoleCommandWithWorldAndArgs GDumpProvincesCommand(
	TEXT("Rts.DumpProvinces"),
	TEXT("Logs province entity state. Optional argument: a single province index."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const UProvinceEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UProvinceEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsProvinces, Warning, TEXT("No UProvinceEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() > 0)
			{
				Subsystem->LogProvince(FCString::Atoi(*Args[0]));
				return;
			}

			UE_LOG(LogRtsProvinces, Log, TEXT("=== %d provinces ==="), Subsystem->Num());
			for (int32 Index = 0; Index < Subsystem->Num(); ++Index)
			{
				Subsystem->LogProvince(Index);
			}
		}));
