#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "State.generated.h"

class ATimezone;
class UEcology;

class FStateModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

/**
 * Static (semi-static) data
 */
USTRUCT(BlueprintType)
struct STATE_API FStateInfo
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name = FText::FromString("Unnamed State");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSlateBrush Portrait = FSlateNoResource();
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UEcology* Ecology = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* ClickSound = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStateEvent);

UCLASS(Blueprintable)
class AState : public AActor
{
    GENERATED_BODY()
public:
    AState();

    UPROPERTY(BlueprintAssignable)
    FStateEvent OnInfoChanged;

    UPROPERTY(BlueprintAssignable)
    FStateEvent OnStateOwnerChanged;
    
private:
    UPROPERTY(EditAnywhere, Replicated, meta=(AllowPrivateAccess), ReplicatedUsing=OnRep_StateInfo, BlueprintGetter=GetStateInfo, BlueprintSetter=SetStateInfo)
    FStateInfo StateInfo;
    
    UPROPERTY(EditAnywhere, Replicated, meta=(AllowPrivateAccess), ReplicatedUsing=OnRep_StateInfo, BlueprintSetter=SetStateOwner, BlueprintReadWrite)
    AActor* StateOwner;    
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    ATimezone* Timezone;
    
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION(BlueprintGetter)
    const FStateInfo& GetStateInfo() const
    {
        return StateInfo;
    }
    
    UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    FORCEINLINE void SetStateInfo(const FStateInfo& NewStateInfo)
    {
        StateInfo = NewStateInfo;
        OnInfoChanged.Broadcast();
    }
    
    UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    FORCEINLINE void SetStateOwner(AActor* NewStateOwner)
    {
        StateOwner = NewStateOwner;
        OnStateOwnerChanged.Broadcast();
    }
    
    // Visuals
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetHighlighted(bool bNewHighlighted);
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetSelected(bool bNewSelected);
    
private:
    UFUNCTION()
    FORCEINLINE void OnRep_StateInfo()
    {
        OnInfoChanged.Broadcast();
    }
    
    UFUNCTION()
    FORCEINLINE void OnRep_StateOwner()
    {
        OnStateOwnerChanged.Broadcast();
    }
    
};
