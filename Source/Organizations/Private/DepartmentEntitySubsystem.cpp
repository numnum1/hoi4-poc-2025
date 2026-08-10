#include "DepartmentEntitySubsystem.h"

#include "DepartmentFragments.h"
#include "MassEntityBuilder.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsDepartments, Log, All);

bool UDepartmentEntitySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UDepartmentEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();

	OrganizationQuery = FMassEntityQuery(GetEntityManager().AsShared());
	OrganizationQuery.AddTagRequirement<FDepartmentTag>(EMassFragmentPresence::All);
	OrganizationQuery.AddRequirement<FDepartmentIndexFragment>(EMassFragmentAccess::ReadOnly);
	OrganizationQuery.AddRequirement<FDepartmentOrganizationFragment>(EMassFragmentAccess::ReadOnly);

	ParentQuery = FMassEntityQuery(GetEntityManager().AsShared());
	ParentQuery.AddTagRequirement<FDepartmentTag>(EMassFragmentPresence::All);
	ParentQuery.AddRequirement<FDepartmentIndexFragment>(EMassFragmentAccess::ReadOnly);
	ParentQuery.AddRequirement<FDepartmentParentFragment>(EMassFragmentAccess::ReadOnly);

	// Carries the function too, so GetUsedSpace can filter and sum in one pass.
	SeatQuery = FMassEntityQuery(GetEntityManager().AsShared());
	SeatQuery.AddTagRequirement<FDepartmentTag>(EMassFragmentPresence::All);
	SeatQuery.AddRequirement<FDepartmentIndexFragment>(EMassFragmentAccess::ReadOnly);
	SeatQuery.AddRequirement<FDepartmentSeatFragment>(EMassFragmentAccess::ReadOnly);
	SeatQuery.AddRequirement<FDepartmentFunctionFragment>(EMassFragmentAccess::ReadOnly);
}

void UDepartmentEntitySubsystem::Deinitialize()
{
	EntityByIndex.Reset();
	AliveIds.Reset();
	EntitySubsystem = nullptr;

	Super::Deinitialize();
}

FMassEntityManager& UDepartmentEntitySubsystem::GetEntityManager() const
{
	check(EntitySubsystem);
	return EntitySubsystem->GetMutableEntityManager();
}

FDepartmentId UDepartmentEntitySubsystem::CreateDepartment(const FOrganizationId InOrganizationId,
                                                           const FDepartmentId InParentId,
                                                           const FBuildingId InSeatId,
                                                           const EBuildingFunction InFunction,
                                                           const int32 InSpace)
{
	const FDepartmentId NewId(EntityByIndex.Num());

	// Fragments derive from FMassFragment, so they are not aggregates — fill them by field.
	FDepartmentIndexFragment IndexFragment;
	IndexFragment.Id = NewId;

	FDepartmentOrganizationFragment OrganizationFragment;
	OrganizationFragment.OrganizationId = InOrganizationId;

	FDepartmentParentFragment ParentFragment;
	ParentFragment.ParentId = InParentId;

	FDepartmentSeatFragment SeatFragment;
	SeatFragment.BuildingId = InSeatId;
	SeatFragment.Space = InSpace;

	FDepartmentFunctionFragment FunctionFragment;
	FunctionFragment.Function = InFunction;

	UE::Mass::FEntityBuilder Builder = GetEntityManager().MakeEntityBuilder();
	Builder.Add<FDepartmentTag>()
	       .Add<FDepartmentIndexFragment>(IndexFragment)
	       .Add<FDepartmentOrganizationFragment>(OrganizationFragment)
	       .Add<FDepartmentParentFragment>(ParentFragment)
	       .Add<FDepartmentSeatFragment>(SeatFragment)
	       .Add<FDepartmentFunctionFragment>(FunctionFragment);

	EntityByIndex.Add(Builder.Commit());
	AliveIds.Add(NewId);
	return NewId;
}

void UDepartmentEntitySubsystem::DestroyDepartment(const FDepartmentId InDepartmentId)
{
	const FMassEntityHandle Entity = GetEntity(InDepartmentId);
	if (!Entity.IsValid())
	{
		return;
	}

	GetEntityManager().DestroyEntity(Entity);
	EntityByIndex[InDepartmentId.Value] = FMassEntityHandle();
	AliveIds.RemoveSingle(InDepartmentId);
}

void UDepartmentEntitySubsystem::Reset()
{
	if (EntityByIndex.Num() > 0)
	{
		GetEntityManager().BatchDestroyEntities(EntityByIndex);
		EntityByIndex.Reset();
		AliveIds.Reset();
	}
}

