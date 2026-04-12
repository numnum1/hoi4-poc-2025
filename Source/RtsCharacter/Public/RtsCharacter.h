#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "RtsCharacter.generated.h"

class FRtsCharacterModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};


USTRUCT(BlueprintType)
struct FRtsCharacterInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name = FText::FromString("Unnamed Character");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSlateBrush Portrait = {};
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<class AState> StateOfBirth;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRtsCharacterEvent);

UCLASS(Blueprintable)
class RTSCHARACTER_API ARtsCharacter : public AActor
{
    GENERATED_BODY()
public:
    ARtsCharacter();

    UPROPERTY(BlueprintAssignable)
    FRtsCharacterEvent OnCharacterInfoChanged;
    
private:
    UPROPERTY(EditAnywhere, Replicated, meta=(AllowPrivateAccess), ReplicatedUsing=OnRep_CharacterInfo, BlueprintGetter=GetCharacterInfo, BlueprintSetter=SetCharacterInfo)
    FRtsCharacterInfo CharacterInfo;
    
public:
    UFUNCTION(BlueprintGetter)
    const FRtsCharacterInfo& GetCharacterInfo() const
    {
        return CharacterInfo;
    }
    
    UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    FORCEINLINE void SetCharacterInfo(const FRtsCharacterInfo& NewCharacterInfo)
    {
        CharacterInfo = NewCharacterInfo;
        OnCharacterInfoChanged.Broadcast();
    }
    
    UFUNCTION()
    FORCEINLINE void OnRep_CharacterInfo()
    {
        OnCharacterInfoChanged.Broadcast();
    }
    
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
