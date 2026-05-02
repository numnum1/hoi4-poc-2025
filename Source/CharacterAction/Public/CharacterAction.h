#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "CharacterAction.generated.h"

class FCharacterActionModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterActionEvent);

/**
 * 
 */
UCLASS(BlueprintType, Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class CHARACTERACTION_API UCharacterAction : public UObject
{
    GENERATED_BODY()

public:
    UCharacterAction();

    UPROPERTY(BlueprintAssignable)
    FCharacterActionEvent OnChanged;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Info)
    FText ActionName = FText::FromString("");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Info)
    FSlateBrush Icon = FSlateNoResource();
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
    virtual bool IsSupportedForNetworking() const override { return true; }
	
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Click();
	
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void Setup(const FText& NewActionName, const FSlateBrush& NewIcon)
    {
        ActionName = NewActionName;
        Icon = NewIcon;
        OnChanged.Broadcast();
    }
private:
    UFUNCTION()
    FORCEINLINE void OnRep_Info()
    {
        OnChanged.Broadcast();
    }
	
};
