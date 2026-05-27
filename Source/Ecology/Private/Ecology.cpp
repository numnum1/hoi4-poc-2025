#include "Ecology.h"

#define LOCTEXT_NAMESPACE "FEcologyModule"

void FEcologyModule::StartupModule()
{
    
}

void FEcologyModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FEcologyModule, Ecology)

UEcology::UEcology()
{
	EcologyName = FText::FromString("Unnamed Ecology");
}
