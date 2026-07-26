// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "UObject/Object.h"
#include "BuildingOwner.generated.h"

UENUM(BlueprintType)
enum class EBuildingOwnerType : uint8
{
	BOT_Character UMETA(DisplayName = "Character"),
	BOT_Organization UMETA(DisplayName = "Organization")
};

/**
 * Which character or organization owns a building. Exactly one of CharacterId/OrganizationId
 * is meaningful, selected by OwnerType — use MakeCharacterOwner/MakeOrganizationOwner rather
 * than setting fields by hand so an inconsistent combination can't be constructed.
 */
USTRUCT(BlueprintType)
struct BUILDINGS_API FBuildingOwner
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBuildingOwnerType OwnerType = EBuildingOwnerType::BOT_Character;

	/** Valid iff OwnerType == BOT_Character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterId CharacterId;

	/** Valid iff OwnerType == BOT_Organization. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOrganizationId OrganizationId;

	static FBuildingOwner MakeCharacterOwner(const FCharacterId InCharacterId)
	{
		FBuildingOwner Owner;
		Owner.OwnerType = EBuildingOwnerType::BOT_Character;
		Owner.CharacterId = InCharacterId;
		return Owner;
	}

	static FBuildingOwner MakeOrganizationOwner(const FOrganizationId InOrganizationId)
	{
		FBuildingOwner Owner;
		Owner.OwnerType = EBuildingOwnerType::BOT_Organization;
		Owner.OrganizationId = InOrganizationId;
		return Owner;
	}
};
