#pragma once

#include "CoreMinimal.h"
#include "OrganizationInfo.h"
#include "RtsEntityProxy.h"
#include "RtsIds.h"
#include "OrganizationProxy.generated.h"

class UBuildingProxy;
class UCharacterProxy;
class UNationProxy;
class UOrganizations;

/**
 * One organization, as an object you can navigate from instead of an id you have to carry around.
 *
 * Everything is read through UOrganizations on each call. Get one from
 * URtsProxySubsystem::GetOrganization, or from any other proxy that leads here. See
 * URtsEntityProxy for the rules all proxies follow.
 */
UCLASS(BlueprintType)
class RTSPROXY_API UOrganizationProxy : public URtsEntityProxy
{
	GENERATED_BODY()

public:
	/** Called by URtsProxySubsystem right after construction. Not for general use. */
	void Setup(URtsProxySubsystem* InProxies, FOrganizationId InOrganizationId, UOrganizations* InOrganizations);

	UFUNCTION(BlueprintPure, Category="Organization")
	FOrganizationId GetId() const { return OrganizationId; }

	virtual bool IsAlive() const override;

	//~ Display ----------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Organization")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintPure, Category="Organization")
	FSlateBrush GetLogo() const;

	//~ Alignment --------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Organization|Alignment")
	FOrganizationAlignment GetAlignment() const;

	UFUNCTION(BlueprintPure, Category="Organization|Alignment")
	bool IsAlignmentPublished() const;

	UFUNCTION(BlueprintPure, Category="Organization|Alignment")
	TArray<float> GetAlignmentBranches() const;

	/** One branch of the published alignment, or 0 if the index is out of range. */
	UFUNCTION(BlueprintPure, Category="Organization|Alignment")
	float GetAlignmentBranch(int32 InIndex) const;

	//~ Navigation -------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Organization")
	TArray<UNationProxy*> GetSettledNations() const;

	/** Everyone holding a position here. Walks all characters — fine for a UI panel. */
	UFUNCTION(BlueprintPure, Category="Organization")
	TArray<UCharacterProxy*> GetMembers() const;

	/** Every building owned by this organization. Walks all buildings. */
	UFUNCTION(BlueprintPure, Category="Organization")
	TArray<UBuildingProxy*> GetOwnedBuildings() const;

	//~ Actions ----------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category="Organization")
	void PublishAlignment(const TArray<float>& InAlignment);

	UFUNCTION(BlueprintCallable, Category="Organization")
	void SettleIn(const UNationProxy* InNation);

	/** Destroys the entity. The name and logo stay behind, and so does this proxy. */
	UFUNCTION(BlueprintCallable, Category="Organization")
	void Dissolve();

	/** The facade underneath, for the few calls no proxy covers. */
	UOrganizations* GetContainer() const { return Organizations; }

protected:
	virtual void BindEvents() override;
	virtual void UnbindEvents() override;

private:
	UFUNCTION()
	void HandleOrganizationEvent(FOrganizationId InOrganizationId);

	FOrganizationId OrganizationId;

	UPROPERTY()
	TObjectPtr<UOrganizations> Organizations;
};
