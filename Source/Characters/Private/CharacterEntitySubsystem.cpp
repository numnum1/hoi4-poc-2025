#include "CharacterEntitySubsystem.h"

#include "CharacterFragments.h"
#include "CharacterHourlySimProcessor.h"
#include "Time.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"
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
}

void UCharacterEntitySubsystem::Deinitialize()
{
	if (Time)
	{
		Time->OnHourElapsed.RemoveDynamic(this, &UCharacterEntitySubsystem::HandleHourElapsed);
		Time = nullptr;
	}

	EntityByIndex.Reset();
	SimProcessor = nullptr;
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UCharacterEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

void UCharacterEntitySubsystem::CreateCharacters(const TArray<FCharacterSpawnData>& InSpawnData)
{
	FMassEntityManager& EntityManager = GetEntityManager();

	if (EntityByIndex.Num() > 0)
	{
		EntityManager.BatchDestroyEntities(EntityByIndex);
		EntityByIndex.Reset();
	}

	EntityByIndex.Reserve(InSpawnData.Num());

	for (int32 CharacterIndex = 0; CharacterIndex < InSpawnData.Num(); ++CharacterIndex)
	{
		const FCharacterSpawnData& Data = InSpawnData[CharacterIndex];

		// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
		FCharacterIndexFragment IndexFragment;
		IndexFragment.Index = CharacterIndex;

		FCharacterOriginFragment OriginFragment;
		OriginFragment.BirthProvinceIndex = Data.BirthProvinceIndex;
		OriginFragment.BaseRaceIndex = Data.BaseRaceIndex;

		FCharacterLocationFragment LocationFragment;
		LocationFragment.CurrentProvinceIndex = Data.CurrentProvinceIndex;
		LocationFragment.TimezoneOffset = Data.TimezoneOffset;

		FCharacterAbilitiesFragment AbilitiesFragment;
		AbilitiesFragment.Abilities = Data.RaceModifiers;

		FCharacterScheduleSharedFragment ScheduleFragment;
		ScheduleFragment.Schedule = Data.Schedule;

		FCharacterRaceSharedFragment RaceFragment;
		RaceFragment.RaceIndex = Data.BaseRaceIndex;
		RaceFragment.RaceModifiers = Data.RaceModifiers;

		// FEntityBuilder over BatchCreateEntities: the archetype varies by sim tier and the
		// shared fragment values vary per character, so batching would have to group first.
		// At this scale that trade is not worth the indirection.
		UE::Mass::FEntityBuilder Builder = EntityManager.MakeEntityBuilder();

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
	}

	UE_LOG(LogRtsCharacters, Log, TEXT("Created %d character entities"), EntityByIndex.Num());
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
	if (InHoursElapsed <= 0 || EntityByIndex.Num() == 0 || !Time)
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
}

// -----------------------------------------------------------------------------
// Read
// -----------------------------------------------------------------------------

int32 UCharacterEntitySubsystem::GetCurrentProvince(const int32 InCharacterIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterIndex);
	if (!Entity.IsValid())
	{
		return INDEX_NONE;
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FCharacterLocationFragment>().CurrentProvinceIndex;
}

int32 UCharacterEntitySubsystem::GetTimezoneOffset(const int32 InCharacterIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterIndex);
	if (!Entity.IsValid())
	{
		return 0;
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FCharacterLocationFragment>().TimezoneOffset;
}

FAbilities UCharacterEntitySubsystem::GetAbilities(const int32 InCharacterIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterIndex);
	if (!Entity.IsValid())
	{
		return FAbilities();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FCharacterAbilitiesFragment>().Abilities;
}

TArray<FPosition> UCharacterEntitySubsystem::GetPositions(const int32 InCharacterIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterIndex);
	if (!Entity.IsValid())
	{
		return {};
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FCharacterPositionsFragment>().Values;
}

FSchedule UCharacterEntitySubsystem::GetSchedule(const int32 InCharacterIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterIndex);
	if (!Entity.IsValid())
	{
		return FSchedule();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetConstSharedFragmentData<FCharacterScheduleSharedFragment>().Schedule;
}

// -----------------------------------------------------------------------------
// Write
// -----------------------------------------------------------------------------

void UCharacterEntitySubsystem::MoveCharacter(const int32 InCharacterIndex, const int32 InNewProvinceIndex,
                                              const int32 InNewTimezoneOffset)
{
	const FMassEntityHandle Entity = GetEntity(InCharacterIndex);
	if (!Entity.IsValid())
	{
		return;
	}

	FCharacterLocationFragment& Location =
		FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FCharacterLocationFragment>();
	Location.CurrentProvinceIndex = InNewProvinceIndex;
	Location.TimezoneOffset = InNewTimezoneOffset;
}

void UCharacterEntitySubsystem::AddPosition(const int32 InCharacterIndex, const int32 InAtOrganizationIndex)
{
	const FMassEntityHandle Entity = GetEntity(InCharacterIndex);
	if (!Entity.IsValid())
	{
		return;
	}

	FPosition NewPosition;
	NewPosition.OrganizationIndex = InAtOrganizationIndex;

	FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FCharacterPositionsFragment>()
		.Values.Add(NewPosition);
}

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------

void UCharacterEntitySubsystem::LogCharacter(const int32 InCharacterIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InCharacterIndex);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsCharacters, Warning, TEXT("[%d] no entity"), InCharacterIndex);
		return;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	const FCharacterLocationFragment& Location = View.GetFragmentData<FCharacterLocationFragment>();
	const FCharacterVitalsFragment& Vitals = View.GetFragmentData<FCharacterVitalsFragment>();
	const FCharacterSkillsFragment& Skills = View.GetFragmentData<FCharacterSkillsFragment>();
	const FCharacterPositionsFragment& Positions = View.GetFragmentData<FCharacterPositionsFragment>();

	UE_LOG(LogRtsCharacters, Log,
		TEXT("[%d] Province=%d Tz=%d | Energy=%d Happiness=%d | Sci=%d Str=%d Med=%d | Positions=%d"),
		InCharacterIndex, Location.CurrentProvinceIndex, Location.TimezoneOffset,
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
				Subsystem->LogCharacter(FCString::Atoi(*Args[0]));
				return;
			}

			UE_LOG(LogRtsCharacters, Log, TEXT("=== %d characters ==="), Subsystem->Num());
			for (int32 Index = 0; Index < Subsystem->Num(); ++Index)
			{
				Subsystem->LogCharacter(Index);
			}
		}));
