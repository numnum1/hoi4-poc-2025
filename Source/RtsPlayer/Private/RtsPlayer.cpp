#include "RtsPlayer.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FRtsPlayerModule"

void FRtsPlayerModule::StartupModule()
{
    
}

void FRtsPlayerModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FRtsPlayerModule, RtsPlayer)

ARtsPlayer::ARtsPlayer()
{
}

void ARtsPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARtsPlayer, ControlledCharacterId);
}
