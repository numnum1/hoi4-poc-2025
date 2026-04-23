#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Demography.generated.h"

class URace;

class FDemographyModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDemographyEvent);

UCLASS(BlueprintType)
class UDemography : public UActorComponent
{
    GENERATED_BODY()
public:
    UDemography();

    UPROPERTY(BlueprintAssignable)
    FDemographyEvent OnFieldsChanged;
private:
    UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, meta=(AllowPrivateAccess), Replicated, ReplicatedUsing=OnRep_Fields, BlueprintSetter=SetRace)
    URace* Race = nullptr;
    
    UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, meta=(AllowPrivateAccess), Replicated, ReplicatedUsing=OnRep_Fields, BlueprintSetter=SetCount)
    int32 Count;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintSetter)
    FORCEINLINE void SetRace(URace* NewRace)
    {
        Setter(Race, NewRace);
    }
    
    UFUNCTION(BlueprintSetter)
    FORCEINLINE void SetCount(const int32 NewCount)
    {
        Setter(Count, NewCount);
    }
    
private:
    UFUNCTION()
    FORCEINLINE void OnRep_Fields()
    {
        OnFieldsChanged.Broadcast();
    }
    template<class T>
    FORCEINLINE void Setter(T& Field, const T& NewField)
    {
        if (Field == NewField) return;
        Field = NewField;
        OnFieldsChanged.Broadcast();
    }
};