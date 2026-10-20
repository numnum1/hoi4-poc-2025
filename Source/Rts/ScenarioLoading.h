#pragma once

#include "Algo/Transform.h"
#include "BankAsset.h"
#include "BankInfo.h"
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
#include "EthnicityAsset.h"
#include "EthnicityInfo.h"
#include "PopulationInfo.h"
#include "ProvinceAsset.h"
#include "ProvinceInfo.h"
#include "RoleAsset.h"
#include "RoleInfo.h"
#include "ScheduleAsset.h"
#include "ScheduleInfo.h"

namespace ScenarioLoader
{
	/**
	 * The race library, discovered from everything that names a race: characters directly, and
	 * provinces through the ethnicities of the people living in them. Both sources are needed —
	 * an ethnicity's race that no character happens to have would otherwise be missing from the
	 * library, and FEthnicityInfo::RaceId would come out invalid for it.
	 */
	inline TTuple<TArray<FRaceCategory>, TArray<FLifeType>, TArray<FSpecies>, TArray<FRace>, TArray<URaceAsset*>>
	ExtractRaces(const TArray<UCharacterAsset*>& InCharacters, const TArray<UProvinceAsset*>& InProvinces)
	{
		TArray<URaceAsset*>        RaceAssets;
		TArray<USpeciesAsset*>     SpeciesAssets;
		TArray<ULifeTypeAsset*>    LifeTypeAssets;
		TArray<URaceCategoryAsset*> CategoryAssets;

		auto DiscoverRace = [&](URaceAsset* Race)
		{
			if (!Race || RaceAssets.Contains(Race)) return;
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
		};

		for (const UCharacterAsset* Char : InCharacters)
		{
			if (!Char) continue;
			DiscoverRace(Char->GetBaseRace());
		}

		for (const UProvinceAsset* Province : InProvinces)
		{
			if (!Province) continue;

			for (const FProvincePopulationEntry& Entry : Province->GetPopulation())
			{
				if (Entry.Ethnicity) DiscoverRace(Entry.Ethnicity->GetRace());
			}
		}

		auto ConvertTraits = [](const TArray<URaceTraitAsset*>& InTraits) -> TArray<FRaceTrait>
		{
			TArray<FRaceTrait> Out;
			Algo::Transform(InTraits, Out, [](const URaceTraitAsset* T)
			{
				return FRaceTrait{ T->GetTraitName(), T->GetTraitDescription() };
			});
			return Out;
		};

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


	inline TTuple<TArray<FNationInfo>, TArray<FProvinceSpawnData>> ExtractNationsAndProvinces(const TArray<UProvinceAsset*>& InProvinces)
	{
		TTuple<TArray<FNationInfo>, TArray<FProvinceSpawnData>> Res;

		TArray<UNationAsset*> NationAssets;
		NationAssets.Reserve(InProvinces.Num());

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
				Nation->GetFlag(),
				Nation->GetMapColor()
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
	 * The ethnicity library plus one pop per province and ethnicity, both discovered from the
	 * province assets — the same "find it through what uses it" shape ExtractRaces and
	 * ExtractSchedules have, so there is no separate ethnicity list that could fall out of sync.
	 *
	 * InProvinces must be the same array ExtractNationsAndProvinces was given: province ids are
	 * positions in it, so the null-skipping below has to match that function's exactly or every pop
	 * would land in the wrong province. InRaceIndex is the asset array ExtractRaces returned, so an
	 * ethnicity's race resolves to the same FRaceId URaces stores it under.
	 */
	inline TTuple<TArray<FEthnicityInfo>, TArray<FPopSpawnData>> ExtractPopulations(const TArray<UProvinceAsset*>& InProvinces,
	                                                                                  const TArray<URaceAsset*>& InRaceIndex)
	{
		TTuple<TArray<FEthnicityInfo>, TArray<FPopSpawnData>> Res;

		TArray<UEthnicityAsset*> EthnicityAssets;
		for (const UProvinceAsset* Province : InProvinces)
		{
			if (!Province) continue;

			for (const FProvincePopulationEntry& Entry : Province->GetPopulation())
			{
				if (Entry.Ethnicity && !EthnicityAssets.Contains(Entry.Ethnicity))
				{
					EthnicityAssets.Add(Entry.Ethnicity);
				}
			}
		}

		Res.Key.Reserve(EthnicityAssets.Num());
		for (const UEthnicityAsset* Ethnicity : EthnicityAssets)
		{
			FEthnicityInfo Info;
			Info.EthnicityName = Ethnicity->GetEthnicityName();
			Info.MapColor = Ethnicity->GetMapColor();
			Info.RaceId = FRaceId(InRaceIndex.Find(Ethnicity->GetRace()));
			Info.AnnualBirthRate = Ethnicity->GetAnnualBirthRate();
			Res.Key.Add(MoveTemp(Info));
		}

		int32 ProvinceIndex = 0;
		for (const UProvinceAsset* Province : InProvinces)
		{
			if (!Province) continue;

			const FProvinceId ProvinceId(ProvinceIndex++);

			// Summed per ethnicity rather than one pop per authored entry: an ethnicity listed twice
			// on the same province is the same people, and two pops for them would drift apart.
			TMap<FEthnicityId, int32> SizeByEthnicity;
			for (const FProvincePopulationEntry& Entry : Province->GetPopulation())
			{
				if (Entry.Ethnicity == nullptr || Entry.Size <= 0)
				{
					continue;
				}

				const FEthnicityId EthnicityId(EthnicityAssets.Find(Entry.Ethnicity));
				SizeByEthnicity.FindOrAdd(EthnicityId) += Entry.Size;
			}

			// In library order, so a tie in UPopulations::GetDominantEthnicity resolves to the lower
			// id rather than to whatever order the asset happened to list them in.
			for (int32 EthnicityIndex = 0; EthnicityIndex < EthnicityAssets.Num(); ++EthnicityIndex)
			{
				const FEthnicityId EthnicityId(EthnicityIndex);
				const int32* Size = SizeByEthnicity.Find(EthnicityId);
				if (Size == nullptr)
				{
					continue;
				}

				FPopSpawnData SpawnData;
				SpawnData.ProvinceId = ProvinceId;
				SpawnData.EthnicityId = EthnicityId;
				SpawnData.Size = *Size;
				Res.Value.Add(MoveTemp(SpawnData));
			}
		}

		return Res;
	}

	/**
	 * The schedule library, discovered from the characters that follow its entries — the same
	 * way ExtractRaces discovers races. Returns the runtime infos for USchedules alongside the
	 * asset array they came from, so ExtractCharacters can resolve an asset back to its
	 * FScheduleId by position.
	 */
	inline TTuple<TArray<FScheduleInfo>, TArray<UScheduleAsset*>>
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
	 *
	 * Also returns the filtered (null-skipped) source asset array alongside the spawn data, index
	 * for index — the same shape ExtractSchedules returns ScheduleAssets in — so RtsGameModeBase can
	 * read a character's BankAccount asset field back after Create() without redoing the null-filter.
	 */
	inline TTuple<TArray<FCharacterSpawnData>, TArray<UCharacterAsset*>>
	ExtractCharacters(const TArray<UCharacterAsset*>& InCharacters, const TArray<UProvinceAsset*>& InProvinces,
	                  const TArray<FProvinceSpawnData>& InProvinceInfos, const TArray<URaceAsset*>& InRaceIndex,
	                  const TArray<FRace>& InRaces, const TArray<UScheduleAsset*>& InScheduleIndex)
	{
		TArray<FCharacterSpawnData> Out;
		TArray<UCharacterAsset*> CharacterAssets;
		Out.Reserve(InCharacters.Num());
		CharacterAssets.Reserve(InCharacters.Num());

		for (UCharacterAsset* Char : InCharacters)
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
			CharacterAssets.Add(Char);
		}

		return { MoveTemp(Out), MoveTemp(CharacterAssets) };
	}

	/**
	 * Organizations founded up front by the scenario. RtsGameModeBase feeds each entry to
	 * UOrganizations::Add — the same entry point a player-triggered founding would use — so
	 * scenario-authored and in-play organizations end up indistinguishable at runtime.
	 *
	 * Also returns the filtered source asset array alongside, same reason as ExtractCharacters.
	 */
	inline TTuple<TArray<FOrganizationInfo>, TArray<UOrganizationAsset*>> ExtractOrganizations(const TArray<UOrganizationAsset*>& InOrganizations)
	{
		TArray<FOrganizationInfo> Out;
		TArray<UOrganizationAsset*> OrganizationAssets;
		Out.Reserve(InOrganizations.Num());
		OrganizationAssets.Reserve(InOrganizations.Num());

		for (UOrganizationAsset* Org : InOrganizations)
		{
			if (!Org) continue;

			FOrganizationInfo Info;
			Info.OrganizationName = Org->GetOrganizationName();
			Info.Logo = Org->GetLogo();
			Out.Add(MoveTemp(Info));
			OrganizationAssets.Add(Org);
		}

		return { MoveTemp(Out), MoveTemp(OrganizationAssets) };
	}

	/**
	 * Banks created up front by the scenario. Also returns the filtered source asset array, so
	 * a character's/organization's FBankAccountAsset::Bank can resolve to the same FBankId
	 * RtsGameModeBase's AddBank loop assigned it, by position — same shape as ExtractSchedules.
	 */
	inline TTuple<TArray<FBankInfo>, TArray<UBankAsset*>> ExtractBanks(const TArray<UBankAsset*>& InBanks)
	{
		TArray<FBankInfo> Out;
		TArray<UBankAsset*> BankAssets;
		Out.Reserve(InBanks.Num());
		BankAssets.Reserve(InBanks.Num());

		for (UBankAsset* Bank : InBanks)
		{
			if (!Bank) continue;

			FBankInfo Info;
			Info.BankName = Bank->GetBankName();
			Info.Logo = Bank->GetLogo();
			Out.Add(MoveTemp(Info));
			BankAssets.Add(Bank);
		}

		return { MoveTemp(Out), MoveTemp(BankAssets) };
	}

	/**
	 * Roles offered across every organization. RtsGameModeBase feeds each entry to URoles::Setup
	 * before organizations are founded, so a founder position can already resolve a leadership role.
	 */
	inline TArray<FRoleInfo> ExtractRoles(const TArray<URoleAsset*>& InRoles)
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
			Info.bIsDefaultMembership = Role->IsDefaultMembership();
			Out.Add(MoveTemp(Info));
		}

		return Out;
	}
}
