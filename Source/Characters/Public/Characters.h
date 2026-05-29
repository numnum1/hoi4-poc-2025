#pragma once

#include "CoreMinimal.h"
#include "CharacterInfo.h"
#include "Schedule.h"
#include "Modules/ModuleManager.h"
#include "Characters.generated.h"

class FCharactersModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

USTRUCT(BlueprintType)
struct CHARACTERS_API FOrganizationRoles
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> OrganizationIndexes = {};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharactersEvent, int32, InCharacterIndex);

UCLASS()
class CHARACTERS_API UCharacters : public UActorComponent
{
    GENERATED_BODY()
public:
    UCharacters();

    UPROPERTY(BlueprintAssignable)
    FCharactersEvent OnRolesChanged;
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FCharacterInfo> Infos;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<int32> CurrentProvinceIndexes;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<int32> ScheduleCharacterIndexMapping;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FOrganizationRoles> Roles;
public:
    FORCEINLINE const TArray<FOrganizationRoles>& GetRoles() const { return Roles; }
    
    UFUNCTION(BlueprintCallable)
    void Setup(
        const TArray<FCharacterInfo>& NewInfos,
        const TArray<int32>& NewCurrentProvinceIndexes,
        const TArray<int32>& NewScheduleCharacterIndexMapping
    );
    
    UFUNCTION(BlueprintCallable)
    void AddRole(int32 InCharacterIndex, int32 InAtOrganizationIndex);
};