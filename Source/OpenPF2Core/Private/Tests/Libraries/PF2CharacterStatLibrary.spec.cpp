// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Libraries/PF2CharacterStatLibrary.h"

#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2CharacterStatLibrarySpec,
                     "OpenPF2.Libraries.CharacterStat",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
END_DEFINE_PF_SPEC(FPF2CharacterStatLibrarySpec)

void FPF2CharacterStatLibrarySpec::Define()
{
	Describe(TEXT("CalculateAbilityModifier"), [=, this]
	{
		struct FAbilityModifierTestTuple
		{
			float AbilityScoreValue;
			float ExpectedModifier;
		};

		const TArray<FAbilityModifierTestTuple> TestParameters =
		{
			{  1, -5 },
			{  2, -4 },
			{  3, -4 },
			{ 10,  0 },
			{ 11,  0 },
			{ 24,  7 },
			{ 25,  7 },
		};

		for (const auto& CurrentTestParameters : TestParameters)
		{
			const float AbilityScoreValue = CurrentTestParameters.AbilityScoreValue,
			            ExpectedModifier  = CurrentTestParameters.ExpectedModifier;

			Describe(FString::Format(TEXT("when the ability score value is '{0}'"), {FString::FormatAsNumber(AbilityScoreValue)}), [=, this]
			{
				It(FString::Format(TEXT("returns '{0}'"), {FString::FormatAsNumber(ExpectedModifier)}), [=, this]
				{
					TestEqual(
						TEXT("Result"),
						UPF2CharacterStatLibrary::CalculateAbilityModifier(AbilityScoreValue),
						ExpectedModifier
					);
				});
			});
		}
	});

	Describe(TEXT("CalculateAbilityBoostAmount"), [=, this]
	{
		struct FAbilityBoostTestTuple
		{
			float StartingAbilityScore;
			float BoostCount;
			float ExpectedBoostAmount;
		};

		const TArray<FAbilityBoostTestTuple> TestParameters =
		{
			{ 10, 1, 2 },
			{ 18, 1, 1 },
			{ 10, 5, 9 },
			{ 10, 0, 0 },
		};

		for (const auto& CurrentTestParameters : TestParameters)
		{
			const float StartingAbilityScore = CurrentTestParameters.StartingAbilityScore,
			            BoostCount           = CurrentTestParameters.BoostCount,
			            ExpectedBoostAmount  = CurrentTestParameters.ExpectedBoostAmount;

			Describe(FString::Format(TEXT("when the starting ability score is '{0}' and the boost count is '{1}'"), {FString::FormatAsNumber(StartingAbilityScore), FString::FormatAsNumber(BoostCount)}), [=, this]
			{
				It(FString::Format(TEXT("returns '{0}'"), {FString::FormatAsNumber(ExpectedBoostAmount)}), [=, this]
				{
					TestEqual(
						TEXT("Result"),
						UPF2CharacterStatLibrary::CalculateAbilityBoostAmount(StartingAbilityScore, BoostCount),
						ExpectedBoostAmount
					);
				});
			});
		}
	});

	Describe(TEXT("CalculateAncestryFeatCap"), [=, this]
	{
		struct FFeatCapTestTuple
		{
			float CharacterLevel;
			float ExpectedFeatCap;
		};

		const TArray<FFeatCapTestTuple> TestParameters =
		{
			{  1, 1 },
			{  2, 1 },
			{  3, 1 },
			{  4, 1 },
			{  5, 2 },
			{  6, 2 },
			{  9, 3 },
			{ 13, 4 },
			{ 17, 5 },
		};

		for (const auto& CurrentTestParameters : TestParameters)
		{
			const float CharacterLevel  = CurrentTestParameters.CharacterLevel,
			            ExpectedFeatCap = CurrentTestParameters.ExpectedFeatCap;

			Describe(FString::Format(TEXT("when the character level is '{0}'"), {FString::FormatAsNumber(CharacterLevel)}), [=, this]
			{
				It(FString::Format(TEXT("returns '{0}'"), {FString::FormatAsNumber(ExpectedFeatCap)}), [=, this]
				{
					TestEqual(
						TEXT("Result"),
						UPF2CharacterStatLibrary::CalculateAncestryFeatCap(CharacterLevel),
						ExpectedFeatCap
					);
				});
			});
		}
	});
}
