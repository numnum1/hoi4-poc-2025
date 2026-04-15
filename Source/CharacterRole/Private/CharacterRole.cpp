#include "CharacterRole.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FCharacterRoleModule"

void FCharacterRoleModule::StartupModule()
{
    
}

void FCharacterRoleModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FCharacterRoleModule, CharacterRole)

UCharacterRole::UCharacterRole()
{
}

void UCharacterRole::SetupWithContext_Implementation(AActor* InContext)
{
	verify(InContext);
	verify(!Context);
	
	Context = InContext;
	OnChanged.Broadcast();
}

void UCharacterRole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCharacterRole, RoleName);
	DOREPLIFETIME(UCharacterRole, Icon);
}
