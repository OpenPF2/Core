// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Libraries/PF2DiceLibrary.h"

#include "Utilities/PF2ArrayUtilities.h"

const FRegexPattern UPF2DiceLibrary::DiceRollPattern = FRegexPattern(TEXT("^(\\d{1,})d(\\d{1,})$"));

// NAME_None forces the RNG to initialize itself with a random seed.
FRandomStream UPF2DiceLibrary::DiceRng = FRandomStream(NAME_None);

int32 UPF2DiceLibrary::RollStringSum(const FName RollExpression)
{
	return PF2ArrayUtilities::Reduce(
		RollString(RollExpression),
		0,
		[](const int32 PreviousValue, const int32 CurrentValue)
		{
			return PreviousValue + CurrentValue;
		});
}

int32 UPF2DiceLibrary::RollSum(const int32 RollCount, const int32 DieSize)
{
	return PF2ArrayUtilities::Reduce(
		Roll(RollCount, DieSize),
		0,
		[](const int32 PreviousValue, const int32 CurrentValue)
		{
			return PreviousValue + CurrentValue;
		});
}

TArray<int32> UPF2DiceLibrary::RollString(const FName RollExpression)
{
	TArray<int32> Result;
	int32         RollCount,
	              DieSize;

	if (ParseRollExpression(RollExpression, RollCount, DieSize))
	{
		Result = Roll(RollCount, DieSize);
	}

	return Result;
}

TArray<int32> UPF2DiceLibrary::Roll(const int32 RollCount, const int32 DieSize)
{
	TArray<int32> Rolls;

	for (int32 RollIndex = 0; RollIndex < RollCount; ++RollIndex)
	{
		int32 Roll;

		if (DieSize == 0)
		{
			// Edge case: Unlikely to happen, but just in case, we need to make sure that rolling a zero-sided die does
			// not return 1. Could happen if the die size is passed in dynamically.
			Roll = 0;
		}
		else
		{
			Roll = GetDiceRng().RandRange(1, DieSize);
		}

		Rolls.Add(Roll);
	}

	return Rolls;
}

FName UPF2DiceLibrary::NextSizeString(const FName RollExpression)
{
	FName Result;
	int32 RollCount,
	      DieSize;

	if (ParseRollExpression(RollExpression, RollCount, DieSize))
	{
		const int32 NextDieSize = NextSize(DieSize);

		Result = FName(
			FString::Format(
				TEXT("{0}d{1}"),
				{
					FString::FormatAsNumber(RollCount),
					FString::FormatAsNumber(NextDieSize)
				}
			)
		);
	}
	else
	{
		Result = FName(TEXT("0d0"));
	}

	return Result;
}

int32 UPF2DiceLibrary::NextSize(const int32 DieSize)
{
	return DieSize + 2;
}

bool UPF2DiceLibrary::ParseRollExpression(const FName RollExpression, int32& RollCount, int32& DieSize)
{
	bool          bResult;
	FRegexMatcher ExpressionMatcher = FRegexMatcher(DiceRollPattern, RollExpression.ToString().ToLower());

	if (ExpressionMatcher.FindNext())
	{
		const FString RollCountString = ExpressionMatcher.GetCaptureGroup(1),
		              DieSizeString   = ExpressionMatcher.GetCaptureGroup(2);

		RollCount = FCString::Atoi(*RollCountString);
		DieSize   = FCString::Atoi(*DieSizeString);
		bResult   = true;
	}
	else
	{
		RollCount = 0;
		DieSize   = 0;
		bResult   = false;
	}

	return bResult;
}

int32 UPF2DiceLibrary::GetRandomSeed()
{
	return DiceRng.GetInitialSeed();
}

void UPF2DiceLibrary::SetRandomSeed(const int32 Seed)
{
	DiceRng.Initialize(Seed);
}
