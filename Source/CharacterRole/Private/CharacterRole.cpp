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
	RoleName = FText::FromString(TEXT("Unnamed Role"));
	Icon = FSlateNoResource();
	Title = FText::FromString(TEXT(""));
	Actions = {};
	Resources = {};
}

void UCharacterRole::PutResource(const FRoleResourceCount& NewResourceCount)
{
	verify(NewResourceCount.Resource);

	Resources.RemoveAll([NewResourceCount](const FRoleResourceCount& T)
	{
		return NewResourceCount.Resource == T.Resource;
	});
	if (NewResourceCount.Amount == 0) return;
	
	Resources.Add(NewResourceCount);
}

void UCharacterRole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCharacterRole, RoleName);
	DOREPLIFETIME(UCharacterRole, Icon);
	DOREPLIFETIME(UCharacterRole, Title);
	DOREPLIFETIME(UCharacterRole, Actions);
}
