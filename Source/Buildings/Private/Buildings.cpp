#include "Buildings.h"

#include "BuildingCatalogSettings.h"
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

void UBuildings::EnsureCatalogLoaded() const
{
	if (bCatalogLoaded)
	{
		return;
	}
	bCatalogLoaded = true;

	const UBuildingCatalogSettings* Settings = GetDefault<UBuildingCatalogSettings>();
	if (Settings == nullptr)
	{
		return;
	}

	for (const TSoftObjectPtr<UBuildingType>& SoftType : Settings->BuildingTypes)
	{
		if (UBuildingType* LoadedType = SoftType.LoadSynchronous())
		{
			BuildingTypes.AddUnique(LoadedType);
		}
	}
}

int32 UBuildings::ResolveTypeIndex(UBuildingType* InBuildingType)
{
	EnsureCatalogLoaded();
	const int32 Existing = BuildingTypes.Find(InBuildingType);
	return Existing != INDEX_NONE ? Existing : BuildingTypes.Add(InBuildingType);
}

UBuildingType* UBuildings::GetBuildingType(const FBuildingId InBuildingId) const
{
	EnsureCatalogLoaded();

	const UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return nullptr;
	}

	const int32 TypeIndex = Entities->GetTypeIndex(InBuildingId);
	return BuildingTypes.IsValidIndex(TypeIndex) ? BuildingTypes[TypeIndex] : nullptr;
}

TArray<UBuildingType*> UBuildings::GetHeadquarterBuildingTypes() const
{
	EnsureCatalogLoaded();

	TArray<UBuildingType*> Result;
	for (UBuildingType* Type : BuildingTypes)
	{
		if (Type && Type->IsHeadquarters())
		{
			Result.Add(Type);
		}
	}
	return Result;
}

FProvinceId UBuildings::GetProvince(const FBuildingId InBuildingId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetProvince(InBuildingId) : FProvinceId();
}

EBuildingOwnerType UBuildings::GetOwnerType(const FBuildingId InBuildingId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetOwnerType(InBuildingId) : EBuildingOwnerType::BOT_None;
}

FCharacterId UBuildings::GetOwnerCharacter(const FBuildingId InBuildingId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetOwnerCharacter(InBuildingId) : FCharacterId();
}

FOrganizationId UBuildings::GetOwnerOrganization(const FBuildingId InBuildingId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetOwnerOrganization(InBuildingId) : FOrganizationId();
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

TArray<FBuildingId> UBuildings::CollectBuildingsOwnedByOrganization(const FOrganizationId InOwnerId) const
{
	const UBuildingEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectBuildingsOwnedByOrganization(InOwnerId) : TArray<FBuildingId>();
}

FBuildingId UBuildings::AddForCharacter(UBuildingType* InBuildingType, const FProvinceId InProvinceId, const FCharacterId InOwnerId)
{
	UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return FBuildingId();
	}

	const int32 TypeIndex = ResolveTypeIndex(InBuildingType);
	const FBuildingId NewId = Entities->CreateBuildingForCharacter(InProvinceId, TypeIndex, InOwnerId);
	OnBuildingAdded.Broadcast(NewId);
	return NewId;
}

FBuildingId UBuildings::AddForOrganization(UBuildingType* InBuildingType, const FProvinceId InProvinceId, const FOrganizationId InOwnerId)
{
	UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return FBuildingId();
	}

	const int32 TypeIndex = ResolveTypeIndex(InBuildingType);
	const FBuildingId NewId = Entities->CreateBuildingForOrganization(InProvinceId, TypeIndex, InOwnerId);
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

void UBuildings::SetOrganizationOwner(const FBuildingId InBuildingId, const FOrganizationId InNewOwnerId)
{
	UBuildingEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return;
	}

	Entities->SetOrganizationOwner(InBuildingId, InNewOwnerId);
	OnBuildingOwnerChanged.Broadcast(InBuildingId);
}

void UBuildings::Destroy(const FBuildingId InBuildingId)
{
	UBuildingEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	// An organization's seat cannot be taken away from it. Enforced here rather than by leaving the
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

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------

namespace
{
	/** No UBuildingType asset argument on purpose — these commands exist only to exercise the
	 * facade's queries/events without editor access; the spawned building's type stays null. */
	static FAutoConsoleCommandWithWorldAndArgs GSpawnBuildingCommand(
		TEXT("Rts.SpawnBuilding"),
		TEXT("Rts.SpawnBuilding <ProvinceIdx> <Char|Org> <OwnerIdx> - spawns a building through UBuildings."),
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

				if (Args.Num() < 3)
				{
					UE_LOG(LogRtsBuildingsFacade, Warning, TEXT("Usage: Rts.SpawnBuilding <ProvinceIdx> <Char|Org> <OwnerIdx>"));
					return;
				}

				const FProvinceId ProvinceId(FCString::Atoi(*Args[0]));
				const int32 OwnerIdx = FCString::Atoi(*Args[2]);

				FBuildingId NewId;
				if (Args[1] == TEXT("Char"))
				{
					NewId = Buildings->AddForCharacter(nullptr, ProvinceId, FCharacterId(OwnerIdx));
				}
				else if (Args[1] == TEXT("Org"))
				{
					NewId = Buildings->AddForOrganization(nullptr, ProvinceId, FOrganizationId(OwnerIdx));
				}
				else
				{
					UE_LOG(LogRtsBuildingsFacade, Warning, TEXT("Owner kind must be 'Char' or 'Org', got '%s'"), *Args[1]);
					return;
				}

				UE_LOG(LogRtsBuildingsFacade, Log, TEXT("Spawned building %d"), NewId.Value);
			}));

	static FAutoConsoleCommandWithWorldAndArgs GSetBuildingOwnerCommand(
		TEXT("Rts.SetBuildingOwner"),
		TEXT("Rts.SetBuildingOwner <BuildingIdx> <Char|Org> <OwnerIdx> - changes a building's owner through UBuildings."),
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

				if (Args.Num() < 3)
				{
					UE_LOG(LogRtsBuildingsFacade, Warning, TEXT("Usage: Rts.SetBuildingOwner <BuildingIdx> <Char|Org> <OwnerIdx>"));
					return;
				}

				const FBuildingId BuildingId(FCString::Atoi(*Args[0]));
				const int32 OwnerIdx = FCString::Atoi(*Args[2]);

				if (Args[1] == TEXT("Char"))
				{
					Buildings->SetCharacterOwner(BuildingId, FCharacterId(OwnerIdx));
				}
				else if (Args[1] == TEXT("Org"))
				{
					Buildings->SetOrganizationOwner(BuildingId, FOrganizationId(OwnerIdx));
				}
				else
				{
					UE_LOG(LogRtsBuildingsFacade, Warning, TEXT("Owner kind must be 'Char' or 'Org', got '%s'"), *Args[1]);
					return;
				}

				UE_LOG(LogRtsBuildingsFacade, Log, TEXT("Building %d owner changed"), BuildingId.Value);
			}));
}
