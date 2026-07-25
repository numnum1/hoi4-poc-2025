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

FProvinceDisplayInfo UProvinces::GetDisplayInfo(const int32 InProvinceIndex) const
{
	return Display.IsValidIndex(InProvinceIndex) ? Display[InProvinceIndex] : FProvinceDisplayInfo();
}

int32 UProvinces::FindByMapId(const FName InMapId) const
{
	const int32* Found = MapIds.Find(InMapId);
	return Found ? *Found : INDEX_NONE;
}

int32 UProvinces::GetOwningNation(const int32 InProvinceIndex) const
{
	const UProvinceEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetOwningNation(InProvinceIndex) : INDEX_NONE;
}

TArray<int32> UProvinces::CollectProvincesOfNation(const int32 InNationIndex) const
{
	const UProvinceEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectProvincesOfNation(InNationIndex) : TArray<int32>();
}

void UProvinces::Setup(const TArray<FProvinceSpawnData>& NewProvinces)
{
	Display.Reset(NewProvinces.Num());
	MapIds.Empty(NewProvinces.Num());

	for (int32 I = 0; I < NewProvinces.Num(); ++I)
	{
		Display.Add(NewProvinces[I].Display);
		MapIds.Emplace(NewProvinces[I].Display.MapId, I);
	}

	UProvinceEntitySubsystem* Entities = GetEntities();
	if (Entities)
	{
		Entities->CreateProvinces(NewProvinces);
	}
}

void UProvinces::SetOwningNation(const int32 InProvinceIndex, const int32 InNationIndex)
{
	UProvinceEntitySubsystem* Entities = GetEntities();
	if (Entities)
	{
		Entities->SetOwningNation(InProvinceIndex, InNationIndex);
	}
}
