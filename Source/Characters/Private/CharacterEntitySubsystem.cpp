#include "CharacterEntitySubsystem.h"

#include "CharacterFragments.h"
#include "CharacterHourlySimProcessor.h"
#include "ProvinceEntitySubsystem.h"
#include "Time.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsCharacters, Log, All);

bool UCharacterEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Only game worlds run a scenario; editor preview worlds have no characters to own.
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UCharacterEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();

	LocationQuery = FMassEntityQuery(GetEntityManager().AsShared());
	LocationQuery.AddTagRequirement<FCharacterTag>(EMassFragmentPresence::All);
	LocationQuery.AddRequirement<FCharacterIndexFragment>(EMassFragmentAccess::ReadOnly);
	LocationQuery.AddRequirement<FCharacterLocationFragment>(EMassFragmentAccess::ReadOnly);
}

void UCharacterEntitySubsystem::Deinitialize()
{
	if (Time)
	{
		Time->OnHourElapsed.RemoveDynamic(this, &UCharacterEntitySubsystem::HandleHourElapsed);
		Time = nullptr;
	}

	EntityByIndex.Reset();
	AliveIds.Reset();
	SimProcessor = nullptr;
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UCharacterEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

FCharacterId UCharacterEntitySubsystem::CreateCharacter(const FCharacterSpawnData& Data)
{
	const FCharacterId NewId(EntityByIndex.Num());

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FCharacterIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	FCharacterOriginFragment OriginFragment;
	OriginFragment.BirthProvinceId = Data.BirthProvinceId;
	OriginFragment.BaseRaceId = Data.BaseRaceId;

	FCharacterLocationFragment LocationFragment;
	LocationFragment.CurrentProvinceId = Data.CurrentProvinceId;
	LocationFragment.TimezoneOffset = Data.TimezoneOffset;

	FCharacterAbilitiesFragment AbilitiesFragment;
	AbilitiesFragment.Abilities = Data.RaceModifiers;

	FCharacterScheduleSharedFragment ScheduleFragment;
	ScheduleFragment.Schedule = Data.Schedule;
	ScheduleFragment.ScheduleId = Data.ScheduleId;

	FCharacterRaceSharedFragment RaceFragment;
	RaceFragment.RaceId = Data.BaseRaceId;
	RaceFragment.RaceModifiers = Data.RaceModifiers;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();

	Builder.Add<FCharacterTag>()
	       .Add<FCharacterIndexFragment>(IndexFragment)
	       .Add<FCharacterOriginFragment>(OriginFragment)
	       .Add<FCharacterLocationFragment>(LocationFragment)
	       .Add<FCharacterAbilitiesFragment>(AbilitiesFragment)
	       .Add<FCharacterVitalsFragment>()
	       .Add<FCharacterSkillsFragment>()
	       .Add<FCharacterPositionsFragment>()
	       .Add<FCharacterScheduleSharedFragment>(ScheduleFragment)
	       .Add<FCharacterRaceSharedFragment>(RaceFragment);

	switch (Data.SimTier)
	{
	case ECharacterSimTier::CST_Normal:
		Builder.Add<FCharacterSimNormalTag>();
		break;
	case ECharacterSimTier::CST_Max:
		Builder.Add<FCharacterSimMaxTag>();
		break;
	case ECharacterSimTier::CST_None:
	default:
		break;
	}

	EntityByIndex.Add(Builder.Commit());
	AliveIds.Add(NewId);

	return NewId;
}

void UCharacterEntitySubsystem::DestroyCharacter(const FCharacterId InCharacterId)
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!Entity.IsValid())
	{
		return;
	}

	GetEntityManager().DestroyEntity(Entity);
	EntityByIndex[InCharacterId.Value] = FMassEntityHandle();
	AliveIds.RemoveSingle(InCharacterId);
}

void UCharacterEntitySubsystem::BindToTime(UTime* InTime)
{
	if (Time == InTime)
	{
		return;
	}

	if (Time)
	{
		Time->OnHourElapsed.RemoveDynamic(this, &UCharacterEntitySubsystem::HandleHourElapsed);
	}

	Time = InTime;

	if (Time)
	{
		Time->OnHourElapsed.AddDynamic(this, &UCharacterEntitySubsystem::HandleHourElapsed);
	}
}

