// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OrganizationInfo.generated.h"

/**
 * The UI-facing half of an organization: name and logo.
 *
 * Deliberately kept out of Mass — FText is refcounted and FSlateBrush holds a
 * TObjectPtr that Mass would not enter into the GC reference graph. Lives in the
 * UOrganizations facade, indexed by the same int32 the entities carry.
 */
USTRUCT(BlueprintType)
struct ORGANIZATIONS_API FOrganizationInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText OrganizationName = FText::FromString("Unnamed Organization");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Logo = FSlateNoResource();
};

/** Blueprint-facing view of FOrganizationAlignmentFragment plus the published tag. */
USTRUCT(BlueprintType)
struct ORGANIZATIONS_API FOrganizationAlignment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPublished = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Branches = {};
};

/**
 * Kept only so existing Blueprint pins keep resolving; the nested-array workaround it
 * existed for is gone now that settled nations live in a fragment.
 */
USTRUCT(BlueprintType)
struct ORGANIZATIONS_API FSettledNations
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Value = {};
};
