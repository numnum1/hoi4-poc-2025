#pragma once

#include "CoreMinimal.h"
#include "JobLevel.h"
#include "Race.h"
#include "RacialLevel.h"
#include "Modules/ModuleManager.h"
#include "CharacterBuild.generated.h"


class FCharacterBuildModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UCLASS()
class UCharacterBuild : public UActorComponent
{
    GENERATED_BODY()
public:
    UCharacterBuild();
private:
    // Levels
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
    TArray<FRacialLevel> RacialLevels;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
    TArray<FJobLevel> JobLevels;
public:
    // Computations
    UFUNCTION(BlueprintCallable)
    int32 ComputeStrength() const;
    UFUNCTION(BlueprintCallable)
    int32 ComputeDexterity() const;
    UFUNCTION(BlueprintCallable)
    int32 ComputeConstitution() const;
    UFUNCTION(BlueprintCallable)
    int32 ComputeIntelligence() const;
    UFUNCTION(BlueprintCallable)
    int32 ComputeWisdom() const;
    UFUNCTION(BlueprintCallable)
    int32 ComputeCharisma() const;
private:
    // Utils
    FORCEINLINE static float CalculateWeightedAverage(const TArray<TTuple<int32, int32>>& Data) 
    {
        int32 WeightedSum = 0;
        int32 TotalWeight = 0;

        for (const auto& Pair : Data)
        {
            int32 Value = Pair.Get<0>();
            int32 Weight = Pair.Get<1>();

            WeightedSum += Value * Weight;
            TotalWeight += Weight;
        }

        if (TotalWeight == 0)
        {
            return 0.0f;
        }

        return static_cast<float>(WeightedSum) / TotalWeight;
    }
    
    template<typename T>
    FORCEINLINE int32 GetBaseSkill(T Extractor) const
    {
        TArray<TTuple<int32, int32>> Skills = {};
        for (const auto& [Race, Level] : RacialLevels)
        {
            Skills.Add(TTuple<int32, int32>{Extractor(Race->BaseSkills), Level});
        }
        const float Avg = CalculateWeightedAverage(Skills);
        return FMath::RoundToInt(Avg);
    }
    
    template<typename T>
    FORCEINLINE int32 ComputeSkill(T Extractor) const
    {
        const int32 BaseSkill = GetBaseSkill(Extractor);
        return BaseSkill;
    }
};

