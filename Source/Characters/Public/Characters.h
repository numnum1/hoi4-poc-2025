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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FCharacterInfo> Infos;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
    TMap<FGuid, int32> CharacterIds;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FName> CurrentProvinces;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<int32> ScheduleCharacterMapping;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FSchedule> Schedules;
protected:
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
private:
    UFUNCTION(CallInEditor)
    void InitValuesFromInfos();
};