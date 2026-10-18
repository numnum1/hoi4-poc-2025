#pragma once

#include "CoreMinimal.h"
#include "RtsFocusContext.generated.h"

class UBanking;
class UDepartments;
class UOrganizationFocuses;
class UOrganizations;

USTRUCT(BlueprintType)
struct RTSUI_API FRtsFocusContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Focus")
	TObjectPtr<UOrganizations> Organizations;

	UPROPERTY(BlueprintReadOnly, Category="Focus")
	TObjectPtr<UDepartments> Departments;

	UPROPERTY(BlueprintReadOnly, Category="Focus")
	TObjectPtr<UBanking> Banking;

	UPROPERTY(BlueprintReadOnly, Category="Focus")
	TObjectPtr<UOrganizationFocuses> OrganizationFocuses;

	UPROPERTY(BlueprintReadOnly, Category="Focus")
	FDateTime Now;
};
