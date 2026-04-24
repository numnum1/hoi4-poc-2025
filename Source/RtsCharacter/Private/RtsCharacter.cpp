#include "RtsCharacter.h"

#include "CharacterBuild.h"
#include "SelfRole.h"
#include "Net/UnrealNetwork.h"
#include "Organisation.h"
#include "OrganizationRole.h"
#include "Schedule.h"

#define LOCTEXT_NAMESPACE "FRtsCharacterModule"

void FRtsCharacterModule::StartupModule()
{
}

void FRtsCharacterModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRtsCharacterModule, RtsCharacter)

ARtsCharacter::ARtsCharacter()
{
	bAlwaysRelevant = true;
	bReplicates = true;
	bReplicateUsingRegisteredSubObjectList = true;

	CharacterInfo = {};
	CharacterInfo.bMale = true;

	Roles = {};
	
	CurrentState = nullptr;
	
	Schedule = CreateDefaultSubobject<USchedule>(TEXT("Schedule"));
	Build = CreateDefaultSubobject<UCharacterBuild>(TEXT("Build"));
}

void ARtsCharacter::BeginPlay()
{
	Super::BeginPlay();

	for (UCharacterRole* T : Roles)
	{
		verify(T);
		AddReplicatedSubObject(T);
	}
}

void ARtsCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		for (UCharacterRole* T : Roles)
		{
			RemoveRole(T);
		}
	}
}

void ARtsCharacter::AddRole(UCharacterRole* InRole)
{
	verify(InRole);
	ensureMsgf(!Roles.Contains(InRole), TEXT("Adding role that is already registered in character %s"), *CharacterInfo.Name.ToString());

	Roles.AddUnique(InRole);
	AddReplicatedSubObject(InRole);
	OnRolesChanged.Broadcast();
}

void ARtsCharacter::RemoveRole(UCharacterRole* InRole)
{
	verify(InRole);
	ensureMsgf(Roles.Contains(InRole), TEXT("Removing role that is not registered in character %s"), *CharacterInfo.Name.ToString());

	Roles.Remove(InRole);
	RemoveReplicatedSubObject(InRole);
	OnRolesChanged.Broadcast();
}

void ARtsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARtsCharacter, CharacterInfo);
	DOREPLIFETIME(ARtsCharacter, Roles);
	DOREPLIFETIME(ARtsCharacter, CurrentState);
}

void ARtsCharacter::Server_FoundOrganisation_Implementation(const FText& InName, const FSlateBrush& InLogo, UOrganizationPosition* InPosition)
{
	ensure(!InName.IsEmptyOrWhitespace());

	AOrganization* Founded = GetWorld()->SpawnActor<AOrganization>(AOrganization::StaticClass());
	verify(Founded);
	Founded->SetInfo({InName, InLogo});
	
	// Setup with self as leader of new organization
	UOrganizationRole* OrganizationRole = NewObject<UOrganizationRole>(this);
	OrganizationRole->Setup(Founded, InPosition);
	AddRole(OrganizationRole);
	
}
