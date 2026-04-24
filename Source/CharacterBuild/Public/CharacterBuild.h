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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterBuildEvent);

UCLASS()
class UCharacterBuild : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterBuild();

	UPROPERTY(BlueprintAssignable)
	FCharacterBuildEvent OnChanged;

private:
	// Levels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Fields)
	TArray<FRacialLevel> RacialLevels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Fields)
	TArray<FJobLevel> JobLevels;
	// Stats
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Fields)
	FSkillSet SkillSet;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Fields)
	TArray<URaceTrait*> Traits;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	UFUNCTION()
	FORCEINLINE void OnRep_Fields()
	{
		OnChanged.Broadcast();
	}

	// Computations
	UFUNCTION(CallInEditor, Category="CharacterBuild")
	void RecomputeValues();
	
	void RecollectTraits();
	
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

	template <typename T>
	FORCEINLINE int32 GetBaseSkill(T Extractor) const
	{
		TArray<TTuple<int32, int32>> Skills = {};
		for (const auto& [Genome, Level] : RacialLevels)
		{
			#if WITH_EDITOR
				if (!GIsEditor)
				{
					ensureMsgf(Genome, TEXT("Character %s has nullptr in race level"), *GetOwner()->GetName());
					ensureMsgf(Level, TEXT("Character %s has 0 level"), *GetOwner()->GetName());
					if (!Genome || Level <= 0) continue;		
				}
			#endif 
			URace* Race = Cast<URace>(Genome);
			if (!Race) continue; // Is something else
			Skills.Add(TTuple<int32, int32>{Extractor(Race->BaseSkills), Level});
		}
		const float Avg = CalculateWeightedAverage(Skills);
		return FMath::RoundToInt(Avg);
	}

	template <typename T>
	FORCEINLINE int32 ComputeSkill(T Extractor) const
	{
		const int32 BaseSkill = GetBaseSkill(Extractor);
		return BaseSkill;
	}
};
