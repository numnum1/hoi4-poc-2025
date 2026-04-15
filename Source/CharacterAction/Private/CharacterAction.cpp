#include "CharacterAction.h"

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

void UCharacterAction::Click_Implementation(AActor* InRoleContext)
{
	ensureMsgf(false, TEXT("Missing implementation for handle click in %s"), *GetClass()->GetName());
}