#include "OrganizationEntitySubsystem.h"

#include "OrganizationFragments.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsOrganizations, Log, All);

bool UOrganizationEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UOrganizationEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();

	SettledNationsQuery = FMassEntityQuery(GetEntityManager().AsShared());
	SettledNationsQuery.AddTagRequirement<FOrganizationTag>(EMassFragmentPresence::All);
	SettledNationsQuery.AddRequirement<FOrganizationIndexFragment>(EMassFragmentAccess::ReadOnly);
	SettledNationsQuery.AddRequirement<FOrganizationSettledNationsFragment>(EMassFragmentAccess::ReadOnly);
}

void UOrganizationEntitySubsystem::Deinitialize()
{
	EntityByIndex.Reset();
	AliveIds.Reset();
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UOrganizationEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

FOrganizationId UOrganizationEntitySubsystem::CreateOrganization(const FCharacterId InOwnerId)
{
	const FOrganizationId NewId(EntityByIndex.Num());

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FOrganizationIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	FOrganizationOwnerFragment OwnerFragment;
	OwnerFragment.OwnerId = InOwnerId;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	// The headquarters starts out invalid and is filled in by SetHeadquarters: the building has to
	// be created for this organization, so the id it needs cannot exist before this call returns.
	Builder.Add<FOrganizationTag>()
	       .Add<FOrganizationIndexFragment>(IndexFragment)
	       .Add<FOrganizationOwnerFragment>(OwnerFragment)
	       .Add<FOrganizationAlignmentFragment>()
	       .Add<FOrganizationSettledNationsFragment>()
	       .Add<FOrganizationHeadquartersFragment>();

	EntityByIndex.Add(Builder.Commit());
	AliveIds.Add(NewId);
	return NewId;
}

void UOrganizationEntitySubsystem::DestroyOrganization(const FOrganizationId InOrganizationId)
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		return;
	}

	GetEntityManager().DestroyEntity(Entity);
	EntityByIndex[InOrganizationId.Value] = FMassEntityHandle();
	AliveIds.RemoveSingle(InOrganizationId);
}

void UOrganizationEntitySubsystem::Reset()
{
	if (EntityByIndex.Num() > 0)
	{
		GetEntityManager().BatchDestroyEntities(EntityByIndex);
		EntityByIndex.Reset();
		AliveIds.Reset();
	}
}

// -----------------------------------------------------------------------------
// Read
// -----------------------------------------------------------------------------

namespace
{
	// A getter seeing an id that just died is normal UI behavior, not a bug — log quietly and
	// return the default. Contrast with the Write section below, which ensureMsgf's instead.
	void LogInvalidRead(const FOrganizationId InOrganizationId, const TCHAR* InAccessor)
	{
		UE_LOG(LogRtsOrganizations, Verbose, TEXT("%s: organization %d is not alive"), InAccessor, InOrganizationId.Value);
	}
}

FCharacterId UOrganizationEntitySubsystem::GetOwnerCharacter(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InOrganizationId, TEXT("GetOwnerCharacter"));
		return FCharacterId();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationOwnerFragment>().OwnerId;
}

FBuildingId UOrganizationEntitySubsystem::GetHeadquarters(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InOrganizationId, TEXT("GetHeadquarters"));
		return FBuildingId();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationHeadquartersFragment>().BuildingId;
}

void UOrganizationEntitySubsystem::SetHeadquarters(const FOrganizationId InOrganizationId, const FBuildingId InBuildingId)
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("SetHeadquarters: organization %d is not alive"), InOrganizationId.Value))
	{
		return;
	}

	FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationHeadquartersFragment>().BuildingId = InBuildingId;
}

bool UOrganizationEntitySubsystem::IsAlignmentPublished(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InOrganizationId, TEXT("IsAlignmentPublished"));
		return false;
	}
	return FMassEntityView(GetEntityManager(), Entity).HasTag<FOrganizationPublishedTag>();
}

FOrganizationAlignment UOrganizationEntitySubsystem::GetAlignment(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InOrganizationId, TEXT("GetAlignment"));
		return FOrganizationAlignment();
	}

	FOrganizationAlignment Result;
	Result.bPublished = IsAlignmentPublished(InOrganizationId);
	Result.Branches = FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationAlignmentFragment>().Branches;
	return Result;
}

