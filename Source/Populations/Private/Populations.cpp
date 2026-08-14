#include "Populations.h"

#include "PopulationEntitySubsystem.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "FPopulationsModule"

void FPopulationsModule::StartupModule()
{

}

void FPopulationsModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPopulationsModule, Populations)

UPopulations::UPopulations()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UPopulationEntitySubsystem* UPopulations::GetEntities() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UPopulationEntitySubsystem>() : nullptr;
}

void UPopulations::SetupEthnicities(const TArray<FEthnicityInfo>& InEthnicities)
{
	Ethnicities = InEthnicities;
}

FEthnicityInfo UPopulations::GetEthnicityInfo(const FEthnicityId InEthnicityId) const
{
	return Ethnicities.IsValidIndex(InEthnicityId.Value) ? Ethnicities[InEthnicityId.Value] : FEthnicityInfo();
}

int32 UPopulations::Num() const
{
	const UPopulationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->NumSlots() : 0;
}

bool UPopulations::IsAlive(const FPopId InPopId) const
{
	const UPopulationEntitySubsystem* Entities = GetEntities();
	return Entities && Entities->IsAlive(InPopId);
}

FPopInfo UPopulations::GetPopInfo(const FPopId InPopId) const
{
	const UPopulationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetPopInfo(InPopId) : FPopInfo();
}

TArray<FPopInfo> UPopulations::CollectPops() const
{
	const UPopulationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectPops() : TArray<FPopInfo>();
}

TArray<FPopInfo> UPopulations::CollectPopsInProvince(const FProvinceId InProvinceId) const
{
	const UPopulationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectPopsInProvince(InProvinceId) : TArray<FPopInfo>();
}

int32 UPopulations::GetProvincePopulation(const FProvinceId InProvinceId) const
{
	const UPopulationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetProvincePopulation(InProvinceId) : 0;
}

FEthnicityId UPopulations::GetDominantEthnicity(const FProvinceId InProvinceId) const
{
	FEthnicityId Dominant;
	int32 DominantSize = 0;

	for (const FPopInfo& Pop : CollectPopsInProvince(InProvinceId))
	{
		// Strictly greater, so an equal-sized group encountered later does not take over — with
		// pops created in ethnicity order that makes the lower id win a tie.
		if (Pop.Size > DominantSize)
		{
			DominantSize = Pop.Size;
			Dominant = Pop.EthnicityId;
		}
	}

	return Dominant;
}

FPopId UPopulations::Create(const FPopSpawnData& InSpawnData)
{
	UPopulationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CreatePop(InSpawnData) : FPopId();
}

void UPopulations::Setup(const TArray<FPopSpawnData>& InSpawnData)
{
	for (const FPopSpawnData& SpawnData : InSpawnData)
	{
		Create(SpawnData);
	}

	OnPopulationsSetup.Broadcast();
}

void UPopulations::SetSize(const FPopId InPopId, const int32 InSize)
{
	UPopulationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->SetSize(InPopId, InSize);
	OnPopSizeChanged.Broadcast(InPopId);
}

void UPopulations::Destroy(const FPopId InPopId)
{
	UPopulationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->DestroyPop(InPopId);
	OnPopDestroyed.Broadcast(InPopId);
}
