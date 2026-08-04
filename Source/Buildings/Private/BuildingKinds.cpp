#include "BuildingKinds.h"

#define LOCTEXT_NAMESPACE "BuildingKinds"

TArray<EBuildingKind> UBuildingKinds::GetHeadquarterKinds()
{
	return { EBuildingKind::BK_PartyOffice };
}

bool UBuildingKinds::IsHeadquarters(const EBuildingKind InKind)
{
	switch (InKind)
	{
	case EBuildingKind::BK_None:
		return false;
	case EBuildingKind::BK_PartyOffice:
		return true;
	}
	return false;
}

FText UBuildingKinds::GetDisplayName(const EBuildingKind InKind)
{
	switch (InKind)
	{
	case EBuildingKind::BK_None:
		return FText::GetEmpty();
	case EBuildingKind::BK_PartyOffice:
		return LOCTEXT("PartyOffice", "Party Office");
	}
	return FText::GetEmpty();
}

FSlateBrush UBuildingKinds::GetIcon(const EBuildingKind InKind)
{
	// No art authored yet — URtsActions::GetIcon is in the same state. The switch exists so adding
	// one later is a one-line change in the place every other per-kind answer already lives.
	switch (InKind)
	{
	case EBuildingKind::BK_None:
	case EBuildingKind::BK_PartyOffice:
		return FSlateNoResource();
	}
	return FSlateNoResource();
}

int32 UBuildingKinds::GetBuildCost(const EBuildingKind InKind)
{
	// First-pass numbers, meant to be read against the balances UCharacterAsset authors.
	switch (InKind)
	{
	case EBuildingKind::BK_None:
		return 0;
	case EBuildingKind::BK_PartyOffice:
		return 500;
	}
	return 0;
}

#undef LOCTEXT_NAMESPACE
