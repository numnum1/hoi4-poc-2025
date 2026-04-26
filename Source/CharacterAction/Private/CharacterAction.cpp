#include "CharacterAction.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FCharacterActionModule"

void FCharacterActionModule::StartupModule()
{
    
}

void FCharacterActionModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FCharacterActionModule, CharacterAction)

UCharacterAction::UCharacterAction()
{
	ActionName = FText::FromString("Unnamed Action");
	Icon = FSlateNoResource();
}

void UCharacterAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCharacterAction, ActionName);
	DOREPLIFETIME(UCharacterAction, Icon);
}

void UCharacterAction::Click_Implementation()
{
	ensureMsgf(false, TEXT("Override this"));
}
