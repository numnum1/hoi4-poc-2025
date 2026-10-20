#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityElementTypes.h"
#include "Mass/ExternalSubsystemTraits.h"
#include "UObject/TopLevelAssetPath.h"
#include "OrganizationFocusFragments.generated.h"

USTRUCT()
struct FOCUSES_API FOrganizationFocusOwnerFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FOrganizationId OrganizationId;
};

USTRUCT()
struct FOCUSES_API FOrganizationFocusRunFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FTopLevelAssetPath CurrentFocus;

	UPROPERTY()
	FDateTime StartedAt;

	UPROPERTY()
	FTopLevelAssetPath PendingEventFocus;
};

USTRUCT()
struct FOCUSES_API FOrganizationFocusHistoryFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FTopLevelAssetPath> CompletedFocuses;
};

template<>
struct TMassFragmentTraits<FOrganizationFocusHistoryFragment> final
{
	enum
	{
		AuthorAcceptsItsNotTriviallyCopyable = true
	};
};

USTRUCT()
struct FOCUSES_API FOrganizationFocusTag : public FMassTag
{
	GENERATED_BODY()
};
