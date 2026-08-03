#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RtsIds.h"
#include "RtsIdLibrary.generated.h"

/**
 * Blueprint access to the validity check of the typed ids in RtsIds.h. The structs' own IsValid()
 * is plain C++ and therefore invisible to Blueprint, so every id type gets one pure node here.
 * One function per type instead of an int32 overload: passing the id itself keeps the type safety
 * the ids exist for, and UFUNCTIONs cannot be overloaded on argument type.
 */
UCLASS()
class RTSCORE_API URtsIdLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "RTS|Ids", meta = (DisplayName = "Is Valid (Character Id)", CompactNodeTitle = "IsValid"))
	static bool IsValidCharacterId(const FCharacterId& InId);

	UFUNCTION(BlueprintPure, Category = "RTS|Ids", meta = (DisplayName = "Is Valid (Province Id)", CompactNodeTitle = "IsValid"))
	static bool IsValidProvinceId(const FProvinceId& InId);

	UFUNCTION(BlueprintPure, Category = "RTS|Ids", meta = (DisplayName = "Is Valid (Nation Id)", CompactNodeTitle = "IsValid"))
	static bool IsValidNationId(const FNationId& InId);

	UFUNCTION(BlueprintPure, Category = "RTS|Ids", meta = (DisplayName = "Is Valid (Organization Id)", CompactNodeTitle = "IsValid"))
	static bool IsValidOrganizationId(const FOrganizationId& InId);

	UFUNCTION(BlueprintPure, Category = "RTS|Ids", meta = (DisplayName = "Is Valid (Building Id)", CompactNodeTitle = "IsValid"))
	static bool IsValidBuildingId(const FBuildingId& InId);

	UFUNCTION(BlueprintPure, Category = "RTS|Ids", meta = (DisplayName = "Is Valid (Race Id)", CompactNodeTitle = "IsValid"))
	static bool IsValidRaceId(const FRaceId& InId);

	UFUNCTION(BlueprintPure, Category = "RTS|Ids", meta = (DisplayName = "Is Valid (Bank Id)", CompactNodeTitle = "IsValid"))
	static bool IsValidBankId(const FBankId& InId);

	UFUNCTION(BlueprintPure, Category = "RTS|Ids", meta = (DisplayName = "Is Valid (Bank Account Id)", CompactNodeTitle = "IsValid"))
	static bool IsValidBankAccountId(const FBankAccountId& InId);

	UFUNCTION(BlueprintPure, Category = "RTS|Ids", meta = (DisplayName = "Is Valid (Schedule Id)", CompactNodeTitle = "IsValid"))
	static bool IsValidScheduleId(const FScheduleId& InId);

	UFUNCTION(BlueprintPure, Category = "RTS|Ids", meta = (DisplayName = "Is Valid (Role Id)", CompactNodeTitle = "IsValid"))
	static bool IsValidRoleId(const FRoleId& InId);
};
