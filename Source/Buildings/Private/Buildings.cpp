#include "Buildings.h"

#include "BuildingEntitySubsystem.h"
#include "BuildingType.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsBuildingsFacade, Log, All);

#define LOCTEXT_NAMESPACE "FBuildingsModule"

void FBuildingsModule::StartupModule()
{

}

void FBuildingsModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBuildingsModule, Buildings)

UBuildings::UBuildings()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UBuildingEntitySubsystem* UBuildings::GetEntities() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UBuildingEntitySubsystem>() : nullptr;
}

int32 UBuildings::ResolveTypeIndex(UBuildingType* InBuildingType)
{
	const int32 Existing = BuildingTypes.Find(InBuildingType);
	return Existing != INDEX_NONE ? Existing : BuildingTypes.Add(InBuildingType);
}

int32 UBuildings::Num() const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->NumSlots() : 0;
}

bool UBuildings::IsAlive(const FBuildingId InBuildingId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities && Entities->IsAlive(InBuildingId);
}

UBuildingType* UBuildings::GetBuildingType(const FBuildingId InBuildingId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return nullptr;
	}

	const int32 TypeIndex = Entities->GetTypeIndex(InBuildingId);
	return BuildingTypes.IsValidIndex(TypeIndex) ? BuildingTypes[TypeIndex] : nullptr;
}

FProvinceId UBuildings::GetProvince(const FBuildingId InBuildingId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetProvince(InBuildingId) : FProvinceId();
}

FBuildingOwner UBuildings::GetBuildingOwner(const FBuildingId InBuildingId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetOwner(InBuildingId) : FBuildingOwner();
}

TArray<UBuildingType*> UBuildings::CollectBuildingsInProvince(const FProvinceId InProvinceId) const
{
	TArray<UBuildingType*> Result;

	const UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return Result;
	}

	for (const FBuildingId Id : Entities->CollectBuildingsInProvince(InProvinceId))
	{
		if (UBuildingType* Type = GetBuildingType(Id))
		{
			Result.Add(Type);
		}
	}
	return Result;
}

int32 UBuildings::CountBuildingsOfType(const FProvinceId InProvinceId, UBuildingType* InBuildingType) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities || !InBuildingType)
	{
		return 0;
	}

	const int32 TypeIndex = BuildingTypes.Find(InBuildingType);
	if (TypeIndex == INDEX_NONE)
	{
		return 0;
	}
	return Entities->CountBuildingsOfType(InProvinceId, TypeIndex);
}

int32 UBuildings::CountBuildingsInSlot(const FProvinceId InProvinceId, const FName InSlotName) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return 0;
	}

	int32 Count = 0;
	for (const FBuildingId Id : Entities->CollectBuildingsInProvince(InProvinceId))
	{
		if (const UBuildingType* Type = GetBuildingType(Id))
		{
			if (Type->GetBuildingSlotName() == InSlotName)
			{
				++Count;
			}
		}
	}
	return Count;
}

FBuildingId UBuildings::Add(UBuildingType* InBuildingType, const FProvinceId InProvinceId, const FBuildingOwner& InOwner)
{
	if (!InBuildingType)
	{
		return FBuildingId();
	}

	UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return FBuildingId();
	}

	const int32 TypeIndex = ResolveTypeIndex(InBuildingType);
	const int32 CurrentCount = Entities->CountBuildingsOfType(InProvinceId, TypeIndex);
	if (CurrentCount >= InBuildingType->GetMaxPerProvince())
	{
		UE_LOG(LogRtsBuildingsFacade, Warning,
			TEXT("UBuildings::Add: province %d already has %d/%d of building type '%s', refusing to add another"),
			InProvinceId.Value, CurrentCount, InBuildingType->GetMaxPerProvince(), *InBuildingType->GetBuildingName().ToString());
		return FBuildingId();
	}

	const FBuildingId NewId = Entities->CreateBuilding(InProvinceId, TypeIndex, InOwner);
	OnBuildingAdded.Broadcast(NewId);
	return NewId;
}

void UBuildings::Destroy(const FBuildingId InBuildingId)
{
	UBuildingEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->DestroyBuilding(InBuildingId);
	OnBuildingDestroyed.Broadcast(InBuildingId);
}
