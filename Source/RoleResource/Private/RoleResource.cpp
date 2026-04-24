#include "RoleResource.h"

#define LOCTEXT_NAMESPACE "FRoleResourceModule"

void FRoleResourceModule::StartupModule()
{
    
}

void FRoleResourceModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FRoleResourceModule, RoleResource)

URoleResource::URoleResource()
{
	ResourceName = FText::FromString("Unnamed Resource");
	Icon = FSlateNoResource();
}