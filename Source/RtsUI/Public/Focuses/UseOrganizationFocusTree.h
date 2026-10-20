#pragma once

#include "CoreMinimal.h"
#include "Focuses/FocusTreeEdge.h"
#include "Focuses/FocusTreeExclusiveLink.h"
#include "Focuses/RtsFocusContext.h"
#include "MVVMViewModelBase.h"
#include "RtsIds.h"
#include "Styling/SlateBrush.h"
#include "UseOrganizationFocusTree.generated.h"

class UBanking;
class UDepartments;
class UFocusEventOptionViewModel;
class UFocusNodeViewModel;
class UFocusTreeTabViewModel;
class UOrganizationFocusAsset;
class UOrganizationFocuses;
class UOrganizationFocusTreeAsset;
class UOrganizations;
class UTime;

UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UUseOrganizationFocusTree : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void Initialize();

	void SetOrganizationId(FOrganizationId InOrganizationId);

	UFUNCTION(BlueprintCallable, Category="Use Organization Focus Tree")
	void SelectTree(int32 InTreeIndex);

	const TArray<TObjectPtr<UFocusNodeViewModel>>& GetNodes() const { return Nodes; }
	const TArray<FFocusTreeEdge>& GetEdges() const { return Edges; }
	const TArray<FFocusTreeExclusiveLink>& GetExclusiveLinks() const { return ExclusiveLinks; }
	const TArray<TObjectPtr<UFocusEventOptionViewModel>>& GetEventOptions() const { return EventOptions; }

protected:
	virtual void BeginDestroy() override;

private:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use Organization Focus Tree", meta=(AllowPrivateAccess))
	bool bHasOrganization = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use Organization Focus Tree", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UFocusTreeTabViewModel>> Tabs;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use Organization Focus Tree", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UFocusNodeViewModel>> Nodes;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use Organization Focus Tree|Event", meta=(AllowPrivateAccess))
	bool bHasPendingEvent = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use Organization Focus Tree|Event", meta=(AllowPrivateAccess))
	FText EventTitle;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use Organization Focus Tree|Event", meta=(AllowPrivateAccess))
	FText EventDescription;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use Organization Focus Tree|Event", meta=(AllowPrivateAccess))
	FSlateBrush EventImage;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use Organization Focus Tree|Event", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UFocusEventOptionViewModel>> EventOptions;

	UFUNCTION()
	void HandleFocusChanged(FOrganizationId InOrganizationId);

	UFUNCTION()
	void HandlePendingEventChanged(FOrganizationId InOrganizationId);

	UFUNCTION()
	void HandleHourElapsed(int32 InHoursElapsed);

	UFUNCTION()
	void HandleBalanceChanged(FBankAccountId InAccountId);

	void BindFacades();
	void UnbindFacades();

	FRtsFocusContext MakeContext() const;

	void RefreshTree();
	void RefreshNodeStatus();
	void RefreshEvent();

	UPROPERTY(Transient)
	TObjectPtr<UOrganizationFocuses> OrganizationFocuses;

	UPROPERTY(Transient)
	TObjectPtr<UOrganizations> Organizations;

	UPROPERTY(Transient)
	TObjectPtr<UDepartments> Departments;

	UPROPERTY(Transient)
	TObjectPtr<UBanking> Banking;

	UPROPERTY(Transient)
	TObjectPtr<UTime> Time;

	UPROPERTY()
	TObjectPtr<UOrganizationFocusTreeAsset> ActiveTree;

	UPROPERTY()
	TArray<TObjectPtr<UFocusTreeTabViewModel>> TabCache;

	UPROPERTY()
	TMap<TObjectPtr<UOrganizationFocusAsset>, TObjectPtr<UFocusNodeViewModel>> NodeCache;

	UPROPERTY()
	TArray<TObjectPtr<UFocusEventOptionViewModel>> EventOptionCache;

	FOrganizationId OrganizationId;

	TArray<FFocusTreeEdge> Edges;

	TArray<FFocusTreeExclusiveLink> ExclusiveLinks;
};
