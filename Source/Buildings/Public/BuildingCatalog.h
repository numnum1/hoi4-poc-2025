#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BuildingCatalog.generated.h"

class UBuildingType;

UINTERFACE(MinimalAPI)
class UBuildingCatalog : public UInterface
{
	GENERATED_BODY()
};

class BUILDINGS_API IBuildingCatalog
{
	GENERATED_BODY()

public:
	virtual const TArray<TObjectPtr<UBuildingType>>& GetBuildingTypes() const = 0;
};
