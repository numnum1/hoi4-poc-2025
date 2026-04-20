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
    
private:
    UPROPERTY(EditAnywhere, Replicated, meta=(AllowPrivateAccess), ReplicatedUsing=OnRep_StateInfo, BlueprintGetter=GetStateInfo, BlueprintSetter=SetStateInfo)
    FStateInfo StateInfo;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    ATimezone* Timezone;
    
public:
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
    
    UFUNCTION()
    FORCEINLINE void OnRep_StateInfo()
    {
        OnInfoChanged.Broadcast();
    }
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // Visuals
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void SetHighlighted(bool bNewHighlighted);
    
};