namespace
{
	// A getter seeing an id that just died is normal UI behavior, not a bug — log quietly and
	// return the default, unlike the write functions below, which ensureMsgf's instead.
	void LogInvalidRead(const FDepartmentId InDepartmentId, const TCHAR* InAccessor)
	{
		UE_LOG(LogRtsDepartments, Verbose, TEXT("%s: department %d is not alive"), InAccessor, InDepartmentId.Value);
	}
}

FOrganizationId UDepartmentEntitySubsystem::GetOrganization(const FDepartmentId InDepartmentId) const
{
	const FMassEntityHandle Entity = GetEntity(InDepartmentId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InDepartmentId, TEXT("GetOrganization"));
		return FOrganizationId();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FDepartmentOrganizationFragment>().OrganizationId;
}

FDepartmentId UDepartmentEntitySubsystem::GetParent(const FDepartmentId InDepartmentId) const
{
	const FMassEntityHandle Entity = GetEntity(InDepartmentId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InDepartmentId, TEXT("GetParent"));
		return FDepartmentId();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FDepartmentParentFragment>().ParentId;
}

FBuildingId UDepartmentEntitySubsystem::GetSeat(const FDepartmentId InDepartmentId) const
{
	const FMassEntityHandle Entity = GetEntity(InDepartmentId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InDepartmentId, TEXT("GetSeat"));
		return FBuildingId();
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FDepartmentSeatFragment>().BuildingId;
}

int32 UDepartmentEntitySubsystem::GetSpace(const FDepartmentId InDepartmentId) const
{
	const FMassEntityHandle Entity = GetEntity(InDepartmentId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InDepartmentId, TEXT("GetSpace"));
		return 0;
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FDepartmentSeatFragment>().Space;
}

EBuildingFunction UDepartmentEntitySubsystem::GetFunction(const FDepartmentId InDepartmentId) const
{
	const FMassEntityHandle Entity = GetEntity(InDepartmentId);
	if (!Entity.IsValid())
	{
		LogInvalidRead(InDepartmentId, TEXT("GetFunction"));
		return EBuildingFunction::BF_Bureaucracy;
	}
	return FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FDepartmentFunctionFragment>().Function;
}

TArray<FDepartmentId> UDepartmentEntitySubsystem::CollectDepartmentsOf(const FOrganizationId InOrganizationId) const
{
	TArray<FDepartmentId> Result;

	FMassExecutionContext Context(GetEntityManager());
	OrganizationQuery.ForEachEntityChunk(Context, [&Result, InOrganizationId](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FDepartmentIndexFragment> Ids = Ctx.GetFragmentView<FDepartmentIndexFragment>();
		const TConstArrayView<FDepartmentOrganizationFragment> Organizations =
			Ctx.GetFragmentView<FDepartmentOrganizationFragment>();

		for (int32 EntityIt = 0; EntityIt < Ctx.GetNumEntities(); ++EntityIt)
		{
			if (Organizations[EntityIt].OrganizationId == InOrganizationId)
			{
				Result.Add(Ids[EntityIt].Id);
			}
		}
	});

	return Result;
}

TArray<FDepartmentId> UDepartmentEntitySubsystem::CollectDepartmentsIn(const FBuildingId InBuildingId) const
{
	TArray<FDepartmentId> Result;

	FMassExecutionContext Context(GetEntityManager());
	SeatQuery.ForEachEntityChunk(Context, [&Result, InBuildingId](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FDepartmentIndexFragment> Ids = Ctx.GetFragmentView<FDepartmentIndexFragment>();
		const TConstArrayView<FDepartmentSeatFragment> Seats = Ctx.GetFragmentView<FDepartmentSeatFragment>();

		for (int32 EntityIt = 0; EntityIt < Ctx.GetNumEntities(); ++EntityIt)
		{
			if (Seats[EntityIt].BuildingId == InBuildingId)
			{
				Result.Add(Ids[EntityIt].Id);
			}
		}
	});

	return Result;
}

TArray<FDepartmentId> UDepartmentEntitySubsystem::CollectChildren(const FDepartmentId InDepartmentId) const
{
	TArray<FDepartmentId> Result;

	FMassExecutionContext Context(GetEntityManager());
	ParentQuery.ForEachEntityChunk(Context, [&Result, InDepartmentId](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FDepartmentIndexFragment> Ids = Ctx.GetFragmentView<FDepartmentIndexFragment>();
		const TConstArrayView<FDepartmentParentFragment> Parents = Ctx.GetFragmentView<FDepartmentParentFragment>();

		for (int32 EntityIt = 0; EntityIt < Ctx.GetNumEntities(); ++EntityIt)
		{
			if (Parents[EntityIt].ParentId == InDepartmentId)
			{
				Result.Add(Ids[EntityIt].Id);
			}
		}
	});

	return Result;
}

