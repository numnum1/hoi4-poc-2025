#include "RtsEntityProxy.h"

void URtsEntityProxy::BeginDestroy()
{
	UnbindEvents();
	Super::BeginDestroy();
}
