#pragma once

#include "CoreMinimal.h"
#include "JuristicPerson/Public/JuristicPerson.h"
#include "Modules/ModuleManager.h"
#include "RtsCharacter.generated.h"

class UCharacterBuild;
class URace;
class USchedule;
class UOrganizationPosition;
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
    TSoftObjectPtr<class AState> StateOfBirth = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    URace* GeneticRace = nullptr;
};

class UCharacterRole;
class ARtsCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRtsCharacterEvent);

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

    UPROPERTY(BlueprintAssignable)
    FRtsCharacterEvent OnCurrentStateChanged;

    // Events
    UPROPERTY(BlueprintAssignable)
    FRtsCharacterEvent FoundOrganisation;
    
private:
    UPROPERTY(EditAnywhere, Replicated, meta=(AllowPrivateAccess), ReplicatedUsing=OnRep_CharacterInfo, BlueprintGetter=GetCharacterInfo, BlueprintSetter=SetCharacterInfo)
    FRtsCharacterInfo CharacterInfo;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Roles, Instanced, BlueprintGetter=GetRoles)
    TArray<UCharacterRole*> Roles;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_CurrentState)
    AState* CurrentState;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    USchedule* Schedule;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    UCharacterBuild* Build;
    
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
        OnCharacterInfoChanged.Broadcast();
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
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // Actions
    UFUNCTION(BlueprintCallable, Server, Reliable, meta=(UnsafafeDuringConstruction))
    void Server_FoundOrganisation(const FText& InName, const FSlateBrush& InLogo, UOrganizationPosition* InPosition);
private:
    UFUNCTION()
    FORCEINLINE void OnRep_CharacterInfo()
    {
        OnCharacterInfoChanged.Broadcast();
    }
    UFUNCTION()
    FORCEINLINE void OnRep_Roles()
    {
        OnRolesChanged.Broadcast();
    }
    UFUNCTION()
    FORCEINLINE void OnRep_CurrentState()
    {
        OnCurrentStateChanged.Broadcast();
    }
};