void UCharacterEntitySubsystem::HandleHourElapsed(const int32 InHoursElapsed)
{
	if (InHoursElapsed <= 0 || AliveIds.Num() == 0 || !Time)
	{
		return;
	}

	if (!SimProcessor)
	{
		SimProcessor = NewObject<UCharacterHourlySimProcessor>(this);
		SimProcessor->CallInitialize(this, GetEntityManager().AsShared());
	}

	// CurrentTime already sits past the hours we are about to simulate, so wind back.
	const int32 Shifted = Time->GetCurrentTime().GetHour() - InHoursElapsed;
	SimProcessor->SetStep(static_cast<uint8>(((Shifted % 24) + 24) % 24), InHoursElapsed);

	UE::Mass::FProcessingContext ProcessingContext(GetEntityManager(), /*DeltaSeconds=*/0.f, /*bFlushCommandBuffer=*/true);
	UE::Mass::Executor::Run(*SimProcessor, ProcessingContext);

	for (const FCharacterId SimulatedId : SimProcessor->GetLastSimulatedIds())
	{
		OnCharacterSimulated.Broadcast(SimulatedId);
	}
}

namespace
{
	// A getter seeing an id that just died is normal UI behavior, not a bug — log quietly and
	// return the default, unlike the write functions below, which ensureMsgf's instead.
	void LogInvalidRead(const FCharacterId InCharacterId, const TCHAR* InAccessor)
	{
		UE_LOG(LogRtsCharacters, Verbose, TEXT("%s: character %d is not alive"), InAccessor, InCharacterId.Value);
	}
}

FProvinceId UCharacterEntitySubsystem::GetCurrentProvince(const FCharacterId InCharacterId) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InCharacterId, TEXT("GetCurrentProvince"));
		return FProvinceId();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FCharacterLocationFragment>().CurrentProvinceId;
}

int32 UCharacterEntitySubsystem::GetTimezoneOffset(const FCharacterId InCharacterId) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InCharacterId, TEXT("GetTimezoneOffset"));
		return 0;
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FCharacterLocationFragment>().TimezoneOffset;
}

FAbilities UCharacterEntitySubsystem::GetAbilities(const FCharacterId InCharacterId) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InCharacterId, TEXT("GetAbilities"));
		return FAbilities();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FCharacterAbilitiesFragment>().Abilities;
}

TArray<FPosition> UCharacterEntitySubsystem::GetPositions(const FCharacterId InCharacterId) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InCharacterId, TEXT("GetPositions"));
		return {};
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FCharacterPositionsFragment>().Values;
}

FSchedule UCharacterEntitySubsystem::GetSchedule(const FCharacterId InCharacterId) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InCharacterId, TEXT("GetSchedule"));
		return FSchedule();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetConstSharedFragmentData<FCharacterScheduleSharedFragment>().Schedule;
}

FScheduleId UCharacterEntitySubsystem::GetScheduleId(const FCharacterId InCharacterId) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InCharacterId, TEXT("GetScheduleId"));
		return FScheduleId();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetConstSharedFragmentData<FCharacterScheduleSharedFragment>().ScheduleId;
}

FCharacterVitals UCharacterEntitySubsystem::GetVitals(const FCharacterId InCharacterId) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InCharacterId, TEXT("GetVitals"));
		return FCharacterVitals();
	}
	const FCharacterVitalsFragment& Vitals =
		FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FCharacterVitalsFragment>();

	FCharacterVitals Result;
	Result.Energy = Vitals.Energy;
	Result.Happiness = Vitals.Happiness;
	return Result;
}

FCharacterSkills UCharacterEntitySubsystem::GetSkills(const FCharacterId InCharacterId) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InCharacterId, TEXT("GetSkills"));
		return FCharacterSkills();
	}
	const FCharacterSkillsFragment& Skills =
		FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FCharacterSkillsFragment>();

	FCharacterSkills Result;
	Result.Science = Skills.Science;
	Result.Strength = Skills.Strength;
	Result.Medicine = Skills.Medicine;
	return Result;
}

TArray<FCharacterId> UCharacterEntitySubsystem::CollectCharactersInProvince(const FProvinceId InProvinceId) const
{
	TArray<FCharacterId> Result;

	FMassExecutionContext Context(GetEntityManager());
	LocationQuery.ForEachEntityChunk(Context, [&Result, InProvinceId](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FCharacterIndexFragment> Ids = Ctx.GetFragmentView<FCharacterIndexFragment>();
		const TConstArrayView<FCharacterLocationFragment> Locations = Ctx.GetFragmentView<FCharacterLocationFragment>();

		for (int32 EntityIt = 0; EntityIt < Ctx.GetNumEntities(); ++EntityIt)
		{
			if (Locations[EntityIt].CurrentProvinceId == InProvinceId)
			{
				Result.Add(Ids[EntityIt].Id);
			}
		}
	});

	return Result;
}

