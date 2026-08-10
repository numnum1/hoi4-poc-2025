#include "ProvinceEntitySubsystem.h"

#include "ProvinceFragments.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"

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

	OwnershipQuery = FMassEntityQuery(GetEntityManager().AsShared());
	OwnershipQuery.AddTagRequirement<FProvinceTag>(EMassFragmentPresence::All);
	OwnershipQuery.AddRequirement<FProvinceIndexFragment>(EMassFragmentAccess::ReadOnly);
	OwnershipQuery.AddRequirement<FProvinceOwnershipFragment>(EMassFragmentAccess::ReadOnly);
}

void UProvinceEntitySubsystem::Deinitialize()
{
	EntityByIndex.Reset();
	AliveIds.Reset();
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UProvinceEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

FProvinceId UProvinceEntitySubsystem::CreateProvince(const FProvinceSpawnData& InSpawnData)
{
	const FProvinceId NewId(EntityByIndex.Num());

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FProvinceIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	FProvinceOwnershipFragment OwnershipFragment;
	OwnershipFragment.OwningNationId = InSpawnData.OwningNationId;

	FProvinceTimeFragment TimeFragment;
	TimeFragment.TimezoneOffset = InSpawnData.TimezoneOffset;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FProvinceTag>()
	       .Add<FProvinceIndexFragment>(IndexFragment)
	       .Add<FProvinceOwnershipFragment>(OwnershipFragment)
	       .Add<FProvinceTimeFragment>(TimeFragment);

	EntityByIndex.Add(Builder.Commit());
	AliveIds.Add(NewId);
	return NewId;
}

void UProvinceEntitySubsystem::DestroyProvince(const FProvinceId InProvinceId)
{
	const FMassEntityHandle Entity = GetEntity(InProvinceId);
	if (!Entity.IsValid())
	{
		return;
	}

	GetEntityManager().DestroyEntity(Entity);
	EntityByIndex[InProvinceId.Value] = FMassEntityHandle();
	AliveIds.RemoveSingle(InProvinceId);
}

namespace
{
	// A getter seeing an id that just died is normal UI behavior, not a bug — log quietly and
	// return the default, unlike the write functions below, which ensureMsgf's instead.
	void LogInvalidRead(const FProvinceId InProvinceId, const TCHAR* InAccessor)
	{
		UE_LOG(LogRtsProvinces, Verbose, TEXT("%s: province %d is not alive"), InAccessor, InProvinceId.Value);
	}
}

FNationId UProvinceEntitySubsystem::GetOwningNation(const FProvinceId InProvinceId) const
{
	const FMassEntityHandle Entity = GetEntity(InProvinceId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InProvinceId, TEXT("GetOwningNation"));
		return FNationId();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FProvinceOwnershipFragment>().OwningNationId;
}

int32 UProvinceEntitySubsystem::GetTimezoneOffset(const FProvinceId InProvinceId) const
{
	const FMassEntityHandle Entity = GetEntity(InProvinceId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InProvinceId, TEXT("GetTimezoneOffset"));
		return 0;
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FProvinceTimeFragment>().TimezoneOffset;
}

TArray<FProvinceId> UProvinceEntitySubsystem::CollectProvincesOfNation(const FNationId InNationId) const
{
	TArray<FProvinceId> Result;

	FMassExecutionContext Context(GetEntityManager());
	OwnershipQuery.ForEachEntityChunk(Context, [&Result, InNationId](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FProvinceIndexFragment> Ids = Ctx.GetFragmentView<FProvinceIndexFragment>();
		const TConstArrayView<FProvinceOwnershipFragment> Owners = Ctx.GetFragmentView<FProvinceOwnershipFragment>();

		for (int32 EntityIt = 0; EntityIt < Ctx.GetNumEntities(); ++EntityIt)
		{
			if (Owners[EntityIt].OwningNationId == InNationId)
			{
				Result.Add(Ids[EntityIt].Id);
			}
		}
	});

	return Result;
}

void UProvinceEntitySubsystem::SetOwningNation(const FProvinceId InProvinceId, const FNationId InNationId)
{
	const FMassEntityHandle Entity = GetEntity(InProvinceId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("SetOwningNation: province %d is not alive"), InProvinceId.Value))
	{
		return;
	}

	FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FProvinceOwnershipFragment>().OwningNationId = InNationId;
}

void UProvinceEntitySubsystem::LogProvince(const FProvinceId InProvinceId) const
{
	const FMassEntityHandle Entity = GetEntity(InProvinceId);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsProvinces, Warning, TEXT("[%d] no entity"), InProvinceId.Value);
		return;
	}

	UE_LOG(LogRtsProvinces, Log, TEXT("[%d] OwningNation=%d Tz=%d"),
		InProvinceId.Value, GetOwningNation(InProvinceId).Value, GetTimezoneOffset(InProvinceId));
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
				Subsystem->LogProvince(FProvinceId(FCString::Atoi(*Args[0])));
				return;
			}

			UE_LOG(LogRtsProvinces, Log, TEXT("=== Alive=%d Slots=%d ==="),
				Subsystem->GetAliveIds().Num(), Subsystem->NumSlots());
			for (int32 Index = 0; Index < Subsystem->NumSlots(); ++Index)
			{
				Subsystem->LogProvince(FProvinceId(Index));
			}
		}));
