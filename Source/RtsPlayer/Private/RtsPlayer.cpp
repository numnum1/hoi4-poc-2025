#include "RtsPlayer.h"

#include "TimeSimulation.h"

#define LOCTEXT_NAMESPACE "FRtsPlayerModule"

void FRtsPlayerModule::StartupModule()
{
    
}

void FRtsPlayerModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FRtsPlayerModule, RtsPlayer)

void ARtsPlayer::Server_SetSpeed_Implementation(ATimeSimulation* InTarget, const int32 InSpeed)
{
	verify(InTarget);
	verify(InSpeed == 1 || InSpeed == 2);
	
	InTarget->SetSpeed(InSpeed); 
}