int32 UDepartmentEntitySubsystem::GetUsedSpace(const FBuildingId InBuildingId, const EBuildingFunction InFunction) const
{
	int32 Used = 0;

	FMassExecutionContext Context(GetEntityManager());
	SeatQuery.ForEachEntityChunk(Context, [&Used, InBuildingId, InFunction](FMassExecutionContext& Ctx)
	{
		const TConstArrayView<FDepartmentSeatFragment> Seats = Ctx.GetFragmentView<FDepartmentSeatFragment>();
		const TConstArrayView<FDepartmentFunctionFragment> Functions = Ctx.GetFragmentView<FDepartmentFunctionFragment>();

		for (int32 EntityIt = 0; EntityIt < Ctx.GetNumEntities(); ++EntityIt)
		{
			if (Seats[EntityIt].BuildingId == InBuildingId && Functions[EntityIt].Function == InFunction)
			{
				Used += Seats[EntityIt].Space;
			}
		}
	});

	return Used;
}

void UDepartmentEntitySubsystem::SetParent(const FDepartmentId InDepartmentId, const FDepartmentId InNewParentId)
{
	const FMassEntityHandle Entity = GetEntity(InDepartmentId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("SetParent: department %d is not alive"), InDepartmentId.Value))
	{
		return;
	}

	// Walking up from the prospective parent must not lead back here, or the branch would be cut
	// loose from its organization and every upward walk would spin forever.
	for (FDepartmentId Ancestor = InNewParentId; Ancestor.IsValid(); Ancestor = GetParent(Ancestor))
	{
		if (!ensureMsgf(Ancestor != InDepartmentId,
			TEXT("SetParent: department %d cannot be parented to its own descendant %d"),
			InDepartmentId.Value, InNewParentId.Value))
		{
			return;
		}
	}

	FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FDepartmentParentFragment>().ParentId = InNewParentId;
}

void UDepartmentEntitySubsystem::SetSeat(const FDepartmentId InDepartmentId, const FBuildingId InBuildingId, const int32 InSpace)
{
	const FMassEntityHandle Entity = GetEntity(InDepartmentId);
	if (!ensureMsgf(Entity.IsValid(), TEXT("SetSeat: department %d is not alive"), InDepartmentId.Value))
	{
		return;
	}

	FDepartmentSeatFragment& Seat = FMassEntityView(GetEntityManager(), Entity)
		.GetFragmentData<FDepartmentSeatFragment>();
	Seat.BuildingId = InBuildingId;
	Seat.Space = InSpace;
}

void UDepartmentEntitySubsystem::LogDepartment(const FDepartmentId InDepartmentId) const
{
	const FMassEntityHandle Entity = GetEntity(InDepartmentId);
	if (!Entity.IsValid())
	{
		UE_LOG(LogRtsDepartments, Warning, TEXT("[%d] no entity"), InDepartmentId.Value);
		return;
	}

	const FMassEntityView View(GetEntityManager(), Entity);
	const FDepartmentOrganizationFragment& Organization = View.GetFragmentData<FDepartmentOrganizationFragment>();
	const FDepartmentParentFragment& Parent = View.GetFragmentData<FDepartmentParentFragment>();
	const FDepartmentSeatFragment& Seat = View.GetFragmentData<FDepartmentSeatFragment>();
	const FDepartmentFunctionFragment& Function = View.GetFragmentData<FDepartmentFunctionFragment>();

	UE_LOG(LogRtsDepartments, Log, TEXT("[%d] Organization=%d Parent=%d Seat=%d Space=%d Function=%s"),
		InDepartmentId.Value, Organization.OrganizationId.Value, Parent.ParentId.Value,
		Seat.BuildingId.Value, Seat.Space, *UEnum::GetValueAsString(Function.Function));
}

static FAutoConsoleCommandWithWorldAndArgs GDumpDepartmentsCommand(
	TEXT("Rts.DumpDepartments"),
	TEXT("Logs department entity state. Optional argument: a single department index."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const UDepartmentEntitySubsystem* Subsystem =
				World ? World->GetSubsystem<UDepartmentEntitySubsystem>() : nullptr;
			if (!Subsystem)
			{
				UE_LOG(LogRtsDepartments, Warning, TEXT("No UDepartmentEntitySubsystem in this world"));
				return;
			}

			if (Args.Num() > 0)
			{
				Subsystem->LogDepartment(FDepartmentId(FCString::Atoi(*Args[0])));
				return;
			}

			UE_LOG(LogRtsDepartments, Log, TEXT("=== Alive=%d Slots=%d ==="),
				Subsystem->GetAliveIds().Num(), Subsystem->NumSlots());
			for (int32 Index = 0; Index < Subsystem->NumSlots(); ++Index)
			{
				Subsystem->LogDepartment(FDepartmentId(Index));
			}
		}));
