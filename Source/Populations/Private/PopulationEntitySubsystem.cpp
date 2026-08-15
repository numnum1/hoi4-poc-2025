#include "PopulationEntitySubsystem.h"

#include "PopGrowthProcessor.h"
#include "PopulationFragments.h"
#include "Time.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsPopulations, Log, All);

bool UPopulationEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UPopulationEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();

	PopQuery = FMassEntityQuery(GetEntityManager().AsShared());
	PopQuery.AddTagRequirement<FPopTag>(EMassFragmentPresence::All);
	PopQuery.AddRequirement<FPopIndexFragment>(EMassFragmentAccess::ReadOnly);
	PopQuery.AddRequirement<FPopProvinceFragment>(EMassFragmentAccess::ReadOnly);
	PopQuery.AddRequirement<FPopEthnicityFragment>(EMassFragmentAccess::ReadOnly);
	PopQuery.AddRequirement<FPopSizeFragment>(EMassFragmentAccess::ReadOnly);
}

void UPopulationEntitySubsystem::Deinitialize()
{
	if (Time)
	{
		Time->OnHourElapsed.RemoveDynamic(this, &UPopulationEntitySubsystem::HandleHourElapsed);
		Time = nullptr;
	}

	EntityByIndex.Reset();
	AliveIds.Reset();
	SimProcessor = nullptr;
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UPopulationEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

FPopId UPopulationEntitySubsystem::CreatePop(const FPopSpawnData& InSpawnData, const float InAnnualBirthRate)
{
	const FPopId NewId(EntityByIndex.Num());

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FPopIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	FPopProvinceFragment ProvinceFragment;
	ProvinceFragment.ProvinceId = InSpawnData.ProvinceId;

	FPopEthnicityFragment EthnicityFragment;
	EthnicityFragment.EthnicityId = InSpawnData.EthnicityId;

	FPopSizeFragment SizeFragment;
	SizeFragment.Size = InSpawnData.Size;

	FPopGrowthSharedFragment GrowthFragment;
	GrowthFragment.AnnualBirthRate = InAnnualBirthRate;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FPopTag>()
	       .Add<FPopIndexFragment>(IndexFragment)
	       .Add<FPopProvinceFragment>(ProvinceFragment)
	       .Add<FPopEthnicityFragment>(EthnicityFragment)
	       .Add<FPopSizeFragment>(SizeFragment)
	       .Add<FPopGrowthSharedFragment>(GrowthFragment);

	EntityByIndex.Add(Builder.Commit());
	AliveIds.Add(NewId);
	return NewId;
}

void UPopulationEntitySubsystem::DestroyPop(const FPopId InPopId)
{
	const FMassEntityHandle Entity = GetEntity(InPopId);
	if (!Entity.IsValid())
	{
		return;
	}

	GetEntityManager().DestroyEntity(Entity);
	EntityByIndex[InPopId.Value] = FMassEntityHandle();
	AliveIds.RemoveSingle(InPopId);
}

void UPopulationEntitySubsystem::BindToTime(UTime* InTime)
{
	if (Time == InTime)
	{
		return;
	}

	if (Time)
	{
		Time->OnHourElapsed.RemoveDynamic(this, &UPopulationEntitySubsystem::HandleHourElapsed);
	}

	Time = InTime;

	if (Time)
	{
		Time->OnHourElapsed.AddDynamic(this, &UPopulationEntitySubsystem::HandleHourElapsed);
		// Baseline: the first crossed month applies growth for the time since binding, not since
		// the epoch.
		LastGrowthTime = Time->GetCurrentTime();
	}
}

void UPopulationEntitySubsystem::HandleHourElapsed(const int32 InHoursElapsed)
{
	if (InHoursElapsed <= 0 || AliveIds.Num() == 0 || !Time)
	{
		return;
	}

	const FDateTime CurrentTime = Time->GetCurrentTime();

	// Growth runs once per crossed calendar month, not every hour — at Speed 1000 that is the
	// difference between one Mass query per month and one per hour. A jump of several months in one
	// call (high Speed) is fine: the growth formula is continuous in elapsed hours regardless of how
	// many month boundaries that spans.
	const bool bCrossedMonth = CurrentTime.GetYear() != LastGrowthTime.GetYear()
		|| CurrentTime.GetMonth() != LastGrowthTime.GetMonth();
	if (!bCrossedMonth)
	{
		return;
	}

	const float ElapsedHours = static_cast<float>((CurrentTime - LastGrowthTime).GetTotalHours());
	LastGrowthTime = CurrentTime;

	if (!SimProcessor)
	{
		SimProcessor = NewObject<UPopGrowthProcessor>(this);
		SimProcessor->CallInitialize(this, GetEntityManager().AsShared());
	}

	SimProcessor->SetElapsedHours(ElapsedHours);

	UE::Mass::FProcessingContext ProcessingContext(GetEntityManager(), /*DeltaSeconds=*/0.f, /*bFlushCommandBuffer=*/true);
	UE::Mass::Executor::Run(*SimProcessor, ProcessingContext);

	OnPopulationsGrown.Broadcast();
}

namespace
{
	// A getter seeing an id that just died is normal UI behavior, not a bug — log quietly and
	// return the default, unlike SetSize below, which ensureMsgf's instead.
	void LogInvalidRead(const FPopId InPopId, const TCHAR* InAccessor)
	{
		UE_LOG(LogRtsPopulations, Verbose, TEXT("%s: pop %d is not alive"), InAccessor, InPopId.Value);
	}
}

FPopInfo UPopulationEntitySubsystem::GetPopInfo(const FPopId InPopId) const
{
	const FMassEntityHandle Entity = GetEntity(InPopId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InPopId, TEXT("GetPopInfo"));
		return FPopInfo();
	}

	const FMassEntityView View(GetEntityManager(), Entity);

	FPopInfo Info;
	Info.Id = InPopId;
	Info.ProvinceId = View.GetFragmentData<FPopProvinceFragment>().ProvinceId;
	Info.EthnicityId = View.GetFragmentData<FPopEthnicityFragment>().EthnicityId;
	Info.Size = View.GetFragmentData<FPopSizeFragment>().Size;
	return Info;
}

