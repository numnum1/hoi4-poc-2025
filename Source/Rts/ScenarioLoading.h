#pragma once

#include "CharacterAsset.h"
#include "CharacterInfo.h"
#include "Race.h"
#include "RaceAsset.h"
#include "Schedule.h"
#include "NationAsset.h"
#include "ProvinceAsset.h"

namespace ScenarioLoader
{
	__inline TTuple<TArray<FRace>, TArray<URaceAsset*>> ExtractRaces(const TArray<UCharacterAsset*>& InCharacters)
	{
		TArray<FRace> OutRaces{};
		TArray<URaceAsset*> Assets{};
		for (UCharacterAsset* T : InCharacters)
		{
			if (Assets.Contains(T->GetBaseRace())) continue;
			OutRaces.Add(FRace{
				T->GetBaseRace()->GetRaceName()
			});
		}
		return {OutRaces, Assets};
	}
	
	
	__inline TTuple<TArray<FNationInfo>, TArray<FProvinceInfo>> ExtractNationsAndProvinces(const TArray<UProvinceAsset*>& InProvinces)
	{
		TTuple<TArray<FNationInfo>, TArray<FProvinceInfo>> Res;

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

			Res.Value.Add(FProvinceInfo{
				Province->GetMapId(),
				Province->GetProvinceName(),
				Province->GetPicture(),
				Province->GetInspectionSound(),
				Province->GetEcology(),
				Province->GetTimezoneOffset(),
				NationIndex
			});
		}

		return Res;
	}
	
	__inline TTuple<
	TArray<FCharacterInfo>,
	TArray<int32>,
	TArray<int32>,
	TArray<FSchedule>
	>
	ExtractCharacters(const TArray<UCharacterAsset*>& InCharacters, const TArray<UProvinceAsset*> InProvinces, const TArray<URaceAsset*>& InRaceIndex)
	{
		TArray<FCharacterInfo> Infos;
		TArray<int32> CurrentProvinceIndexes;
		TArray<int32> ScheduleCharacterIndexMapping;
		TArray<FSchedule> Schedules;
		
		Infos.Reserve(InCharacters.Num());
		CurrentProvinceIndexes.Reserve(InCharacters.Num());
		ScheduleCharacterIndexMapping.Reserve(InCharacters.Num());
		Schedules.Reserve(InCharacters.Num());

		for (int I = 0; I < InCharacters.Num(); ++I)
		{
			const UCharacterAsset* Char = InCharacters[I];
			Infos[I] = FCharacterInfo{
				Char->GetCharacterName(),
				Char->GetPortrait(),
				InProvinces.Find(Char->GetProvinceOfBirth()),
				InRaceIndex.Find(Char->GetBaseRace()),
				InProvinces.Find(Char->GetCurrentProvince())
			};
		}
		
		return {Infos, CurrentProvinceIndexes, ScheduleCharacterIndexMapping, Schedules};
	}
	
	
}
