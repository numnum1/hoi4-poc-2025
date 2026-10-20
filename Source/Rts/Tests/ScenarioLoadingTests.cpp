// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#include "Misc/AutomationTest.h"
#include "Rts/ScenarioLoading.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace ScenarioLoadingTests
{
	constexpr EAutomationTestFlags TestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter;

	template<typename TValue>
	void SetPrivateProperty(UObject* InObject, const FName InPropertyName, const TValue& InValue)
	{
		const FProperty* Property = FindFProperty<FProperty>(InObject->GetClass(), InPropertyName);
		check(Property != nullptr);
		check(Property->GetElementSize() == sizeof(TValue));
		*Property->ContainerPtrToValuePtr<TValue>(InObject) = InValue;
	}

	URaceAsset* MakeRace(const uint8 InStrength = 0)
	{
		URaceAsset* Race = NewObject<URaceAsset>();
		SetPrivateProperty<uint8>(Race, TEXT("Strength"), InStrength);
		return Race;
	}

	UEthnicityAsset* MakeEthnicity(URaceAsset* InRace)
	{
		UEthnicityAsset* Ethnicity = NewObject<UEthnicityAsset>();
		SetPrivateProperty<TObjectPtr<URaceAsset>>(Ethnicity, TEXT("Race"), InRace);
		return Ethnicity;
	}

	FProvincePopulationEntry MakePopulationEntry(UEthnicityAsset* InEthnicity, const int32 InSize)
	{
		FProvincePopulationEntry Entry;
		Entry.Ethnicity = InEthnicity;
		Entry.Size = InSize;
		return Entry;
	}

	UProvinceAsset* MakeProvince(UNationAsset* InNation, const int32 InTimezoneOffset = 0,
	                             const TArray<FProvincePopulationEntry>& InPopulation = {})
	{
		UProvinceAsset* Province = NewObject<UProvinceAsset>();
		SetPrivateProperty<UNationAsset*>(Province, TEXT("OwningNation"), InNation);
		SetPrivateProperty<int32>(Province, TEXT("TimezoneOffset"), InTimezoneOffset);
		SetPrivateProperty<TArray<FProvincePopulationEntry>>(Province, TEXT("Population"), InPopulation);
		return Province;
	}

	UCharacterAsset* MakeCharacter(UProvinceAsset* InCurrentProvince, URaceAsset* InRace, UScheduleAsset* InSchedule = nullptr)
	{
		UCharacterAsset* Character = NewObject<UCharacterAsset>();
		SetPrivateProperty<UProvinceAsset*>(Character, TEXT("CurrentProvince"), InCurrentProvince);
		SetPrivateProperty<UProvinceAsset*>(Character, TEXT("ProvinceOfBirth"), InCurrentProvince);
		SetPrivateProperty<URaceAsset*>(Character, TEXT("BaseRace"), InRace);
		SetPrivateProperty<UScheduleAsset*>(Character, TEXT("Schedule"), InSchedule);
		return Character;
	}
}

using namespace ScenarioLoadingTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractNationsAndProvincesTest,
	"Rts.ScenarioLoading.ExtractNationsAndProvinces.DeduplicatesNationsAndSkipsMissingProvinces", ScenarioLoadingTests::TestFlags)