void UCharacterEntitySubsystem::MoveCharacter(const FCharacterId InCharacterId, const FProvinceId InNewProvinceId)
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("MoveCharacter: character %d is not alive"), InCharacterId.Value))
	{
		return;
	}

	int32 NewTimezoneOffset = 0;
	if (const UProvinceEntitySubsystem* Provinces = GetWorld() ? GetWorld()->GetSubsystem<UProvinceEntitySubsystem>() : nullptr)
	{
		NewTimezoneOffset = Provinces->GetTimezoneOffset(InNewProvinceId);
	}

	FCharacterLocationFragment& Location =
		FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FCharacterLocationFragment>();
	Location.CurrentProvinceId = InNewProvinceId;
	Location.TimezoneOffset = NewTimezoneOffset;
}

void UCharacterEntitySubsystem::AddPosition(const FCharacterId InCharacterId, const FOrganizationId InAtOrganizationId, const FRoleId InRoleId)
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("AddPosition: character %d is not alive"), InCharacterId.Value))
	{
		return;
	}

	TArray<FPosition>& Positions =
		FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FCharacterPositionsFragment>().Values;

	const bool bAlreadyHeld = Positions.ContainsByPredicate([&](const FPosition& Existing)
	{
		return Existing.OrganizationId == InAtOrganizationId && Existing.RoleId == InRoleId;
	});
	if (bAlreadyHeld)
	{
		return;
	}

	FPosition NewPosition;
	NewPosition.OrganizationId = InAtOrganizationId;
	NewPosition.RoleId = InRoleId;
	Positions.Add(NewPosition);
}

void UCharacterEntitySubsystem::RemovePosition(const FCharacterId InCharacterId, const FOrganizationId InAtOrganizationId)
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("RemovePosition: character %d is not alive"), InCharacterId.Value))
	{
		return;
	}

	TArray<FPosition>& Positions =
		FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FCharacterPositionsFragment>().Values;

	Positions.RemoveAll([&](const FPosition& Existing)
	{
		return Existing.OrganizationId == InAtOrganizationId;
	});
}

void UCharacterEntitySubsystem::LogCharacter(const FCharacterId InCharacterId) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterId);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsCharacters, Warning, TEXT("[%d] no entity"), InCharacterId.Value);
		return;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	const FCharacterLocationFragment& Location = View.GetFragmentData<FCharacterLocationFragment>();
	const FCharacterVitalsFragment& Vitals = View.GetFragmentData<FCharacterVitalsFragment>();
	const FCharacterSkillsFragment& Skills = View.GetFragmentData<FCharacterSkillsFragment>();
	const FCharacterPositionsFragment& Positions = View.GetFragmentData<FCharacterPositionsFragment>();

	UE_LOG(LogRtsCharacters, Log,
		TEXT("[%d] Province=%d Tz=%d | Energy=%d Happiness=%d | Sci=%d Str=%d Med=%d | Positions=%d"),
		InCharacterId.Value, Location.CurrentProvinceId.Value, Location.TimezoneOffset,
		Vitals.Energy, Vitals.Happiness,
		Skills.Science, Skills.Strength, Skills.Medicine,
		Positions.Values.Num());
}

static FAutoConsoleCommandWithWorldAndArgs GDumpCharactersCommand(
	TEXT("Rts.DumpCharacters"),
	TEXT("Logs character entity state. Optional argument: a single character index."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const UCharacterEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UCharacterEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsCharacters, Warning, TEXT("No UCharacterEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() > 0)
			{
				Subsystem->LogCharacter(FCharacterId(FCString::Atoi(*Args[0])));
				return;
			}

			UE_LOG(LogRtsCharacters, Log, TEXT("=== Alive=%d Slots=%d ==="),
				Subsystem->GetAliveIds().Num(), Subsystem->NumSlots());
			for (int32 Index = 0; Index < Subsystem->NumSlots(); ++Index)
			{
				Subsystem->LogCharacter(FCharacterId(Index));
			}
		}));

static FAutoConsoleCommandWithWorldAndArgs GKillCharacterCommand(
	TEXT("Rts.KillCharacter"),
	TEXT("Destroys a character's entity by id, leaving its slot marked dead. Argument: the character id."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			UCharacterEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UCharacterEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsCharacters, Warning, TEXT("No UCharacterEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() == 0)
			{
				UE_LOG(LogRtsCharacters, Warning, TEXT("Usage: Rts.KillCharacter <id>"));
				return;
			}

			const FCharacterId Id(FCString::Atoi(*Args[0]));
			if (!Subsystem->IsAlive(Id))
			{
				UE_LOG(LogRtsCharacters, Warning, TEXT("[%d] already not alive"), Id.Value);
				return;
			}

			Subsystem->DestroyCharacter(Id);
			UE_LOG(LogRtsCharacters, Log, TEXT("[%d] destroyed"), Id.Value);
		}));