int32 UPopulationEntitySubsystem::GetSize(const FPopId InPopId) const
{
	const FMassEntityHandle Entity = GetEntity(InPopId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InPopId, TEXT("GetSize"));
		return 0;
	}
	return FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FPopSizeFragment>().Size;
}

void UPopulationEntitySubsystem::ForEachPop(TFunctionRef<void(const FPopInfo&)> InVisitor) const
{
	FMassExecutionContext Context(GetEntityManager());
	PopQuery.ForEachEntityChunk(Context, [&InVisitor](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FPopIndexFragment> Ids = Ctx.GetFragmentView<FPopIndexFragment>();
		const TConstArrayView<FPopProvinceFragment> Provinces = Ctx.GetFragmentView<FPopProvinceFragment>();
		const TConstArrayView<FPopEthnicityFragment> Ethnicities = Ctx.GetFragmentView<FPopEthnicityFragment>();
		const TConstArrayView<FPopSizeFragment> Sizes = Ctx.GetFragmentView<FPopSizeFragment>();

		for (int32 EntityIt = 0; EntityIt < Ctx.GetNumEntities(); ++EntityIt)
		{
			FPopInfo Info;
			Info.Id = Ids[EntityIt].Id;
			Info.ProvinceId = Provinces[EntityIt].ProvinceId;
			Info.EthnicityId = Ethnicities[EntityIt].EthnicityId;
			Info.Size = Sizes[EntityIt].Size;
			InVisitor(Info);
		}
	});
}

TArray<FPopInfo> UPopulationEntitySubsystem::CollectPops() const
{
	TArray<FPopInfo> Result;
	Result.Reserve(AliveIds.Num());
	ForEachPop([&Result](const FPopInfo& InInfo) { Result.Add(InInfo); });
	return Result;
}

TArray<FPopInfo> UPopulationEntitySubsystem::CollectPopsInProvince(const FProvinceId InProvinceId) const
{
	TArray<FPopInfo> Result;
	ForEachPop([&Result, InProvinceId](const FPopInfo& InInfo)
	{
		if (InInfo.ProvinceId == InProvinceId)
		{
			Result.Add(InInfo);
		}
	});
	return Result;
}

int32 UPopulationEntitySubsystem::GetProvincePopulation(const FProvinceId InProvinceId) const
{
	int32 Total = 0;
	ForEachPop([&Total, InProvinceId](const FPopInfo& InInfo)
	{
		if (InInfo.ProvinceId == InProvinceId)
		{
			Total += InInfo.Size;
		}
	});
	return Total;
}

void UPopulationEntitySubsystem::SetSize(const FPopId InPopId, const int32 InSize)
{
	const FMassEntityHandle Entity = GetEntity(InPopId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("SetSize: pop %d is not alive"), InPopId.Value))
	{
		return;
	}

	FMassEntityView(GetEntityManager(), Entity).GetFragmentData<FPopSizeFragment>().Size = FMath::Max(0, InSize);
}

void UPopulationEntitySubsystem::LogPop(const FPopId InPopId) const
{
	const FMassEntityHandle Entity = GetEntity(InPopId);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsPopulations, Warning, TEXT("[%d] no entity"), InPopId.Value);
		return;
	}

	const FPopInfo Info = GetPopInfo(InPopId);
	UE_LOG(LogRtsPopulations, Log, TEXT("[%d] Province=%d Ethnicity=%d Size=%d"),
		InPopId.Value, Info.ProvinceId.Value, Info.EthnicityId.Value, Info.Size);
}

static FAutoConsoleCommandWithWorldAndArgs GDumpPopsCommand(
	TEXT("Rts.DumpPops"),
	TEXT("Logs pop entity state. Optional argument: a single pop index."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const UPopulationEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UPopulationEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsPopulations, Warning, TEXT("No UPopulationEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() > 0)
			{
				Subsystem->LogPop(FPopId(FCString::Atoi(*Args[0])));
				return;
			}

			UE_LOG(LogRtsPopulations, Log, TEXT("=== Alive=%d Slots=%d ==="),
				Subsystem->GetAliveIds().Num(), Subsystem->NumSlots());
			for (int32 Index = 0; Index < Subsystem->NumSlots(); ++Index)
			{
				Subsystem->LogPop(FPopId(Index));
			}
		}));
