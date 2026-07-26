#pragma once

#include "Algo/Transform.h"
#include "CharacterAsset.h"
#include "CharacterInfo.h"
#include "Race.h"
#include "RaceCategory.h"
#include "LifeType.h"
#include "Species.h"
#include "RaceAsset.h"
#include "NationAsset.h"
#include "OrganizationAsset.h"
#include "OrganizationInfo.h"
#include "ProvinceAsset.h"
#include "ProvinceInfo.h"
#include "RoleAsset.h"
#include "RoleInfo.h"
#include "ScheduleAsset.h"
#include "ScheduleInfo.h"

namespace ScenarioLoader
{
	__inline TTuple<TArray<FRaceCategory>, TArray<FLifeType>, TArray<FSpecies>, TArray<FRace>, TArray<URaceAsset*>>
	ExtractRaces(const TArray<UCharacterAsset*>& InCharacters)
	{
		// --- Collect unique assets from all characters ---
		TArray<URaceAsset*>        RaceAssets;
		TArray<USpeciesAsset*>     SpeciesAssets;
		TArray<ULifeTypeAsset*>    LifeTypeAssets;
		TArray<URaceCategoryAsset*> CategoryAssets;

		for (const UCharacterAsset* Char : InCharacters)
		{
			if (!Char) continue;
			URaceAsset* Race = Char->GetBaseRace();
			if (!Race || RaceAssets.Contains(Race)) continue;
			RaceAssets.Add(Race);

			USpeciesAsset* Sp = Race->GetSpecies();
			if (Sp && !SpeciesAssets.Contains(Sp))
			{
				SpeciesAssets.Add(Sp);

				if (ULifeTypeAsset* LT = Sp->GetLifeType())
					if (!LifeTypeAssets.Contains(LT))
						LifeTypeAssets.Add(LT);

				if (URaceCategoryAsset* Cat = Sp->GetCategory())
					if (!CategoryAssets.Contains(Cat))
						CategoryAssets.Add(Cat);
			}
		}

		// --- Helper: convert asset traits to runtime traits ---
		auto ConvertTraits = [](const TArray<URaceTraitAsset*>& InTraits) -> TArray<FRaceTrait>
		{
			TArray<FRaceTrait> Out;
			Algo::Transform(InTraits, Out, [](const URaceTraitAsset* T)
			{
				return FRaceTrait{ T->GetTraitName(), T->GetTraitDescription() };
			});
			return Out;
		};

		// --- Build runtime structs ---
		TArray<FRaceCategory> OutCategories;
		OutCategories.Reserve(CategoryAssets.Num());
		for (const URaceCategoryAsset* Cat : CategoryAssets)
			OutCategories.Add(FRaceCategory{ Cat->GetCategoryName() });

		TArray<FLifeType> OutLifeTypes;
		OutLifeTypes.Reserve(LifeTypeAssets.Num());
		for (const ULifeTypeAsset* LT : LifeTypeAssets)
			OutLifeTypes.Add(FLifeType{ LT->GetLifeTypeName(), ConvertTraits(LT->GetTraits()) });

		TArray<FSpecies> OutSpecies;
		OutSpecies.Reserve(SpeciesAssets.Num());
		for (const USpeciesAsset* Sp : SpeciesAssets)
			OutSpecies.Add(FSpecies{
				Sp->GetSpeciesName(),
				CategoryAssets.Find(Sp->GetCategory()),
				LifeTypeAssets.Find(Sp->GetLifeType()),
				ConvertTraits(Sp->GetTraits())
			});

		TArray<FRace> OutRaces;
		OutRaces.Reserve(RaceAssets.Num());
		for (const URaceAsset* Race : RaceAssets)
			OutRaces.Add(FRace{
				Race->GetRaceName(),
				SpeciesAssets.Find(Race->GetSpecies()),
				ConvertTraits(Race->GetTraits()),
				Race->GetStrength(),
				Race->GetDexterity(),
				Race->GetConstitution(),
				Race->GetIntelligence(),
				Race->GetWisdom(),
				Race->GetCharisma()
			});

		return { OutCategories, OutLifeTypes, OutSpecies, OutRaces, RaceAssets };
	}


	__inline TTuple<TArray<FNationInfo>, TArray<FProvinceSpawnData>> ExtractNationsAndProvinces(const TArray<UProvinceAsset*>& InProvinces)
	{
		TTuple<TArray<FNationInfo>, TArray<FProvinceSpawnData>> Res;

		TArray<UNationAsset*> NationAssets;
		NationAssets.Reserve(InProvinces.Num());

		// 1) Unique Nations
		for (const UProvinceAsset* Province : InProvinces)
		{
			if (!Province) continue;

			UNationAsset* Nation = Province->GetOwningNation();
			if (!Nation) continue;

			if (!NationAssets.Contains(Nation))
			{
				NationAssets.Add(Nation);
			}
		}

		Res.Key.Reserve(NationAssets.Num());
		for (UNationAsset* Nation : NationAssets)
		{
			Res.Key.Add(FNationInfo{
				Nation->GetNationName(),
				Nation->GetFlag()
			});
		}

		Res.Value.Reserve(InProvinces.Num());
		for (const UProvinceAsset* Province : InProvinces)
		{
			if (!Province) continue;

			UNationAsset* Nation = Province->GetOwningNation();
			const FNationId NationId(NationAssets.Find(Nation));

			FProvinceSpawnData SpawnData;
			SpawnData.Display.MapId = Province->GetMapId();
			SpawnData.Display.Name = Province->GetProvinceName();
			SpawnData.Display.Picture = Province->GetPicture();
			SpawnData.Display.InspectionSound = Province->GetInspectionSound();
			SpawnData.Display.Ecology = Province->GetEcology();
			SpawnData.TimezoneOffset = Province->GetTimezoneOffset();
			SpawnData.OwningNationId = NationId;
			Res.Value.Add(MoveTemp(SpawnData));
		}

		return Res;
	}

