#include "Provinces.h"

#include "ProvinceEntitySubsystem.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "FProvincesModule"

void FProvincesModule::StartupModule()
{

}

void FProvincesModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FProvincesModule, Provinces)

UProvinces::UProvinces()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UProvinceEntitySubsystem* UProvinces::GetEntities() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UProvinceEntitySubsystem>() : nullptr;
}

bool UProvinces::IsAlive(const FProvinceId InProvinceId) const
{
	const UProvinceEntitySubsystem* Entities = GetEntities();
	return Entities && Entities->IsAlive(InProvinceId);
}

FProvinceDisplayInfo UProvinces::GetDisplayInfo(const FProvinceId InProvinceId) const
{
	return Display.IsValidIndex(InProvinceId.Value) ? Display[InProvinceId.Value] : FProvinceDisplayInfo();
}

FProvinceId UProvinces::FindByMapId(const FName InMapId) const
{
	const FProvinceId* Found = MapIds.Find(InMapId);
	return Found ? *Found : FProvinceId();
}

FNationId UProvinces::GetOwningNation(const FProvinceId InProvinceId) const
{
	const UProvinceEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetOwningNation(InProvinceId) : FNationId();
}

int32 UProvinces::GetTimezoneOffset(const FProvinceId InProvinceId) const
{
	const UProvinceEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetTimezoneOffset(InProvinceId) : 0;
}

TArray<FProvinceId> UProvinces::CollectProvincesOfNation(const FNationId InNationId) const
{
	const UProvinceEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectProvincesOfNation(InNationId) : TArray<FProvinceId>();
}

FProvinceId UProvinces::Create(const FProvinceSpawnData& InSpawnData)
{
	UProvinceEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return FProvinceId();
	}

	// The entity claims the id; the display row must line up with it.
	const FProvinceId NewId = Entities->CreateProvince(InSpawnData);
	check(NewId.Value == Display.Num());
	Display.Add(InSpawnData.Display);
	MapIds.Emplace(InSpawnData.Display.MapId, NewId);

	return NewId;
}

void UProvinces::Setup(const TArray<FProvinceSpawnData>& NewProvinces)
{
	Display.Reset(NewProvinces.Num());
	MapIds.Empty(NewProvinces.Num());

	for (const FProvinceSpawnData& SpawnData : NewProvinces)
	{
		Create(SpawnData);
	}
}

void UProvinces::SetOwningNation(const FProvinceId InProvinceId, const FNationId InNationId)
{
	UProvinceEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->SetOwningNation(InProvinceId, InNationId);
	OnOwningNationChanged.Broadcast(InProvinceId);
}

void UProvinces::Destroy(const FProvinceId InProvinceId)
{
	UProvinceEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->DestroyProvince(InProvinceId);
	OnProvinceDestroyed.Broadcast(InProvinceId);
}
