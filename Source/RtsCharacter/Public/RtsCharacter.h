#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "RtsCharacter.generated.h"

class USelfRole;

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

class UCharacterRole;
class ARtsCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRtsCharacterEvent, ARtsCharacter*, InCharacter);

UCLASS(Blueprintable)
class RTSCHARACTER_API ARtsCharacter : public AActor
{
    GENERATED_BODY()
public:
    ARtsCharacter();

    UPROPERTY(BlueprintAssignable)
    FRtsCharacterEvent OnCharacterInfoChanged;

    UPROPERTY(BlueprintAssignable)
    FRtsCharacterEvent OnRolesChanged;
    
private:
    UPROPERTY(EditAnywhere, Replicated, meta=(AllowPrivateAccess), ReplicatedUsing=OnRep_CharacterInfo, BlueprintGetter=GetCharacterInfo, BlueprintSetter=SetCharacterInfo)
    FRtsCharacterInfo CharacterInfo;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Roles, Instanced, BlueprintGetter=GetRoles)
    TArray<UCharacterRole*> Roles;
    
public:
    
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    UFUNCTION(BlueprintGetter)
    const FRtsCharacterInfo& GetCharacterInfo() const
    {
        return CharacterInfo;
    }
    
    UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    FORCEINLINE void SetCharacterInfo(const FRtsCharacterInfo& NewCharacterInfo)
    {
        CharacterInfo = NewCharacterInfo;
        OnCharacterInfoChanged.Broadcast(this);
    }
    
    UFUNCTION()
    FORCEINLINE void OnRep_CharacterInfo()
    {
        OnCharacterInfoChanged.Broadcast(this);
    }
    
    UFUNCTION(BlueprintGetter)
    const TArray<UCharacterRole*>& GetRoles() const
    {
        return Roles;
    }
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AddRole(UCharacterRole* InRole);
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void RemoveRole(UCharacterRole* InRole);
    
    UFUNCTION()
    FORCEINLINE void OnRep_Roles()
    {
        OnRolesChanged.Broadcast(this);
    }
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
