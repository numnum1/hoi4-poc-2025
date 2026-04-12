#include "RtsCharacter.h"

#include "Net/UnrealNetwork.h"

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
	
	CharacterInfo = {};
}

void ARtsCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARtsCharacter, CharacterInfo);
}
