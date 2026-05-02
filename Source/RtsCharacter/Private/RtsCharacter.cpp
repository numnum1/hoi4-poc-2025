#include "RtsCharacter.h"

#include "BankAccountHolder.h"
#include "CharacterBuild.h"
#include "Net/UnrealNetwork.h"
#include "Organisation.h"
#include "OrganizationRole.h"
#include "Schedule.h"
#include "SelfRole.h"

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
	AddRole(CreateDefaultSubobject<USelfRole>(TEXT("SelfRole")));
	
	CurrentState = nullptr;
	
	Schedule = CreateDefaultSubobject<USchedule>(TEXT("Schedule"));
	Build = CreateDefaultSubobject<UCharacterBuild>(TEXT("Build"));
	Banking = CreateDefaultSubobject<UBankAccountHolder>(TEXT("Banking"));
	
}

void ARtsCharacter::StartPlacingBuildable(UBuildable* InBuildable, AActor* InFor)
{
	UE_LOG(LogTemp, Warning, TEXT("Place buildable"));
}

void ARtsCharacter::AddRole(UCharacterRole* InRole)
{
	verify(InRole);
	ensureMsgf(!Roles.Contains(InRole), TEXT("Adding role that is already registered in character %s"), *CharacterInfo.Name.ToString());

	Roles.AddUnique(InRole);
	OnRolesChanged.Broadcast();
}

void ARtsCharacter::RemoveRole(UCharacterRole* InRole)
{
	verify(InRole);
	ensureMsgf(Roles.Contains(InRole), TEXT("Removing role that is not registered in character %s"), *CharacterInfo.Name.ToString());

	Roles.Remove(InRole);
	RemoveReplicatedSubObject(InRole);
	for (UCharacterAction* Action : InRole->GetActions())
	{
		verify(Action);
		RemoveReplicatedSubObject(Action);
	}
	OnRolesChanged.Broadcast();
}

void ARtsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARtsCharacter, CharacterInfo);
	DOREPLIFETIME(ARtsCharacter, Roles);
	DOREPLIFETIME(ARtsCharacter, CurrentState);
}

void ARtsCharacter::BuyProperty_Implementation(AOrganization* InFor)
{
	BuyProperty.Broadcast(InFor);
}

void ARtsCharacter::BuyPropertyDelegate(AOrganization* InFor)
{
	BuyProperty.Broadcast(InFor);
}

void ARtsCharacter::Server_FoundOrganisation_Implementation(const FText& InName, const FSlateBrush& InLogo, UOrganizationPosition* InPosition)
{
	ensure(!InName.IsEmptyOrWhitespace());

	AOrganization* Founded = GetWorld()->SpawnActor<AOrganization>(AOrganization::StaticClass());
	verify(Founded);
	Founded->SetInfo({InName, InLogo});
	Founded->RegisterNewWorker(this);
	
	// Setup with self as leader of new organization
	UOrganizationRole* OrganizationRole = NewObject<UOrganizationRole>(this);
	OrganizationRole->Setup(Founded, InPosition);
	AddRole(OrganizationRole);
	
}
