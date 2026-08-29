#include "Buildings.h"

#include "BuildingEntitySubsystem.h"
#include "BuildingType.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

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

int32 UBuildings::Num() const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->NumSlots() : 0;
}

TArray<FBuildingId> UBuildings::GetAliveBuildings() const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetAliveIds() : TArray<FBuildingId>();
}

bool UBuildings::IsAlive(const FBuildingId InBuildingId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities && Entities->IsAlive(InBuildingId);
}

int32 UBuildings::ResolveTypeIndex(UBuildingType* InBuildingType)
{
	const int32 Existing = BuildingTypes.Find(InBuildingType);
	return Existing != INDEX_NONE ? Existing : BuildingTypes.Add(InBuildingType);
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

FCharacterId UBuildings::GetOwnerCharacter(const FBuildingId InBuildingId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetOwnerCharacter(InBuildingId) : FCharacterId();
}

TArray<FBuildingId> UBuildings::CollectBuildingsInProvince(const FProvinceId InProvinceId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectBuildingsInProvince(InProvinceId) : TArray<FBuildingId>();
}

TArray<FBuildingId> UBuildings::CollectBuildingsOwnedByCharacter(const FCharacterId InOwnerId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectBuildingsOwnedByCharacter(InOwnerId) : TArray<FBuildingId>();
}

FBuildingId UBuildings::Add(UBuildingType* InBuildingType, const FProvinceId InProvinceId, const FCharacterId InOwnerId)
{
	UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return FBuildingId();
	}

	const int32 TypeIndex = ResolveTypeIndex(InBuildingType);
	const FBuildingId NewId = Entities->CreateBuilding(InProvinceId, TypeIndex, InOwnerId);
	OnBuildingAdded.Broadcast(NewId);
	return NewId;
}

void UBuildings::SetCharacterOwner(const FBuildingId InBuildingId, const FCharacterId InNewOwnerId)
{
	UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return;
	}

	Entities->SetCharacterOwner(InBuildingId, InNewOwnerId);
	OnBuildingOwnerChanged.Broadcast(InBuildingId);
}

void UBuildings::Destroy(const FBuildingId InBuildingId)
{
	UBuildingEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	// A headquarters is indestructible for now — an organization would otherwise lose the seat of
	// its root department with no way to get another. Enforced here rather than by leaving the
	// button out of some widget, so it holds for console commands and future callers too.
	const UBuildingType* Type = GetBuildingType(InBuildingId);
	if (Entities->IsAlive(InBuildingId) && Type && Type->IsHeadquarters())
	{
		UE_LOG(LogRtsBuildingsFacade, Warning, TEXT("Refusing to destroy building %d: it is a headquarters"), InBuildingId.Value);
		return;
	}

	Entities->DestroyBuilding(InBuildingId);
	OnBuildingDestroyed.Broadcast(InBuildingId);
}

namespace
{
	/** No UBuildingType asset argument on purpose — these commands exist only to exercise the
	 * facade's queries/events without editor access; the spawned building's type stays null. */
	static FAutoConsoleCommandWithWorldAndArgs GSpawnBuildingCommand(
		TEXT("Rts.SpawnBuilding"),
		TEXT("Rts.SpawnBuilding <ProvinceIdx> <OwnerCharacterIdx> - spawns a building through UBuildings."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
			[](const TArray<FString>& Args, UWorld* World)
			{
				const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
				UBuildings* Buildings = GameState ? GameState->FindComponentByClass<UBuildings>() : nullptr;
				if (!Buildings)
				{
					UE_LOG(LogRtsBuildingsFacade, Warning, TEXT("No UBuildings component in this world"));
					return;
				}

				if (Args.Num() < 2)
				{
					UE_LOG(LogRtsBuildingsFacade, Warning, TEXT("Usage: Rts.SpawnBuilding <ProvinceIdx> <OwnerCharacterIdx>"));
					return;
				}

				const FProvinceId ProvinceId(FCString::Atoi(*Args[0]));
				const FCharacterId OwnerId(FCString::Atoi(*Args[1]));

				const FBuildingId NewId = Buildings->Add(nullptr, ProvinceId, OwnerId);
				UE_LOG(LogRtsBuildingsFacade, Log, TEXT("Spawned building %d"), NewId.Value);
			}));

	static FAutoConsoleCommandWithWorldAndArgs GSetBuildingOwnerCommand(
		TEXT("Rts.SetBuildingOwner"),
		TEXT("Rts.SetBuildingOwner <BuildingIdx> <OwnerCharacterIdx> - changes a building's owner through UBuildings."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
			[](const TArray<FString>& Args, UWorld* World)
			{
				const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
				UBuildings* Buildings = GameState ? GameState->FindComponentByClass<UBuildings>() : nullptr;
				if (!Buildings)
				{
					UE_LOG(LogRtsBuildingsFacade, Warning, TEXT("No UBuildings component in this world"));
					return;
				}

				if (Args.Num() < 2)
				{
					UE_LOG(LogRtsBuildingsFacade, Warning, TEXT("Usage: Rts.SetBuildingOwner <BuildingIdx> <OwnerCharacterIdx>"));
					return;
				}

				const FBuildingId BuildingId(FCString::Atoi(*Args[0]));
				Buildings->SetCharacterOwner(BuildingId, FCharacterId(FCString::Atoi(*Args[1])));

				UE_LOG(LogRtsBuildingsFacade, Log, TEXT("Building %d owner changed"), BuildingId.Value);
			}));
}
