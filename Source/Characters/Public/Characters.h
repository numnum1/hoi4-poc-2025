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

UCLASS()
class CHARACTERS_API UCharacters : public UActorComponent
{
    GENERATED_BODY()
public:
    UCharacters();
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Replicated)
    TArray<FCharacterInfo> Infos;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Replicated)
    TArray<int32> CurrentProvinceIndexes;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Replicated)
    TArray<int32> ScheduleCharacterIndexMapping;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Replicated)
    TArray<FSchedule> Schedules;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Replicated)
    TArray<FOrganizationRoles> Roles;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    FORCEINLINE const TArray<FOrganizationRoles>& GetRoles() const { return Roles; }
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void Setup(
        const TArray<FCharacterInfo>& NewInfos,
        const TArray<int32>& NewCurrentProvinceIndexes,
        const TArray<int32>& NewScheduleCharacterIndexMapping,
        const TArray<FSchedule>& NewSchedules
    );
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AddRole(int32 InCharacterIndex, int32 InAtOrganizationIndex);
    
};