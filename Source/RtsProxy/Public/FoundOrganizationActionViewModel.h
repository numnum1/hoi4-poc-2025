#pragma once

#include "CoreMinimal.h"
#include "ActionViewModel.h"
#include "FoundOrganizationActionViewModel.generated.h"

/**
 * Founds a new organization and hands the acting character its leadership position. Self-scoped:
 * founding requires no existing position, so there is nothing for the base class's permission
 * gate to check.
 */
UCLASS()
class RTSPROXY_API UFoundOrganizationActionViewModel : public UActionViewModel
{
	GENERATED_BODY()

protected:
	virtual ERtsActionScope GetScope() const override { return ERtsActionScope::RAS_Self; }
	virtual FText GetDisplayNameFor(const FRtsActionContext& InContext) const override;
	virtual void ExecuteAction(const FRtsActionContext& InContext) const override;
};
