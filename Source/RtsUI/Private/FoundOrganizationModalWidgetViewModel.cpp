#include "FoundOrganizationModalWidgetViewModel.h"

void UFoundOrganizationModalWidgetViewModel::SetSource(const FText& InOrganizationName,
                                                        const TArray<TObjectPtr<UBuildingKindViewModel>>& InHeadquartersTypes,
                                                        UBuildingKindViewModel* InSelectedHeadquartersType,
                                                        const FText& InSelectedProvinceName,
                                                        const int32 InPendingCost,
                                                        const FText& InModalErrorText,
                                                        const bool bInCanConfirm)
{
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, InOrganizationName);
	UE_MVVM_SET_PROPERTY_VALUE(HeadquartersTypes, InHeadquartersTypes);
	UE_MVVM_SET_PROPERTY_VALUE(HeadquartersTypeCount, InHeadquartersTypes.Num());
	UE_MVVM_SET_PROPERTY_VALUE(SelectedHeadquartersType, InSelectedHeadquartersType);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceName, InSelectedProvinceName);
	UE_MVVM_SET_PROPERTY_VALUE(PendingCost, InPendingCost);
	UE_MVVM_SET_PROPERTY_VALUE(ModalErrorText, InModalErrorText);
	UE_MVVM_SET_PROPERTY_VALUE(bCanConfirm, bInCanConfirm);
}

void UFoundOrganizationModalWidgetViewModel::SetOrganizationName(const FText& InName)
{
	// Deliberately does not touch the local OrganizationName property — see the class comment.
	// The parent re-validates and the authoritative value returns through the next SetSource.
	OnOrganizationNameChanged.Broadcast(InName);
}

void UFoundOrganizationModalWidgetViewModel::CallOnConfirmClicked()
{
	OnConfirmClicked.Broadcast();
}

void UFoundOrganizationModalWidgetViewModel::CallOnCancelClicked()
{
	OnCancelClicked.Broadcast();
}
