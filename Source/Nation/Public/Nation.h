#pragma once

#include "CoreMinimal.h"
#include "Flag.h"
#include "Modules/ModuleManager.h"
#include "Nation.generated.h"

class FNationModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

USTRUCT(BlueprintType)
struct NATION_API FNationInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name = FText::FromString("Unnamed Nation");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FFlag Flag = {};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNationEvent);

UCLASS(BlueprintType)
class NATION_API ANation : public AActor
{
    GENERATED_BODY()
public:
    ANation();

    UPROPERTY(BlueprintAssignable)
    FNationEvent OnInfoChanged;

    UPROPERTY(BlueprintAssignable)
    FNationEvent OnStatesChanged;
    
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetInfo, Replicated, ReplicatedUsing=OnRep_Info)
    FNationInfo Info;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_States)
    TArray<class AState*> States;
public:

	UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    FORCEINLINE void SetInfo(const FNationInfo& NewInfo)
	{
	    Info = NewInfo;
	    OnInfoChanged.Broadcast();
	}

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
    UFUNCTION()
    FORCEINLINE void OnRep_Info()
    {
        OnInfoChanged.Broadcast();
    }
    UFUNCTION()
    FORCEINLINE void OnRep_States()
    {
        OnStatesChanged.Broadcast();
    }
};