#include "MapModes/RtsMapModes.h"

#include "Brushes/SlateNoResource.h"
#include "Buildings.h"
#include "Departments.h"
#include "Nations.h"
#include "Organizations.h"
#include "Populations.h"
#include "Provinces.h"

#define LOCTEXT_NAMESPACE "RtsMapModes"

namespace
{
	/** What a province shows when the active mode has nothing to say about it. Not black: an unlit
	 * province would read as a hole in the world rather than as "nothing here". */
	const FColor NeutralProvinceColor = FColor(90, 90, 90);

	/** Every mode returns a color for every living province, never a partial change set. The map
	 * actor writes what it is given without clearing, so a mode that skipped provinces would leave
	 * the previous mode's colors showing through wherever the two disagree. */
	TMap<FProvinceId, FProvinceVisuals> MakeNeutralBase(const UProvinces& InProvinces)
	{
		TMap<FProvinceId, FProvinceVisuals> Visuals;
		Visuals.Reserve(InProvinces.Num());

		for (int32 Index = 0; Index < InProvinces.Num(); ++Index)
		{
			const FProvinceId ProvinceId(Index);
			if (!InProvinces.IsAlive(ProvinceId))
			{
				continue;
			}

			FProvinceVisuals& Visual = Visuals.Add(ProvinceId);
			Visual.PrimaryColor = NeutralProvinceColor;
		}

		return Visuals;
	}

	TMap<FProvinceId, FProvinceVisuals> BuildPoliticalVisuals(const FRtsMapModeContext& InContext)
	{
		if (InContext.Provinces == nullptr || InContext.Nations == nullptr)
		{
			return TMap<FProvinceId, FProvinceVisuals>();
		}

		TMap<FProvinceId, FProvinceVisuals> Visuals = MakeNeutralBase(*InContext.Provinces);

		for (TPair<FProvinceId, FProvinceVisuals>& Entry : Visuals)
		{
			const FNationId OwningNationId = InContext.Provinces->GetOwningNation(Entry.Key);
			if (OwningNationId.IsValid())
			{
				Entry.Value.PrimaryColor = InContext.Nations->GetNationInfo(OwningNationId).MapColor;
			}
		}

		return Visuals;
	}

	TMap<FProvinceId, FProvinceVisuals> BuildOrganizationVisuals(const FRtsMapModeContext& InContext)
	{
		if (InContext.Provinces == nullptr || InContext.Organizations == nullptr
			|| InContext.Departments == nullptr || InContext.Buildings == nullptr)
		{
			return TMap<FProvinceId, FProvinceVisuals>();
		}

		TMap<FProvinceId, FProvinceVisuals> Visuals = MakeNeutralBase(*InContext.Provinces);

		for (int32 Index = 0; Index < InContext.Organizations->Num(); ++Index)
		{
			const FOrganizationId OrganizationId(Index);
			if (!InContext.Organizations->IsAlive(OrganizationId))
			{
				continue;
			}

			const FColor OrganizationColor = URtsMapModes::GetOrganizationColor(OrganizationId);

			// Two organizations settled in the same nation paint the same provinces; the later id
			// wins. Showing one of them beats blending into a color that names neither.
			for (const FNationId SettledNationId : InContext.Organizations->GetSettledNations(OrganizationId))
			{
				for (const FProvinceId ProvinceId : InContext.Provinces->CollectProvincesOfNation(SettledNationId))
				{
					if (FProvinceVisuals* Visual = Visuals.Find(ProvinceId))
					{
						Visual->PrimaryColor = OrganizationColor;
					}
				}
			}

			const FDepartmentId RootDepartmentId = InContext.Organizations->GetRootDepartment(OrganizationId);
			if (!RootDepartmentId.IsValid())
			{
				continue;
			}

			const FBuildingId SeatId = InContext.Departments->GetSeat(RootDepartmentId);
			if (!SeatId.IsValid())
			{
				continue;
			}

			// The headquarters province is picked out on the national channel even when the
			// organization has not settled the nation it sits in — otherwise a freshly founded
			// organization would be invisible.
			if (FProvinceVisuals* Headquarters = Visuals.Find(InContext.Buildings->GetProvince(SeatId)))
			{
				Headquarters->PrimaryColor = OrganizationColor;
				Headquarters->bUseNationalColor = true;
				Headquarters->NationalColor = OrganizationColor;
			}
		}

		return Visuals;
	}

