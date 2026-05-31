// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RaceAsset.generated.h"

UCLASS(BlueprintType)
class SCENARIO_API URaceTraitAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	URaceTraitAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText TraitName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText TraitDescription;
public:
	FORCEINLINE const FText& GetTraitName() const { return TraitName; }
	FORCEINLINE const FText& GetTraitDescription() const { return TraitDescription; }
};

UCLASS(BlueprintType)
class SCENARIO_API ULifeTypeAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	ULifeTypeAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText LifeTypeName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<URaceTraitAsset*> Traits;
public:
	FORCEINLINE const FText& GetLifeTypeName() const { return LifeTypeName; }
	FORCEINLINE const TArray<URaceTraitAsset*>& GetTraits() const { return Traits; }
};

UCLASS(Abstract, NotBlueprintType, NotBlueprintable)
class SCENARIO_API UGenomeAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<URaceTraitAsset*> CollectRaceTraits() const;
};

UCLASS(BlueprintType)
class SCENARIO_API URaceCategoryAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	URaceCategoryAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText CategoryName;
};

/**
 * 
 */
UCLASS()
class SCENARIO_API USpeciesAsset : public UGenomeAsset
{
	GENERATED_BODY()
public:
	USpeciesAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText SpeciesName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	URaceCategoryAsset* Category;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	ULifeTypeAsset* LifeType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<URaceTraitAsset*> Traits;
public:
	virtual TArray<URaceTraitAsset*> CollectRaceTraits_Implementation() const override;
};

/**
 * 
 */
UCLASS()
class SCENARIO_API URaceAsset : public UGenomeAsset
{
	GENERATED_BODY()
public:
	URaceAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText RaceName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	USpeciesAsset* Species;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<URaceTraitAsset*> Traits;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Abilities")
	uint8 Strength = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Abilities")
	uint8 Dexterity = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Abilities")
	uint8 Constitution = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Abilities")
	uint8 Intelligence = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Abilities")
	uint8 Wisdom = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Abilities")
	uint8 Charisma = 0;
	
public:
	FORCEINLINE FText GetRaceName() const { return RaceName; }
	
	FORCEINLINE uint8 GetStrength() const { return Strength; }
	FORCEINLINE uint8 GetDexterity() const { return Dexterity; }
	FORCEINLINE uint8 GetConstitution() const { return Constitution; }
	FORCEINLINE uint8 GetIntelligence() const { return Intelligence; }
	FORCEINLINE uint8 GetWisdom() const { return Wisdom; }
	FORCEINLINE uint8 GetCharisma() const { return Charisma; }
	
	virtual TArray<URaceTraitAsset*> CollectRaceTraits_Implementation() const override;
};