bool FExtractNationsAndProvincesTest::RunTest(const FString& Parameters)
{
	UNationAsset* NationA = NewObject<UNationAsset>();
	UNationAsset* NationB = NewObject<UNationAsset>();

	const TArray<UProvinceAsset*> Provinces = {
		MakeProvince(NationA, 1),
		nullptr,
		MakeProvince(NationB, 2),
		MakeProvince(NationA, 3),
		MakeProvince(nullptr, 4)
	};

	const auto [Nations, SpawnData] = ScenarioLoader::ExtractNationsAndProvinces(Provinces);

	TestEqual(TEXT("A nation shared by several provinces is listed once"), Nations.Num(), 2);
	if (!TestEqual(TEXT("A null province entry produces no spawn data"), SpawnData.Num(), 4))
	{
		return false;
	}

	TestEqual(TEXT("First province belongs to the first nation"), SpawnData[0].OwningNationId.Value, 0);
	TestEqual(TEXT("Second province belongs to the second nation"), SpawnData[1].OwningNationId.Value, 1);
	TestEqual(TEXT("Third province resolves to the shared first nation"), SpawnData[2].OwningNationId.Value, 0);
	TestFalse(TEXT("A province without a nation has an invalid nation id"), SpawnData[3].OwningNationId.IsValid());
	TestEqual(TEXT("Timezone offset is carried over"), SpawnData[2].TimezoneOffset, 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractPopulationsTest,
	"Rts.ScenarioLoading.ExtractPopulations.MergesDuplicateEntriesInLibraryOrder", ScenarioLoadingTests::TestFlags)

bool FExtractPopulationsTest::RunTest(const FString& Parameters)
{
	URaceAsset* Race = MakeRace();
	UEthnicityAsset* EthnicityFirstSeen = MakeEthnicity(nullptr);
	UEthnicityAsset* EthnicitySecondSeen = MakeEthnicity(Race);

	const TArray<UProvinceAsset*> Provinces = {
		MakeProvince(nullptr, 0, {
			MakePopulationEntry(EthnicityFirstSeen, 10),
			MakePopulationEntry(EthnicitySecondSeen, 5),
			MakePopulationEntry(EthnicityFirstSeen, 7),
			MakePopulationEntry(EthnicitySecondSeen, 0)
		}),
		nullptr,
		MakeProvince(nullptr, 0, {
			MakePopulationEntry(EthnicitySecondSeen, 3)
		})
	};

	const auto [Ethnicities, Pops] = ScenarioLoader::ExtractPopulations(Provinces, { Race });

	if (!TestEqual(TEXT("Each ethnicity is listed once"), Ethnicities.Num(), 2))
	{
		return false;
	}
	TestFalse(TEXT("An ethnicity without a race has an invalid race id"), Ethnicities[0].RaceId.IsValid());
	TestEqual(TEXT("An ethnicity's race resolves through the race index"), Ethnicities[1].RaceId.Value, 0);

	if (!TestEqual(TEXT("One pop per province and ethnicity"), Pops.Num(), 3))
	{
		return false;
	}

	TestEqual(TEXT("Duplicate entries of one ethnicity are summed"), Pops[0].Size, 17);
	TestEqual(TEXT("Pops are ordered by ethnicity id"), Pops[0].EthnicityId.Value, 0);
	TestEqual(TEXT("Second pop of the first province"), Pops[1].EthnicityId.Value, 1);
	TestEqual(TEXT("Empty entries do not add to a pop"), Pops[1].Size, 5);
	TestEqual(TEXT("Province ids skip null province entries"), Pops[2].ProvinceId.Value, 1);
	TestEqual(TEXT("Pop size of the second province"), Pops[2].Size, 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractRacesTest,
	"Rts.ScenarioLoading.ExtractRaces.DiscoversRacesThroughCharactersAndEthnicities", ScenarioLoadingTests::TestFlags)

bool FExtractRacesTest::RunTest(const FString& Parameters)
{
	URaceAsset* CharacterRace = MakeRace(7);
	URaceAsset* EthnicityOnlyRace = MakeRace(3);

	const TArray<UProvinceAsset*> Provinces = {
		MakeProvince(nullptr, 0, { MakePopulationEntry(MakeEthnicity(EthnicityOnlyRace), 100) })
	};
	const TArray<UCharacterAsset*> Characters = {
		MakeCharacter(Provinces[0], CharacterRace),
		nullptr,
		MakeCharacter(Provinces[0], CharacterRace)
	};

	const auto [Categories, LifeTypes, Species, Races, RaceAssets] = ScenarioLoader::ExtractRaces(Characters, Provinces);

	if (!TestEqual(TEXT("Each race is listed once"), Races.Num(), 2))
	{
		return false;
	}
	TestTrue(TEXT("Races named by characters come first"), RaceAssets[0] == CharacterRace);
	TestTrue(TEXT("A race only reached through an ethnicity is still discovered"), RaceAssets[1] == EthnicityOnlyRace);
	TestEqual(TEXT("Abilities are copied from the asset"), static_cast<int32>(Races[0].Strength), 7);
	TestEqual(TEXT("A race without species has no species index"), Races[0].SpeciesIndex, static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("No species means no categories"), Categories.Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractSchedulesTest,
	"Rts.ScenarioLoading.ExtractSchedules.ListsEachSharedScheduleOnce", ScenarioLoadingTests::TestFlags)

bool FExtractSchedulesTest::RunTest(const FString& Parameters)
{
	UScheduleAsset* SharedSchedule = NewObject<UScheduleAsset>();
	UScheduleAsset* OtherSchedule = NewObject<UScheduleAsset>();

	const TArray<UCharacterAsset*> Characters = {
		MakeCharacter(nullptr, nullptr, SharedSchedule),
		MakeCharacter(nullptr, nullptr, nullptr),
		nullptr,
		MakeCharacter(nullptr, nullptr, OtherSchedule),
		MakeCharacter(nullptr, nullptr, SharedSchedule)
	};

	const auto [Infos, ScheduleAssets] = ScenarioLoader::ExtractSchedules(Characters);

	TestEqual(TEXT("One info per distinct schedule"), Infos.Num(), 2);
	if (!TestEqual(TEXT("One asset per distinct schedule"), ScheduleAssets.Num(), 2))
	{
		return false;
	}
	TestTrue(TEXT("Schedules keep first-seen order"), ScheduleAssets[0] == SharedSchedule && ScheduleAssets[1] == OtherSchedule);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractCharactersTest,
	"Rts.ScenarioLoading.ExtractCharacters.ResolvesReferencesToRuntimeIds", ScenarioLoadingTests::TestFlags)

bool FExtractCharactersTest::RunTest(const FString& Parameters)
{
	URaceAsset* Race = MakeRace(9);
	UScheduleAsset* Schedule = NewObject<UScheduleAsset>();

	const TArray<UProvinceAsset*> Provinces = {
		MakeProvince(nullptr, 1),
		MakeProvince(nullptr, 9)
	};
	UCharacterAsset* Scheduled = MakeCharacter(Provinces[1], Race, Schedule);
	UCharacterAsset* Unscheduled = MakeCharacter(Provinces[0], Race);
	const TArray<UCharacterAsset*> Characters = { Scheduled, nullptr, Unscheduled };

	const auto [Categories, LifeTypes, Species, Races, RaceAssets] = ScenarioLoader::ExtractRaces(Characters, Provinces);
	const auto [Nations, ProvinceSpawnData] = ScenarioLoader::ExtractNationsAndProvinces(Provinces);
	const auto [ScheduleInfos, ScheduleAssets] = ScenarioLoader::ExtractSchedules(Characters);

	const auto [SpawnData, CharacterAssets] = ScenarioLoader::ExtractCharacters(
		Characters, Provinces, ProvinceSpawnData, RaceAssets, Races, ScheduleAssets);

	if (!TestEqual(TEXT("A null character entry produces no spawn data"), SpawnData.Num(), 2))
	{
		return false;
	}
	TestTrue(TEXT("Source assets stay aligned with the spawn data"),
		CharacterAssets.Num() == 2 && CharacterAssets[0] == Scheduled && CharacterAssets[1] == Unscheduled);

	TestEqual(TEXT("Current province resolves to its id"), SpawnData[0].CurrentProvinceId.Value, 1);
	TestEqual(TEXT("Timezone offset comes from the current province"), SpawnData[0].TimezoneOffset, 9);
	TestEqual(TEXT("Race resolves to its id"), SpawnData[0].BaseRaceId.Value, 0);
	TestEqual(TEXT("Race abilities become modifiers"), static_cast<int32>(SpawnData[0].RaceModifiers.Strength), 9);
	TestEqual(TEXT("Schedule resolves to its id"), SpawnData[0].ScheduleId.Value, 0);
	TestFalse(TEXT("A character without a schedule has an invalid schedule id"), SpawnData[1].ScheduleId.IsValid());
	TestEqual(TEXT("Second character's timezone"), SpawnData[1].TimezoneOffset, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractBanksTest,
	"Rts.ScenarioLoading.ExtractBanks.SkipsMissingBanksAndKeepsAssetsAligned", ScenarioLoadingTests::TestFlags)

bool FExtractBanksTest::RunTest(const FString& Parameters)
{
	UBankAsset* FirstBank = NewObject<UBankAsset>();
	UBankAsset* SecondBank = NewObject<UBankAsset>();

	const auto [Infos, BankAssets] = ScenarioLoader::ExtractBanks({ FirstBank, nullptr, SecondBank });

	TestEqual(TEXT("One info per bank"), Infos.Num(), 2);
	TestTrue(TEXT("Bank ids are positions in the filtered asset array"),
		BankAssets.Num() == 2 && BankAssets[0] == FirstBank && BankAssets[1] == SecondBank);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractRolesTest,
	"Rts.ScenarioLoading.ExtractRoles.CopiesPermissionsAndFlags", ScenarioLoadingTests::TestFlags)

bool FExtractRolesTest::RunTest(const FString& Parameters)
{
	URoleAsset* Leader = NewObject<URoleAsset>();
	SetPrivateProperty<int32>(Leader, TEXT("Permissions"), 0b101);
	SetPrivateProperty<bool>(Leader, TEXT("bIsLeadership"), true);
	SetPrivateProperty<bool>(Leader, TEXT("bIsDefaultMembership"), false);

	URoleAsset* Member = NewObject<URoleAsset>();
	SetPrivateProperty<bool>(Member, TEXT("bIsLeadership"), false);
	SetPrivateProperty<bool>(Member, TEXT("bIsDefaultMembership"), true);

	const TArray<FRoleInfo> Roles = ScenarioLoader::ExtractRoles({ Leader, nullptr, Member });

	if (!TestEqual(TEXT("A null role entry is skipped"), Roles.Num(), 2))
	{
		return false;
	}
	TestEqual(TEXT("Permissions are copied"), Roles[0].Permissions, 0b101);
	TestTrue(TEXT("Leadership flag is copied"), Roles[0].bIsLeadership && !Roles[0].bIsDefaultMembership);
	TestTrue(TEXT("Default membership flag is copied"), !Roles[1].bIsLeadership && Roles[1].bIsDefaultMembership);
	return true;
}

#endif
