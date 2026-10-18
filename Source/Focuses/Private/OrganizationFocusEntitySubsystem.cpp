#include "OrganizationFocusEntitySubsystem.h"

#include "Engine/World.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"
#include "OrganizationFocusFragments.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsFocuses, Log, All);

bool UOrganizationFocusEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UOrganizationFocusEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();
}

void UOrganizationFocusEntitySubsystem::Deinitialize()
{
	EntityByOrganization.Reset();
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UOrganizationFocusEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

FMassEntityHandle UOrganizationFocusEntitySubsystem::FindEntity(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle* Found = EntityByOrganization.Find(InOrganizationId);
	return Found ? *Found : FMassEntityHandle();
}

FMassEntityHandle UOrganizationFocusEntitySubsystem::FindOrCreateEntity(const FOrganizationId InOrganizationId)
{
	const FMassEntityHandle Existing = FindEntity(InOrganizationId);
	if (Existing.IsValid())
	{
		return Existing;
	}

	FOrganizationFocusOwnerFragment OwnerFragment;
	OwnerFragment.OrganizationId = InOrganizationId;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FOrganizationFocusTag>()
	       .Add<FOrganizationFocusOwnerFragment>(OwnerFragment)
	       .Add<FOrganizationFocusRunFragment>()
	       .Add<FOrganizationFocusHistoryFragment>();

	const FMassEntityHandle NewEntity = Builder.Commit();
	EntityByOrganization.Add(InOrganizationId, NewEntity);
	return NewEntity;
}

FTopLevelAssetPath UOrganizationFocusEntitySubsystem::GetCurrentFocus(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle Entity = FindEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		return FTopLevelAssetPath();
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FOrganizationFocusRunFragment>().CurrentFocus;
}

FDateTime UOrganizationFocusEntitySubsystem::GetStartedAt(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle Entity = FindEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		return FDateTime();
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FOrganizationFocusRunFragment>().StartedAt;
}

FTopLevelAssetPath UOrganizationFocusEntitySubsystem::GetPendingEventFocus(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle Entity = FindEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		return FTopLevelAssetPath();
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FOrganizationFocusRunFragment>().PendingEventFocus;
}

bool UOrganizationFocusEntitySubsystem::IsCompleted(const FOrganizationId InOrganizationId, const FTopLevelAssetPath& InFocus) const
{
	const FMassEntityHandle Entity = FindEntity(InOrganizationId);
	if (!Entity.IsValid() || InFocus.IsNull())
	{
		return false;
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FOrganizationFocusHistoryFragment>().CompletedFocuses.Contains(InFocus);
}

TArray<FOrganizationId> UOrganizationFocusEntitySubsystem::CollectOrganizationsWithRunningFocus() const
{
	TArray<FOrganizationId> Result;
	for (const TPair<FOrganizationId, FMassEntityHandle>& Pair : EntityByOrganization)
	{
		if (!FMassEntityView(GetEntityManager(), Pair.Value).GetFragmentData<FOrganizationFocusRunFragment>().CurrentFocus.IsNull())
		{
			Result.Add(Pair.Key);
		}
	}
	return Result;
}

TArray<FOrganizationId> UOrganizationFocusEntitySubsystem::CollectOrganizations() const
{
	TArray<FOrganizationId> Result;
	EntityByOrganization.GetKeys(Result);
	return Result;
}

void UOrganizationFocusEntitySubsystem::SetCurrentFocus(const FOrganizationId InOrganizationId, const FTopLevelAssetPath& InFocus, const FDateTime& InStartedAt)
{
	const FMassEntityHandle Entity = FindOrCreateEntity(InOrganizationId);

	FOrganizationFocusRunFragment& Run = FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FOrganizationFocusRunFragment>();
	Run.CurrentFocus = InFocus;
	Run.StartedAt = InStartedAt;
}

FTopLevelAssetPath UOrganizationFocusEntitySubsystem::CompleteCurrentFocus(const FOrganizationId InOrganizationId)
{
	const FMassEntityHandle Entity = FindEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		return FTopLevelAssetPath();
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	FOrganizationFocusRunFragment& Run = View.GetFragmentData<FOrganizationFocusRunFragment>();
	const FTopLevelAssetPath Completed = Run.CurrentFocus;
	if (Completed.IsNull())
	{
		return Completed;
	}

	View.GetFragmentData<FOrganizationFocusHistoryFragment>().CompletedFocuses.AddUnique(Completed);
	Run.CurrentFocus = FTopLevelAssetPath();
	Run.StartedAt = FDateTime();
	return Completed;
}

void UOrganizationFocusEntitySubsystem::AddCompletedFocus(const FOrganizationId InOrganizationId, const FTopLevelAssetPath& InFocus)
{
	if (InFocus.IsNull())
	{
		return;
	}

	const FMassEntityHandle Entity = FindOrCreateEntity(InOrganizationId);
	FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FOrganizationFocusHistoryFragment>().CompletedFocuses.AddUnique(InFocus);
}

void UOrganizationFocusEntitySubsystem::SetPendingEventFocus(const FOrganizationId InOrganizationId, const FTopLevelAssetPath& InFocus)
{
	const FMassEntityHandle Entity = InFocus.IsNull() ? FindEntity(InOrganizationId) : FindOrCreateEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		return;
	}

	FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FOrganizationFocusRunFragment>().PendingEventFocus = InFocus;
}

void UOrganizationFocusEntitySubsystem::LogOrganization(const FOrganizationId InOrganizationId) const
{
	const FMassEntityHandle Entity = FindEntity(InOrganizationId);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsFocuses, Warning, TEXT("[Org %d] no focus entity"), InOrganizationId.Value);
		return;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	const FOrganizationFocusRunFragment& Run = View.GetFragmentData<FOrganizationFocusRunFragment>();
	const FOrganizationFocusHistoryFragment& History = View.GetFragmentData<FOrganizationFocusHistoryFragment>();

	TArray<FString> CompletedNames;
	for (const FTopLevelAssetPath& Path : History.CompletedFocuses)
	{
		CompletedNames.Add(Path.GetAssetName().ToString());
	}

	UE_LOG(LogRtsFocuses, Log, TEXT("[Org %d] Current=%s StartedAt=%s Pending=%s Completed=[%s]"),
		InOrganizationId.Value,
		*Run.CurrentFocus.GetAssetName().ToString(),
		*Run.StartedAt.ToString(),
		*Run.PendingEventFocus.GetAssetName().ToString(),
		*FString::Join(CompletedNames, TEXT(", ")));
}

static FAutoConsoleCommandWithWorldAndArgs GDumpOrganizationFocusesCommand(
	TEXT("Rts.DumpOrganizationFocuses"),
	TEXT("Logs organization focus state. Optional argument: a single organization index."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const UOrganizationFocusEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UOrganizationFocusEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsFocuses, Warning, TEXT("No UOrganizationFocusEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() > 0)
			{
				Subsystem->LogOrganization(FOrganizationId(FCString::Atoi(*Args[0])));
				return;
			}

			const TArray<FOrganizationId> Organizations = Subsystem->CollectOrganizations();
			UE_LOG(LogRtsFocuses, Log, TEXT("=== Organizations with focus state=%d ==="), Organizations.Num());
			for (const FOrganizationId OrganizationId : Organizations)
			{
				Subsystem->LogOrganization(OrganizationId);
			}
		}));
