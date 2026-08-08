#pragma once

#include "CoreMinimal.h"
#include "RtsIds.generated.h"

/**
 * Typed entity identifiers shared across every domain module (Characters, Provinces, Nations,
 * Organizations, Buildings, Races). Each wraps a single int32 so Blueprint pins carry the type
 * name instead of looking like an interchangeable integer, and the compiler catches passing a
 * FProvinceId where a FCharacterId was expected.
 *
 * A value is a stable identity, not an array position: once assigned it is never reused, even
 * after the entity it names is destroyed. See RtsCore's role in the entity lifecycle model.
 */

USTRUCT(BlueprintType)
struct RTSCORE_API FCharacterId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FCharacterId() = default;
	explicit FCharacterId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FCharacterId& Other) const { return Value == Other.Value; }
	bool operator!=(const FCharacterId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FCharacterId& Id) { return ::GetTypeHash(Id.Value); }
};

USTRUCT(BlueprintType)
struct RTSCORE_API FProvinceId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FProvinceId() = default;
	explicit FProvinceId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FProvinceId& Other) const { return Value == Other.Value; }
	bool operator!=(const FProvinceId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FProvinceId& Id) { return ::GetTypeHash(Id.Value); }
};

USTRUCT(BlueprintType)
struct RTSCORE_API FNationId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FNationId() = default;
	explicit FNationId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FNationId& Other) const { return Value == Other.Value; }
	bool operator!=(const FNationId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FNationId& Id) { return ::GetTypeHash(Id.Value); }
};

USTRUCT(BlueprintType)
struct RTSCORE_API FOrganizationId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FOrganizationId() = default;
	explicit FOrganizationId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FOrganizationId& Other) const { return Value == Other.Value; }
	bool operator!=(const FOrganizationId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FOrganizationId& Id) { return ::GetTypeHash(Id.Value); }
};

USTRUCT(BlueprintType)
struct RTSCORE_API FBuildingId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FBuildingId() = default;
	explicit FBuildingId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FBuildingId& Other) const { return Value == Other.Value; }
	bool operator!=(const FBuildingId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FBuildingId& Id) { return ::GetTypeHash(Id.Value); }
};

/**
 * Names one department — a node inside an organization, seated in a building. An organization
 * points at its root department, a department at its parent; both directions are ids rather than
 * FMassEntityHandles for the usual reason (handles do not survive a session).
 */
USTRUCT(BlueprintType)
struct RTSCORE_API FDepartmentId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FDepartmentId() = default;
	explicit FDepartmentId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FDepartmentId& Other) const { return Value == Other.Value; }
	bool operator!=(const FDepartmentId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FDepartmentId& Id) { return ::GetTypeHash(Id.Value); }
};

USTRUCT(BlueprintType)
struct RTSCORE_API FRaceId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FRaceId() = default;
	explicit FRaceId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FRaceId& Other) const { return Value == Other.Value; }
	bool operator!=(const FRaceId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FRaceId& Id) { return ::GetTypeHash(Id.Value); }
};

USTRUCT(BlueprintType)
struct RTSCORE_API FBankId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FBankId() = default;
	explicit FBankId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FBankId& Other) const { return Value == Other.Value; }
	bool operator!=(const FBankId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FBankId& Id) { return ::GetTypeHash(Id.Value); }
};

USTRUCT(BlueprintType)
struct RTSCORE_API FBankAccountId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FBankAccountId() = default;
	explicit FBankAccountId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FBankAccountId& Other) const { return Value == Other.Value; }
	bool operator!=(const FBankAccountId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FBankAccountId& Id) { return ::GetTypeHash(Id.Value); }
};

/**
 * Names an entry in USchedules' library, not a per-character schedule. Like FRaceId it points
 * into authored template data rather than at an entity: many characters follow the same
 * FScheduleId, and the schedule they actually run is a copy carried in their shared fragment.
 */
USTRUCT(BlueprintType)
struct RTSCORE_API FScheduleId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FScheduleId() = default;
	explicit FScheduleId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FScheduleId& Other) const { return Value == Other.Value; }
	bool operator!=(const FScheduleId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FScheduleId& Id) { return ::GetTypeHash(Id.Value); }
};

/**
 * Names an entry in URoles' library, not a per-character position. Like FScheduleId it points into
 * authored template data rather than at an entity: many characters across many organizations can
 * hold the same FRoleId, and what it grants (permissions, leadership) lives in the library entry.
 */
USTRUCT(BlueprintType)
struct RTSCORE_API FRoleId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = INDEX_NONE;

	FRoleId() = default;
	explicit FRoleId(const int32 InValue) : Value(InValue) {}

	bool IsValid() const { return Value != INDEX_NONE; }
	bool operator==(const FRoleId& Other) const { return Value == Other.Value; }
	bool operator!=(const FRoleId& Other) const { return !(*this == Other); }
	friend uint32 GetTypeHash(const FRoleId& Id) { return ::GetTypeHash(Id.Value); }
};
