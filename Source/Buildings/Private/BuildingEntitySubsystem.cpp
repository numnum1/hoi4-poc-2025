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

	OrganizationOwnerQuery = FMassEntityQuery(GetEntityManager().AsShared());
	OrganizationOwnerQuery.AddTagRequirement<FBuildingTag>(EMassFragmentPresence::All);
	OrganizationOwnerQuery.AddRequirement<FBuildingIndexFragment>(EMassFragmentAccess::ReadOnly);
	OrganizationOwnerQuery.AddRequirement<FBuildingOrganizationOwnerFragment>(EMassFragmentAccess::ReadOnly);
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

FBuildingId UBuildingEntitySubsystem::CreateBuilding(const FProvinceId InProvinceId, const EBuildingKind InKind, UE::Mass::FEntityBuilder& InOutBuilder)
{
	const FBuildingId NewId(EntityByIndex.Num());

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FBuildingIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	FBuildingLocationFragment LocationFragment;
	LocationFragment.ProvinceId = InProvinceId;

	FBuildingKindFragment KindFragment;
	KindFragment.Kind = InKind;

	InOutBuilder.Add<FBuildingTag>()
	            .Add<FBuildingIndexFragment>(IndexFragment)
	            .Add<FBuildingLocationFragment>(LocationFragment)
	            .Add<FBuildingKindFragment>(KindFragment);

	EntityByIndex.Add(InOutBuilder.Commit());
	AliveIds.Add(NewId);
	return NewId;
}

FBuildingId UBuildingEntitySubsystem::CreateBuildingForCharacter(const FProvinceId InProvinceId, const EBuildingKind InKind,
                                                                 const FCharacterId InOwnerId)
{
	FBuildingCharacterOwnerFragment OwnerFragment;
	OwnerFragment.OwnerId = InOwnerId;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FBuildingCharacterOwnerFragment>(OwnerFragment);
	return CreateBuilding(InProvinceId, InKind, Builder);
}

FBuildingId UBuildingEntitySubsystem::CreateBuildingForOrganization(const FProvinceId InProvinceId, const EBuildingKind InKind,
                                                                    const FOrganizationId InOwnerId)
{
	FBuildingOrganizationOwnerFragment OwnerFragment;
	OwnerFragment.OwnerId = InOwnerId;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FBuildingOrganizationOwnerFragment>(OwnerFragment);
	return CreateBuilding(InProvinceId, InKind, Builder);
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

// -----------------------------------------------------------------------------
// Read
// -----------------------------------------------------------------------------

FProvinceId UBuildingEntitySubsystem::GetProvince(const FBuildingId InBuildingId) const
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!Entity.IsValid())
	{
		return FProvinceId();
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBuildingLocationFragment>().ProvinceId;
}

EBuildingKind UBuildingEntitySubsystem::GetKind(const FBuildingId InBuildingId) const
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!Entity.IsValid())
	{
		return EBuildingKind::BK_None;
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBuildingKindFragment>().Kind;
}

EBuildingOwnerType UBuildingEntitySubsystem::GetOwnerType(const FBuildingId InBuildingId) const
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!Entity.IsValid())
	{
		return EBuildingOwnerType::BOT_None;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	if (View.HasFragment<FBuildingCharacterOwnerFragment>())
	{
		return EBuildingOwnerType::BOT_Character;
	}
	if (View.HasFragment<FBuildingOrganizationOwnerFragment>())
	{
		return EBuildingOwnerType::BOT_Organization;
	}
	return EBuildingOwnerType::BOT_None;
}

FCharacterId UBuildingEntitySubsystem::GetOwnerCharacter(const FBuildingId InBuildingId) const
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!Entity.IsValid())
	{
		return FCharacterId();
	}

	const FBuildingCharacterOwnerFragment* OwnerFragment =
		FMassEntityView(GetEntityManager(), Entity).GetFragmentDataPtr<FBuildingCharacterOwnerFragment>();
	return OwnerFragment ? OwnerFragment->OwnerId : FCharacterId();
}

