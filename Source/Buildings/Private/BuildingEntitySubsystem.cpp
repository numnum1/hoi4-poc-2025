#include "BuildingEntitySubsystem.h"

#include "BuildingFragments.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsBuildings, Log, All);

bool UBuildingEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UBuildingEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();

	LocationQuery = FMassEntityQuery(GetEntityManager().AsShared());
	LocationQuery.AddTagRequirement<FBuildingTag>(EMassFragmentPresence::All);
	LocationQuery.AddRequirement<FBuildingIndexFragment>(EMassFragmentAccess::ReadOnly);
	LocationQuery.AddRequirement<FBuildingLocationFragment>(EMassFragmentAccess::ReadOnly);

	CharacterOwnerQuery = FMassEntityQuery(GetEntityManager().AsShared());
	CharacterOwnerQuery.AddTagRequirement<FBuildingTag>(EMassFragmentPresence::All);
	CharacterOwnerQuery.AddRequirement<FBuildingIndexFragment>(EMassFragmentAccess::ReadOnly);
	CharacterOwnerQuery.AddRequirement<FBuildingCharacterOwnerFragment>(EMassFragmentAccess::ReadOnly);
}

void UBuildingEntitySubsystem::Deinitialize()
{
	EntityByIndex.Reset();
	AliveIds.Reset();
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UBuildingEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

FBuildingId UBuildingEntitySubsystem::CreateBuilding(const FProvinceId InProvinceId, const int32 InTypeIndex,
                                                     const FCharacterId InOwnerId)
{
	const FBuildingId NewId(EntityByIndex.Num());

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FBuildingIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	FBuildingLocationFragment LocationFragment;
	LocationFragment.ProvinceId = InProvinceId;

	FBuildingTypeFragment TypeFragment;
	TypeFragment.TypeIndex = InTypeIndex;

	FBuildingCharacterOwnerFragment OwnerFragment;
	OwnerFragment.OwnerId = InOwnerId;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FBuildingTag>()
	       .Add<FBuildingIndexFragment>(IndexFragment)
	       .Add<FBuildingLocationFragment>(LocationFragment)
	       .Add<FBuildingTypeFragment>(TypeFragment)
	       .Add<FBuildingCharacterOwnerFragment>(OwnerFragment);

	EntityByIndex.Add(Builder.Commit());
	AliveIds.Add(NewId);
	return NewId;
}

void UBuildingEntitySubsystem::DestroyBuilding(const FBuildingId InBuildingId)
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!Entity.IsValid())
	{
		return;
	}

	GetEntityManager().DestroyEntity(Entity);
	EntityByIndex[InBuildingId.Value] = FMassEntityHandle();
	AliveIds.RemoveSingle(InBuildingId);
}

FProvinceId UBuildingEntitySubsystem::GetProvince(const FBuildingId InBuildingId) const
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!Entity.IsValid())
	{
		return FProvinceId();
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBuildingLocationFragment>().ProvinceId;
}

int32 UBuildingEntitySubsystem::GetTypeIndex(const FBuildingId InBuildingId) const
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!Entity.IsValid())
	{
		return INDEX_NONE;
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBuildingTypeFragment>().TypeIndex;
}

FCharacterId UBuildingEntitySubsystem::GetOwnerCharacter(const FBuildingId InBuildingId) const
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!Entity.IsValid())
	{
		return FCharacterId();
	}

	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBuildingCharacterOwnerFragment>().OwnerId;
}

TArray<FBuildingId> UBuildingEntitySubsystem::CollectBuildingsInProvince(const FProvinceId InProvinceId) const
{
	TArray<FBuildingId> Result;

	FMassExecutionContext Context(GetEntityManager());
	LocationQuery.ForEachEntityChunk(Context, [&Result, InProvinceId](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FBuildingIndexFragment> Ids = Ctx.GetFragmentView<FBuildingIndexFragment>();
		const TConstArrayView<FBuildingLocationFragment> Locations = Ctx.GetFragmentView<FBuildingLocationFragment>();

		for (int32 EntityIt = 0; EntityIt < Ctx.GetNumEntities(); ++EntityIt)
		{
			if (Locations[EntityIt].ProvinceId == InProvinceId)
			{
				Result.Add(Ids[EntityIt].Id);
			}
		}
	});

	return Result;
}

TArray<FBuildingId> UBuildingEntitySubsystem::CollectBuildingsOwnedByCharacter(const FCharacterId InOwnerId) const
{
	TArray<FBuildingId> Result;

	FMassExecutionContext Context(GetEntityManager());
	CharacterOwnerQuery.ForEachEntityChunk(Context, [&Result, InOwnerId](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FBuildingIndexFragment> Ids = Ctx.GetFragmentView<FBuildingIndexFragment>();
		const TConstArrayView<FBuildingCharacterOwnerFragment> Owners = Ctx.GetFragmentView<FBuildingCharacterOwnerFragment>();

		for (int32 EntityIt = 0; EntityIt < Ctx.GetNumEntities(); ++EntityIt)
		{
			if (Owners[EntityIt].OwnerId == InOwnerId)
			{
				Result.Add(Ids[EntityIt].Id);
			}
		}
	});

	return Result;
}

void UBuildingEntitySubsystem::SetCharacterOwner(const FBuildingId InBuildingId, const FCharacterId InNewOwnerId)
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("SetCharacterOwner: building %d is not alive"), InBuildingId.Value))
	{
		return;
	}

	FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBuildingCharacterOwnerFragment>().OwnerId = InNewOwnerId;
}

void UBuildingEntitySubsystem::LogBuilding(const FBuildingId InBuildingId) const
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsBuildings, Warning, TEXT("[%d] no entity"), InBuildingId.Value);
		return;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	const FBuildingLocationFragment& Location = View.GetFragmentData<FBuildingLocationFragment>();
	const FBuildingTypeFragment& Type = View.GetFragmentData<FBuildingTypeFragment>();

	const FBuildingCharacterOwnerFragment& Owner = View.GetFragmentData<FBuildingCharacterOwnerFragment>();

	UE_LOG(LogRtsBuildings, Log, TEXT("[%d] Province=%d TypeIndex=%d OwnerCharacter=%d"),
		InBuildingId.Value, Location.ProvinceId.Value, Type.TypeIndex, Owner.OwnerId.Value);
}

static FAutoConsoleCommandWithWorldAndArgs GDumpBuildingsCommand(
	TEXT("Rts.DumpBuildings"),
	TEXT("Logs building entity state. Optional argument: a single building index."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const UBuildingEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UBuildingEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsBuildings, Warning, TEXT("No UBuildingEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() > 0)
			{
				Subsystem->LogBuilding(FBuildingId(FCString::Atoi(*Args[0])));
				return;
			}

			UE_LOG(LogRtsBuildings, Log, TEXT("=== Alive=%d Slots=%d ==="),
				Subsystem->GetAliveIds().Num(), Subsystem->NumSlots());
			for (int32 Index = 0; Index < Subsystem->NumSlots(); ++Index)
			{
				Subsystem->LogBuilding(FBuildingId(Index));
			}
		}));
