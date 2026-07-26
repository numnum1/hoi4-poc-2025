#include "Characters.h"

#include "CharacterEntitySubsystem.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "FCharactersModule"

void FCharactersModule::StartupModule()
{

}

void FCharactersModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCharactersModule, Characters)

UCharacters::UCharacters()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bWantsInitializeComponent = true;
}

void UCharacters::InitializeComponent()
{
	Super::InitializeComponent();

	if (UCharacterEntitySubsystem* Entities = GetEntities())
	{
		Entities->OnCharacterSimulated.AddUObject(this, &UCharacters::HandleCharacterSimulated);
	}
}

void UCharacters::UninitializeComponent()
{
	if (UCharacterEntitySubsystem* Entities = GetEntities())
	{
		Entities->OnCharacterSimulated.RemoveAll(this);
	}

	Super::UninitializeComponent();
}

UCharacterEntitySubsystem* UCharacters::GetEntities() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UCharacterEntitySubsystem>() : nullptr;
}

void UCharacters::HandleCharacterSimulated(const FCharacterId InCharacterId)
{
	OnSimulated.Broadcast(InCharacterId);
}

bool UCharacters::IsAlive(const FCharacterId InCharacterId) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities && Entities->IsAlive(InCharacterId);
}

FCharacterId UCharacters::Create(const FCharacterSpawnData& InSpawnData)
{
	UCharacterEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return FCharacterId();
	}

	// The entity claims the id; the display row must line up with it.
	const FCharacterId NewId = Entities->CreateCharacter(InSpawnData);
	check(NewId.Value == Display.Num());
	Display.Add(InSpawnData.Display);

	OnCharacterCreated.Broadcast(NewId);
	return NewId;
}

void UCharacters::Destroy(const FCharacterId InCharacterId)
{
	UCharacterEntitySubsystem* Entities = GetEntities();
	if (!Entities) return;

	Entities->DestroyCharacter(InCharacterId);
	OnCharacterDestroyed.Broadcast(InCharacterId);
}

FCharacterDisplayInfo UCharacters::GetDisplayInfo(const FCharacterId InCharacterId) const
{
	return Display.IsValidIndex(InCharacterId.Value) ? Display[InCharacterId.Value] : FCharacterDisplayInfo();
}

FProvinceId UCharacters::GetCurrentProvince(const FCharacterId InCharacterId) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetCurrentProvince(InCharacterId) : FProvinceId();
}

FAbilities UCharacters::GetAbilities(const FCharacterId InCharacterId) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetAbilities(InCharacterId) : FAbilities();
}

TArray<FPosition> UCharacters::GetPositions(const FCharacterId InCharacterId) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetPositions(InCharacterId) : TArray<FPosition>();
}

FSchedule UCharacters::GetSchedule(const FCharacterId InCharacterId) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetSchedule(InCharacterId) : FSchedule();
}

FScheduleId UCharacters::GetScheduleId(const FCharacterId InCharacterId) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetScheduleId(InCharacterId) : FScheduleId();
}

FCharacterVitals UCharacters::GetVitals(const FCharacterId InCharacterId) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetVitals(InCharacterId) : FCharacterVitals();
}

FCharacterSkills UCharacters::GetSkills(const FCharacterId InCharacterId) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetSkills(InCharacterId) : FCharacterSkills();
}

TArray<FCharacterId> UCharacters::CollectCharactersInProvince(const FProvinceId InProvinceId) const
{
	const UCharacterEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectCharactersInProvince(InProvinceId) : TArray<FCharacterId>();
}

void UCharacters::AddPosition(const FCharacterId InCharacterId, const FOrganizationId InAtOrganizationId, const FRoleId InRoleId)
{
	UCharacterEntitySubsystem* Entities = GetEntities();
	if (!Entities) return;

	Entities->AddPosition(InCharacterId, InAtOrganizationId, InRoleId);
	OnPositionChanged.Broadcast(InCharacterId);
}

void UCharacters::MoveCharacter(const FCharacterId InCharacterId, const FProvinceId NewCurrentProvinceId)
{
	UCharacterEntitySubsystem* Entities = GetEntities();
	if (!Entities) return;

	Entities->MoveCharacter(InCharacterId, NewCurrentProvinceId);
	OnCurrentProvinceChanged.Broadcast(InCharacterId);
}
