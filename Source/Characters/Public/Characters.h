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

UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
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
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void Setup(
        const TArray<FCharacterInfo>& NewInfos,
        const TArray<int32>& NewCurrentProvinceIndexes,
        const TArray<int32>& NewScheduleCharacterIndexMapping,
        const TArray<FSchedule>& NewSchedules
    );
    
};