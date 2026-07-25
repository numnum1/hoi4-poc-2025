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
#include "ProvinceAsset.h"
#include "ProvinceInfo.h"

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
			const int32 NationIndex = NationAssets.Find(Nation);

			FProvinceSpawnData SpawnData;
			SpawnData.Display.MapId = Province->GetMapId();
			SpawnData.Display.Name = Province->GetProvinceName();
			SpawnData.Display.Picture = Province->GetPicture();
			SpawnData.Display.InspectionSound = Province->GetInspectionSound();
			SpawnData.Display.Ecology = Province->GetEcology();
			SpawnData.Display.TimezoneOffset = Province->GetTimezoneOffset();
			SpawnData.OwningNationIndex = NationIndex;
			Res.Value.Add(MoveTemp(SpawnData));
		}

		return Res;
	}

	/**
	 * Everything a character needs at creation, resolved to runtime indexes.
	 * InProvinceInfos must be the array ExtractNationsAndProvinces produced from InProvinces,
	 * so index i of both refers to the same province; it supplies the timezone offset that
	 * gets cached on the entity.
	 */
	__inline TArray<FCharacterSpawnData>
	ExtractCharacters(const TArray<UCharacterAsset*>& InCharacters, const TArray<UProvinceAsset*>& InProvinces,
	                  const TArray<FProvinceSpawnData>& InProvinceInfos, const TArray<URaceAsset*>& InRaceIndex,
	                  const TArray<FRace>& InRaces)
	{
		TArray<FCharacterSpawnData> Out;
		Out.Reserve(InCharacters.Num());

		for (const UCharacterAsset* Char : InCharacters)
		{
			if (!Char) continue;

			const int32 CurrentProvinceIndex = InProvinces.Find(Char->GetCurrentProvince());
			const int32 RaceIndex = InRaceIndex.Find(Char->GetBaseRace());

			FCharacterSpawnData Data;
			Data.Display.CharacterName = Char->GetCharacterName();
			Data.Display.Portrait = Char->GetPortrait();
			Data.BirthProvinceIndex = InProvinces.Find(Char->GetProvinceOfBirth());
			Data.BaseRaceIndex = RaceIndex;
			Data.CurrentProvinceIndex = CurrentProvinceIndex;
			Data.SimTier = Char->GetSimTier();

			if (InProvinceInfos.IsValidIndex(CurrentProvinceIndex))
			{
				Data.TimezoneOffset = InProvinceInfos[CurrentProvinceIndex].Display.TimezoneOffset;
			}

			if (InRaces.IsValidIndex(RaceIndex))
			{
				const FRace& Race = InRaces[RaceIndex];
				Data.RaceModifiers = FAbilities{
					Race.Strength, Race.Dexterity, Race.Constitution,
					Race.Intelligence, Race.Wisdom, Race.Charisma
				};
			}

			// TODO: schedules are not authored yet, so this stays an all-NONE default.
			Out.Add(MoveTemp(Data));
		}

		return Out;
	}
}