TArray<FNationId> UOrganizationEntitySubsystem::GetSettledNations(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InOrganizationId, TEXT("GetSettledNations"));
		return {};
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationSettledNationsFragment>().Values;
}

TArray<FOrganizationId> UOrganizationEntitySubsystem::CollectOrganizationsSettledIn(const FNationId InNationId) const
{
	TArray<FOrganizationId> Result;

	FMassExecutionContext Context(GetEntityManager());
	SettledNationsQuery.ForEachEntityChunk(Context, [&Result, InNationId](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FOrganizationIndexFragment> Ids = Ctx.GetFragmentView<FOrganizationIndexFragment>();
		const TConstArrayView<FOrganizationSettledNationsFragment> Settled =
			Ctx.GetFragmentView<FOrganizationSettledNationsFragment>();

		for (int32 EntityIt = 0; EntityIt < Ctx.GetNumEntities(); ++EntityIt)
		{
			if (Settled[EntityIt].Values.Contains(InNationId))
			{
				Result.Add(Ids[EntityIt].Id);
			}
		}
	});

	return Result;
}

// -----------------------------------------------------------------------------
// Write
// -----------------------------------------------------------------------------

void UOrganizationEntitySubsystem::PublishAlignment(const FOrganizationId InOrganizationId, const TArray<float>& InAlignment)
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("PublishAlignment: organization %d is not alive"), InOrganizationId.Value))
	{
		return;
	}

	FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationAlignmentFragment>().Branches = InAlignment;

	// Moves the entity to the published archetype. Idempotent, so no need to check first.
	GetEntityManager().AddTagToEntity(Entity, FOrganizationPublishedTag::StaticStruct());
}

bool UOrganizationEntitySubsystem::SettleNation(const FOrganizationId InOrganizationId, const FNationId InNationId)
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("SettleNation: organization %d is not alive"), InOrganizationId.Value))
	{
		return false;
	}

	TArray<FNationId>& Settled = FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationSettledNationsFragment>().Values;

	if (Settled.Contains(InNationId))
	{
		return false;
	}

	Settled.Add(InNationId);
	return true;
}

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------

void UOrganizationEntitySubsystem::LogOrganization(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsOrganizations, Warning, TEXT("[%d] no entity"), InOrganizationId.Value);
		return;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	const FOrganizationOwnerFragment& Owner = View.GetFragmentData<FOrganizationOwnerFragment>();
	const FOrganizationAlignmentFragment& Alignment = View.GetFragmentData<FOrganizationAlignmentFragment>();
	const FOrganizationSettledNationsFragment& Settled = View.GetFragmentData<FOrganizationSettledNationsFragment>();
	const FOrganizationHeadquartersFragment& Headquarters = View.GetFragmentData<FOrganizationHeadquartersFragment>();

	UE_LOG(LogRtsOrganizations, Log, TEXT("[%d] Owner=%d Published=%d Branches=%d SettledNations=%d Headquarters=%d"),
		InOrganizationId.Value, Owner.OwnerId.Value, IsAlignmentPublished(InOrganizationId) ? 1 : 0,
		Alignment.Branches.Num(), Settled.Values.Num(), Headquarters.BuildingId.Value);
}

static FAutoConsoleCommandWithWorldAndArgs GDumpOrganizationsCommand(
	TEXT("Rts.DumpOrganizations"),
	TEXT("Logs organization entity state. Optional argument: a single organization index."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const UOrganizationEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UOrganizationEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsOrganizations, Warning, TEXT("No UOrganizationEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() > 0)
			{
				Subsystem->LogOrganization(FOrganizationId(FCString::Atoi(*Args[0])));
				return;
			}

			UE_LOG(LogRtsOrganizations, Log, TEXT("=== Alive=%d Slots=%d ==="),
				Subsystem->GetAliveIds().Num(), Subsystem->NumSlots());
			for (int32 Index = 0; Index < Subsystem->NumSlots(); ++Index)
			{
				Subsystem->LogOrganization(FOrganizationId(Index));
			}
		}));