FOrganizationId UBuildingEntitySubsystem::GetOwnerOrganization(const FBuildingId InBuildingId) const
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!Entity.IsValid())
	{
		return FOrganizationId();
	}

	const FBuildingOrganizationOwnerFragment* OwnerFragment =
		FMassEntityView(GetEntityManager(), Entity).GetFragmentDataPtr<FBuildingOrganizationOwnerFragment>();
	return OwnerFragment ? OwnerFragment->OwnerId : FOrganizationId();
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

TArray<FBuildingId> UBuildingEntitySubsystem::CollectBuildingsOwnedByOrganization(const FOrganizationId InOwnerId) const
{
	TArray<FBuildingId> Result;

	FMassExecutionContext Context(GetEntityManager());
	OrganizationOwnerQuery.ForEachEntityChunk(Context, [&Result, InOwnerId](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FBuildingIndexFragment> Ids = Ctx.GetFragmentView<FBuildingIndexFragment>();
		const TConstArrayView<FBuildingOrganizationOwnerFragment> Owners = Ctx.GetFragmentView<FBuildingOrganizationOwnerFragment>();

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

// -----------------------------------------------------------------------------
// Ownership transfer
// -----------------------------------------------------------------------------

void UBuildingEntitySubsystem::SetCharacterOwner(const FBuildingId InBuildingId, const FCharacterId InNewOwnerId)
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("SetCharacterOwner: building %d is not alive"), InBuildingId.Value))
	{
		return;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	if (FBuildingCharacterOwnerFragment* Existing = View.GetFragmentDataPtr<FBuildingCharacterOwnerFragment>())
	{
		// Already character-owned — a value write, no archetype move.
		Existing->OwnerId = InNewOwnerId;
		return;
	}

	GetEntityManager().RemoveFragmentFromEntity(Entity, FBuildingOrganizationOwnerFragment::StaticStruct());
	GetEntityManager().AddFragmentToEntity(Entity, FBuildingCharacterOwnerFragment::StaticStruct());
	FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBuildingCharacterOwnerFragment>().OwnerId = InNewOwnerId;
}

void UBuildingEntitySubsystem::SetOrganizationOwner(const FBuildingId InBuildingId, const FOrganizationId InNewOwnerId)
{
	const FMassEntityHandle Entity = GetEntity(InBuildingId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("SetOrganizationOwner: building %d is not alive"), InBuildingId.Value))
	{
		return;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	if (FBuildingOrganizationOwnerFragment* Existing = View.GetFragmentDataPtr<FBuildingOrganizationOwnerFragment>())
	{
		// Already organization-owned — a value write, no archetype move.
		Existing->OwnerId = InNewOwnerId;
		return;
	}

	GetEntityManager().RemoveFragmentFromEntity(Entity, FBuildingCharacterOwnerFragment::StaticStruct());
	GetEntityManager().AddFragmentToEntity(Entity, FBuildingOrganizationOwnerFragment::StaticStruct());
	FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FBuildingOrganizationOwnerFragment>().OwnerId = InNewOwnerId;
}

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------

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
	const FBuildingKindFragment& Kind = View.GetFragmentData<FBuildingKindFragment>();

	FString OwnerDescription = TEXT("None");
	switch (GetOwnerType(InBuildingId))
	{
	case EBuildingOwnerType::BOT_Character:
		OwnerDescription = FString::Printf(TEXT("Character %d"), GetOwnerCharacter(InBuildingId).Value);
		break;
	case EBuildingOwnerType::BOT_Organization:
		OwnerDescription = FString::Printf(TEXT("Organization %d"), GetOwnerOrganization(InBuildingId).Value);
		break;
	default:
		break;
	}

	UE_LOG(LogRtsBuildings, Log, TEXT("[%d] Province=%d Kind=%s Owner=%s"),
		InBuildingId.Value, Location.ProvinceId.Value,
		*UEnum::GetValueAsString(Kind.Kind), *OwnerDescription);
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
