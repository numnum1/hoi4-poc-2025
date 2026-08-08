#include "FoundOrganizationModal/FoundOrganizationModalWidgetViewModel.h"

#include "Banking.h"
#include "Buildings.h"
#include "Buildings/BuildingKindViewModel.h"
#include "BuildingType.h"
#include "RtsFacadeSubsystem.h"

#define LOCTEXT_NAMESPACE "FoundOrganizationModalWidgetViewModel"

void UFoundOrganizationModalWidgetViewModel::Initialize(const FCharacterId InCharacterId)
{
	CharacterId = InCharacterId;

	URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this);
	Buildings = Facades ? Facades->GetBuildingsContainer() : nullptr;

	UBanking* NewBanking = Facades ? Facades->GetBankingContainer() : nullptr;
	if (Banking != NewBanking)
	{
		if (Banking)
		{
			Banking->OnBalanceChanged.RemoveAll(this);
		}

		Banking = NewBanking;

		if (Banking)
		{
			Banking->OnBalanceChanged.AddDynamic(this, &UFoundOrganizationModalWidgetViewModel::HandleBalanceChanged);
		}
	}

	RefreshHeadquartersTypes();
}

void UFoundOrganizationModalWidgetViewModel::SetSelectedProvinceId(const FProvinceId InSelectedProvinceId)
{
	SelectedProvinceId = InSelectedProvinceId;
	Revalidate();
}

void UFoundOrganizationModalWidgetViewModel::SetSelectedProvinceName(const FText& InSelectedProvinceName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceName, InSelectedProvinceName);
}

void UFoundOrganizationModalWidgetViewModel::SetOrganizationName(const FText& InName)
{
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, InName);
	Revalidate();
}

void UFoundOrganizationModalWidgetViewModel::ResetForm()
{
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, FText::GetEmpty());
	ApplyHeadquartersType(nullptr);
}

void UFoundOrganizationModalWidgetViewModel::ApplyHeadquartersType(UBuildingType* InBuildingType)
{
	PendingHeadquartersType = InBuildingType;
	RefreshHeadquartersTypes();
}

void UFoundOrganizationModalWidgetViewModel::RefreshHeadquartersTypes()
{
	const int32 Funds = GetCharacterFunds();
	const TArray<UBuildingType*> Catalogue = Buildings ? Buildings->GetHeadquarterBuildingTypes() : TArray<UBuildingType*>();

	TArray<TObjectPtr<UBuildingKindViewModel>> NewTypes;
	UBuildingKindViewModel* NewSelectedType = nullptr;

	for (UBuildingType* BuildingType : Catalogue)
	{
		TObjectPtr<UBuildingKindViewModel>& Entry = HeadquartersTypeCache.FindOrAdd(BuildingType);
		if (Entry == nullptr)
		{
			Entry = NewObject<UBuildingKindViewModel>(this);
			Entry->OnClicked.AddDynamic(this, &UFoundOrganizationModalWidgetViewModel::HandleHeadquartersTypeClicked);
		}

		Entry->SetSource(BuildingType);
		Entry->SetAffordable(Funds >= (BuildingType ? BuildingType->GetBuildCost() : 0));

		const bool bIsPicked = BuildingType == PendingHeadquartersType;
		Entry->SetSelected(bIsPicked);
		if (bIsPicked)
		{
			NewSelectedType = Entry;
		}

		NewTypes.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(HeadquartersTypes, NewTypes);
	UE_MVVM_SET_PROPERTY_VALUE(HeadquartersTypeCount, NewTypes.Num());
	UE_MVVM_SET_PROPERTY_VALUE(SelectedHeadquartersType, NewSelectedType);
	UE_MVVM_SET_PROPERTY_VALUE(PendingCost, PendingHeadquartersType ? PendingHeadquartersType->GetBuildCost() : 0);

	Revalidate();
}

void UFoundOrganizationModalWidgetViewModel::Revalidate()
{
	// Checked in the order the modal fills them in, same as URtsActions::GetInvalidInputReason for
	// EA_FoundOrganization — this is UX feedback for that same rule, not a second authority.
	FText Reason;
	if (OrganizationName.IsEmptyOrWhitespace())
	{
		Reason = LOCTEXT("EmptyOrganizationName", "Enter a name for the organization.");
	}
	else if (PendingHeadquartersType == nullptr || !PendingHeadquartersType->IsHeadquarters())
	{
		Reason = LOCTEXT("NotAHeadquarters", "Pick a headquarters to found the organization in.");
	}
	else
	{
		EBuildingFunction UnusedFunction;
		if (!PendingHeadquartersType->GetPrimaryFunction(UnusedFunction))
		{
			Reason = LOCTEXT("NoBuildingFunction", "This building offers no function to seat a department in.");
		}
		else if (!SelectedProvinceId.IsValid())
		{
			Reason = LOCTEXT("NoProvince", "Click a province on the map to place the headquarters.");
		}
		else if (GetCharacterFunds() < PendingHeadquartersType->GetBuildCost())
		{
			Reason = FText::Format(LOCTEXT("CannotAffordHeadquarters", "You cannot afford this: it costs {0}."),
				FText::AsNumber(PendingHeadquartersType->GetBuildCost()));
		}
	}

	UE_MVVM_SET_PROPERTY_VALUE(ModalErrorText, Reason);
	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Reason.IsEmpty());
}

int32 UFoundOrganizationModalWidgetViewModel::GetCharacterFunds() const
{
	if (Banking == nullptr)
	{
		return 0;
	}

	int32 Total = 0;
	for (const FBankAccountId AccountId : Banking->GetAccountsForCharacter(CharacterId))
	{
		Total += Banking->GetBalance(AccountId);
	}
	return Total;
}

void UFoundOrganizationModalWidgetViewModel::HandleHeadquartersTypeClicked(UBuildingKindViewModel* ClickedType)
{
	if (ClickedType == nullptr)
	{
		return;
	}

	// Clicking the picked row unpicks it, same toggle shape UPlayerCharacterViewModel's other pickers
	// use.
	UBuildingType* ClickedBuildingType = ClickedType->GetBuildingType();
	ApplyHeadquartersType(ClickedBuildingType == PendingHeadquartersType ? nullptr : ClickedBuildingType);
}

void UFoundOrganizationModalWidgetViewModel::HandleBalanceChanged(const FBankAccountId InAccountId)
{
	// Unfiltered, same tolerance UPlayerCharacterViewModel::HandleBalanceChanged has for its own
	// unfiltered facade event — cheap to re-derive, and affordability has to stay honest while the
	// modal is open.
	RefreshHeadquartersTypes();
}

void UFoundOrganizationModalWidgetViewModel::CallOnConfirmClicked()
{
	Revalidate();
	if (!bIsValid)
	{
		return;
	}

	OnConfirmClicked.Broadcast(OrganizationName, PendingHeadquartersType, SelectedProvinceId);
}

void UFoundOrganizationModalWidgetViewModel::CallOnCancelClicked()
{
	OnCancelClicked.Broadcast();
}

void UFoundOrganizationModalWidgetViewModel::BeginDestroy()
{
	if (Banking)
	{
		Banking->OnBalanceChanged.RemoveAll(this);
	}

	Super::BeginDestroy();
}

#undef LOCTEXT_NAMESPACE
