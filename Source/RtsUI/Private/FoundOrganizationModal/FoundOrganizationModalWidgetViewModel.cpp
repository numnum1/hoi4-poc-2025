#include "FoundOrganizationModal/FoundOrganizationModalWidgetViewModel.h"

#include "Actions/RtsActions.h"
#include "Banking.h"
#include "Buildings.h"
#include "Buildings/BuildingKindViewModel.h"
#include "BuildingType.h"
#include "Characters.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"
#include "RtsWorldMap.h"
#include "WorldMapSubsystem.h"

#define LOCTEXT_NAMESPACE "FoundOrganizationModalWidgetViewModel"

void UFoundOrganizationModalWidgetViewModel::Initialize(const FCharacterId InCharacterId)
{
	CharacterId = InCharacterId;

	Buildings = FindRtsGameStateComponent<UBuildings>(this);
	Provinces = FindRtsGameStateComponent<UProvinces>(this);
	Characters = FindRtsGameStateComponent<UCharacters>(this);

	UBanking* NewBanking = FindRtsGameStateComponent<UBanking>(this);
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

	// A valid click is the player picking a site off the overlay — its job is done. An invalid one
	// (clicking off any province) leaves it up so they can keep looking.
	if (InSelectedProvinceId.IsValid())
	{
		ClearProvinceOverlay();
	}

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

	// Picking a type is what the overlay is for — unpicking it (toggling the row off) has nothing
	// left to preview a site for.
	if (PendingHeadquartersType)
	{
		RefreshProvinceOverlay();
	}
	else
	{
		ClearProvinceOverlay();
	}
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
		else if (!URtsActions::CanFoundHeadquartersIn(SelectedProvinceId, Provinces, GetActingNationId()))
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

FNationId UFoundOrganizationModalWidgetViewModel::GetActingNationId() const
{
	const FProvinceId ActorProvinceId = Characters ? Characters->GetCurrentProvince(CharacterId) : FProvinceId();
	return Provinces && ActorProvinceId.IsValid() ? Provinces->GetOwningNation(ActorProvinceId) : FNationId();
}

void UFoundOrganizationModalWidgetViewModel::RefreshProvinceOverlay() const
{
	ARtsWorldMap* WorldMap = ARtsWorldMap::GetRtsWorldMap(this);
	const UWorldMapSubsystem* WorldMapSubsystem = UWorldMapSubsystem::Get(this);
	if (WorldMap == nullptr || WorldMapSubsystem == nullptr || Provinces == nullptr)
	{
		return;
	}

	const FNationId ActingNationId = GetActingNationId();

	TMap<FProvinceId, FColor> Colors;
	for (int32 Index = 0; Index < Provinces->Num(); ++Index)
	{
		const FProvinceId CandidateId(Index);
		if (!Provinces->IsAlive(CandidateId))
		{
			continue;
		}

		const FNationId OwningNation = Provinces->GetOwningNation(CandidateId);
		if (OwningNation.IsValid() && OwningNation != ActingNationId)
		{
			Colors.Add(CandidateId, FColor(128, 128, 128));
			continue;
		}

		if (!URtsActions::CanFoundHeadquartersIn(CandidateId, Provinces, ActingNationId))
		{
			Colors.Add(CandidateId, FColor::Red);
			continue;
		}

		bool bAlreadyHasType = false;
		if (Buildings && PendingHeadquartersType)
		{
			for (const FBuildingId ExistingId : Buildings->CollectBuildingsInProvince(CandidateId))
			{
				if (Buildings->GetBuildingType(ExistingId) == PendingHeadquartersType
					&& Buildings->GetOwnerCharacter(ExistingId) == CharacterId)
				{
					bAlreadyHasType = true;
					break;
				}
			}
		}

		Colors.Add(CandidateId, bAlreadyHasType ? FColor::Blue : FColor::Green);
	}

	TMap<FProvinceId, FProvinceVisuals> Visuals;
	Visuals.Reserve(Colors.Num());
	for (const TPair<FProvinceId, FColor>& Entry : Colors)
	{
		Visuals.Add(Entry.Key).PrimaryColor = Entry.Value;
	}

	WorldMap->PaintProvinces(WorldMapSubsystem->BuildLutRows(Visuals));
}

void UFoundOrganizationModalWidgetViewModel::ClearProvinceOverlay() const
{
	// Nothing to undo here: closing the modal changes URtsHUDViewModel::ActiveModal, which
	// re-broadcasts its computed ProvinceVisuals and repaints the whole map in the active mode over
	// these colors. Painting a "cleared" state here would only fight that repaint.
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

void UFoundOrganizationModalWidgetViewModel::BeginDestroy()
{
	if (Banking)
	{
		Banking->OnBalanceChanged.RemoveAll(this);
	}

	Super::BeginDestroy();
}

#undef LOCTEXT_NAMESPACE
