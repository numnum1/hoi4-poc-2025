#pragma once

#include "CoreMinimal.h"
#include "Race.h"
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), BlueprintSetter=SetRaces, Replicated)
    TArray<FRace> Races;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    FORCEINLINE void SetRaces(const TArray<FRace>& NewRaces)
    {
        Races = NewRaces;
    }
};