	/**
	 * The schedule library, discovered from the characters that follow its entries — the same
	 * way ExtractRaces discovers races. Returns the runtime infos for USchedules alongside the
	 * asset array they came from, so ExtractCharacters can resolve an asset back to its
	 * FScheduleId by position.
	 */
	__inline TTuple<TArray<FScheduleInfo>, TArray<UScheduleAsset*>>
	ExtractSchedules(const TArray<UCharacterAsset*>& InCharacters)
	{
		TArray<UScheduleAsset*> ScheduleAssets;
		ScheduleAssets.Reserve(InCharacters.Num());

		for (const UCharacterAsset* Char : InCharacters)
		{
			if (!Char) continue;

			UScheduleAsset* Schedule = Char->GetSchedule();
			if (Schedule && !ScheduleAssets.Contains(Schedule))
			{
				ScheduleAssets.Add(Schedule);
			}
		}

		TArray<FScheduleInfo> Out;
		Out.Reserve(ScheduleAssets.Num());
		for (const UScheduleAsset* Schedule : ScheduleAssets)
		{
			FScheduleInfo Info;
			Info.ScheduleName = Schedule->GetScheduleName();
			Info.Schedule = Schedule->GetSchedule();
			Out.Add(MoveTemp(Info));
		}

		return { MoveTemp(Out), MoveTemp(ScheduleAssets) };
	}

	/**
	 * Everything a character needs at creation, resolved to runtime indexes.
	 * InProvinceInfos must be the array ExtractNationsAndProvinces produced from InProvinces,
	 * so index i of both refers to the same province; it supplies the timezone offset that
	 * gets cached on the entity. InScheduleIndex is the asset array ExtractSchedules returned,
	 * so a character's schedule asset resolves to the same FScheduleId USchedules stores it under.
	 */
	__inline TArray<FCharacterSpawnData>
	ExtractCharacters(const TArray<UCharacterAsset*>& InCharacters, const TArray<UProvinceAsset*>& InProvinces,
	                  const TArray<FProvinceSpawnData>& InProvinceInfos, const TArray<URaceAsset*>& InRaceIndex,
	                  const TArray<FRace>& InRaces, const TArray<UScheduleAsset*>& InScheduleIndex)
	{
		TArray<FCharacterSpawnData> Out;
		Out.Reserve(InCharacters.Num());

		for (const UCharacterAsset* Char : InCharacters)
		{
			if (!Char) continue;

			const FProvinceId CurrentProvinceId(InProvinces.Find(Char->GetCurrentProvince()));
			const FRaceId RaceId(InRaceIndex.Find(Char->GetBaseRace()));

			FCharacterSpawnData Data;
			Data.Display.CharacterName = Char->GetCharacterName();
			Data.Display.Portrait = Char->GetPortrait();
			Data.BirthProvinceId = FProvinceId(InProvinces.Find(Char->GetProvinceOfBirth()));
			Data.BaseRaceId = RaceId;
			Data.CurrentProvinceId = CurrentProvinceId;
			Data.SimTier = Char->GetSimTier();

			if (InProvinceInfos.IsValidIndex(CurrentProvinceId.Value))
			{
				Data.TimezoneOffset = InProvinceInfos[CurrentProvinceId.Value].TimezoneOffset;
			}

			if (InRaces.IsValidIndex(RaceId.Value))
			{
				const FRace& Race = InRaces[RaceId.Value];
				Data.RaceModifiers = FAbilities{
					Race.Strength, Race.Dexterity, Race.Constitution,
					Race.Intelligence, Race.Wisdom, Race.Charisma
				};
			}

			// A character without a schedule asset keeps the all-NONE default and an invalid
			// FScheduleId: legal, but it will idle through every hour the sim processor runs.
			if (UScheduleAsset* Schedule = Char->GetSchedule())
			{
				Data.Schedule = Schedule->GetSchedule();
				Data.ScheduleId = FScheduleId(InScheduleIndex.Find(Schedule));
			}

			Out.Add(MoveTemp(Data));
		}

		return Out;
	}

	/**
	 * Organizations founded up front by the scenario. RtsGameModeBase feeds each entry to
	 * UOrganizations::Add — the same entry point a player-triggered founding would use — so
	 * scenario-authored and in-play organizations end up indistinguishable at runtime.
	 */
	__inline TArray<FOrganizationInfo> ExtractOrganizations(const TArray<UOrganizationAsset*>& InOrganizations)
	{
		TArray<FOrganizationInfo> Out;
		Out.Reserve(InOrganizations.Num());

		for (const UOrganizationAsset* Org : InOrganizations)
		{
			if (!Org) continue;

			FOrganizationInfo Info;
			Info.OrganizationName = Org->GetOrganizationName();
			Info.Logo = Org->GetLogo();
			Out.Add(MoveTemp(Info));
		}

		return Out;
	}

	/**
	 * Roles offered across every organization. RtsGameModeBase feeds each entry to URoles::Setup
	 * before organizations are founded, so a founder position can already resolve a leadership role.
	 */
	__inline TArray<FRoleInfo> ExtractRoles(const TArray<URoleAsset*>& InRoles)
	{
		TArray<FRoleInfo> Out;
		Out.Reserve(InRoles.Num());

		for (const URoleAsset* Role : InRoles)
		{
			if (!Role) continue;

			FRoleInfo Info;
			Info.RoleName = Role->GetRoleName();
			Info.Permissions = Role->GetPermissions();
			Info.bIsLeadership = Role->IsLeadership();
			Out.Add(MoveTemp(Info));
		}

		return Out;
	}
}
