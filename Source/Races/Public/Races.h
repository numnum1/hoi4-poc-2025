#pragma once

#include "CoreMinimal.h"
#include "Race.h"
#include "RaceCategory.h"
#include "LifeType.h"
#include "Species.h"
#include "Modules/ModuleManager.h"
#include "Races.generated.h"

class FRacesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UCLASS(ClassGroup=(Rts), meta=(BlueprintSpawnableComponent))
class RACES_API URaces : public UActorComponent
{
    GENERATED_BODY()
public:
    URaces();
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), BlueprintSetter=SetCategories)
    TArray<FRaceCategory> Categories;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), BlueprintSetter=SetLifeTypes)
    TArray<FLifeType> LifeTypes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), BlueprintSetter=SetSpecies)
    TArray<FSpecies> Species;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), BlueprintSetter=SetRaces)
    TArray<FRace> Races;

public:
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    FORCEINLINE void SetCategories(const TArray<FRaceCategory>& NewCategories)
    {
        Categories = NewCategories;
    }

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    FORCEINLINE void SetLifeTypes(const TArray<FLifeType>& NewLifeTypes)
    {
        LifeTypes = NewLifeTypes;
    }

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    FORCEINLINE void SetSpecies(const TArray<FSpecies>& NewSpecies)
    {
        Species = NewSpecies;
    }

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    FORCEINLINE void SetRaces(const TArray<FRace>& NewRaces)
    {
        Races = NewRaces;
    }
};
