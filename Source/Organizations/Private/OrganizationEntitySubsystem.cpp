#include "OrganizationEntitySubsystem.h"

#include "OrganizationFragments.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"

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
}

void UOrganizationEntitySubsystem::Deinitialize()
{
	EntityByIndex.Reset();
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UOrganizationEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

int32 UOrganizationEntitySubsystem::CreateOrganization()
{
	const int32 NewIndex = EntityByIndex.Num();

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FOrganizationIndexFragment IndexFragment;
	IndexFragment.Index = NewIndex;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FOrganizationTag>()
	       .Add<FOrganizationIndexFragment>(IndexFragment)
	       .Add<FOrganizationAlignmentFragment>()
	       .Add<FOrganizationSettledNationsFragment>();

	EntityByIndex.Add(Builder.Commit());
	return NewIndex;
}

void UOrganizationEntitySubsystem::Reset()
{
	if (EntityByIndex.Num() > 0)
	{
		GetEntityManager().BatchDestroyEntities(EntityByIndex);
		EntityByIndex.Reset();
	}
}

// -----------------------------------------------------------------------------
// Read
// -----------------------------------------------------------------------------

bool UOrganizationEntitySubsystem::IsAlignmentPublished(const int32 InOrganizationIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationIndex);
	if (!Entity.IsValid())
	{
		return false;
	}
	return FMassEntityView(GetEntityManager(), Entity).HasTag<FOrganizationPublishedTag>();
}

FOrganizationAlignment UOrganizationEntitySubsystem::GetAlignment(const int32 InOrganizationIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationIndex);
	if (!Entity.IsValid())
	{
		return FOrganizationAlignment();
	}

	FOrganizationAlignment Result;
	Result.bPublished = IsAlignmentPublished(InOrganizationIndex);
	Result.Branches = FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationAlignmentFragment>().Branches;
	return Result;
}

TArray<int32> UOrganizationEntitySubsystem::GetSettledNations(const int32 InOrganizationIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationIndex);
	if (!Entity.IsValid())
	{
		return {};
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationSettledNationsFragment>().Values;
}

// -----------------------------------------------------------------------------
// Write
// -----------------------------------------------------------------------------

void UOrganizationEntitySubsystem::PublishAlignment(const int32 InOrganizationIndex, const TArray<float>& InAlignment)
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationIndex);
	if (!Entity.IsValid())
	{
		return;
	}

	FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationAlignmentFragment>().Branches = InAlignment;

	// Moves the entity to the published archetype. Idempotent, so no need to check first.
	GetEntityManager().AddTagToEntity(Entity, FOrganizationPublishedTag::StaticStruct());
}

bool UOrganizationEntitySubsystem::SettleNation(const int32 InOrganizationIndex, const int32 InNationIndex)
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationIndex);
	if (!Entity.IsValid())
	{
		return false;
	}

	TArray<int32>& Settled = FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FOrganizationSettledNationsFragment>().Values;

	if (Settled.Contains(InNationIndex))
	{
		return false;
	}

	Settled.Add(InNationIndex);
	return true;
}

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------

void UOrganizationEntitySubsystem::LogOrganization(const int32 InOrganizationIndex) const
{
	const FMassEntityHandle Entity = GetEntity(InOrganizationIndex);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsOrganizations, Warning, TEXT("[%d] no entity"), InOrganizationIndex);
		return;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	const FOrganizationAlignmentFragment& Alignment = View.GetFragmentData<FOrganizationAlignmentFragment>();
	const FOrganizationSettledNationsFragment& Settled = View.GetFragmentData<FOrganizationSettledNationsFragment>();

	UE_LOG(LogRtsOrganizations, Log, TEXT("[%d] Published=%d Branches=%d SettledNations=%d"),
		InOrganizationIndex, IsAlignmentPublished(InOrganizationIndex) ? 1 : 0,
		Alignment.Branches.Num(), Settled.Values.Num());
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
				Subsystem->LogOrganization(FCString::Atoi(*Args[0]));
				return;
			}

			UE_LOG(LogRtsOrganizations, Log, TEXT("=== %d organizations ==="), Subsystem->Num());
			for (int32 Index = 0; Index < Subsystem->Num(); ++Index)
			{
				Subsystem->LogOrganization(Index);
			}
		}));