	TMap<FProvinceId, FProvinceVisuals> BuildEthnicityVisuals(const FRtsMapModeContext& InContext)
	{
		if (InContext.Provinces == nullptr || InContext.Populations == nullptr)
		{
			return TMap<FProvinceId, FProvinceVisuals>();
		}

		TMap<FProvinceId, FProvinceVisuals> Visuals = MakeNeutralBase(*InContext.Provinces);

		// One pass over every pop rather than UPopulations::GetDominantEthnicity per province: that
		// getter rescans all pops each call, which would make this quadratic across the whole world.
		struct FDominant
		{
			FEthnicityId EthnicityId;
			int32 Size = 0;
		};
		TMap<FProvinceId, FDominant> DominantByProvince;

		for (const FPopInfo& Pop : InContext.Populations->CollectPops())
		{
			if (Pop.Size <= 0 || !Visuals.Contains(Pop.ProvinceId))
			{
				continue;
			}

			FDominant& Dominant = DominantByProvince.FindOrAdd(Pop.ProvinceId);

			// Ties go to the lower id so the map cannot flicker between two equal groups depending on
			// what order the entity chunks happened to come back in.
			const bool bWins = Pop.Size > Dominant.Size
				|| (Pop.Size == Dominant.Size && Pop.EthnicityId.Value < Dominant.EthnicityId.Value);
			if (bWins)
			{
				Dominant.EthnicityId = Pop.EthnicityId;
				Dominant.Size = Pop.Size;
			}
		}

		// A province nobody lives in keeps the neutral base — that is the honest answer here, unlike
		// in the political mode where unowned still means unowned by someone.
		for (const TPair<FProvinceId, FDominant>& Entry : DominantByProvince)
		{
			Visuals[Entry.Key].PrimaryColor =
				InContext.Populations->GetEthnicityInfo(Entry.Value.EthnicityId).MapColor;
		}

		return Visuals;
	}
}

TArray<EMapMode> URtsMapModes::GetMapModes()
{
	return { EMapMode::EMM_Political, EMapMode::EMM_Organizations, EMapMode::EMM_Ethnicities };
}

FText URtsMapModes::GetDisplayName(const EMapMode InMapMode)
{
	switch (InMapMode)
	{
	case EMapMode::EMM_Political:
		return LOCTEXT("PoliticalMapMode", "Political");
	case EMapMode::EMM_Organizations:
		return LOCTEXT("OrganizationsMapMode", "Organizations");
	case EMapMode::EMM_Ethnicities:
		return LOCTEXT("EthnicitiesMapMode", "Ethnicities");
	}

	return FText::GetEmpty();
}

FSlateBrush URtsMapModes::GetIcon(const EMapMode InMapMode)
{
	switch (InMapMode)
	{
	case EMapMode::EMM_Political:
	case EMapMode::EMM_Organizations:
	case EMapMode::EMM_Ethnicities:
		return FSlateNoResource();
	}

	return FSlateNoResource();
}

TMap<FProvinceId, FProvinceVisuals> URtsMapModes::BuildVisuals(const EMapMode InMapMode, const FRtsMapModeContext& InContext)
{
	switch (InMapMode)
	{
	case EMapMode::EMM_Political:
		return BuildPoliticalVisuals(InContext);
	case EMapMode::EMM_Organizations:
		return BuildOrganizationVisuals(InContext);
	case EMapMode::EMM_Ethnicities:
		return BuildEthnicityVisuals(InContext);
	}

	return TMap<FProvinceId, FProvinceVisuals>();
}

FColor URtsMapModes::GetOrganizationColor(const FOrganizationId InOrganizationId)
{
	if (!InOrganizationId.IsValid())
	{
		return NeutralProvinceColor;
	}

	return FLinearColor::IntToDistinctColor(InOrganizationId.Value, /*Saturation=*/0.65f, /*Value=*/0.9f).ToFColor(true);
}

#undef LOCTEXT_NAMESPACE
