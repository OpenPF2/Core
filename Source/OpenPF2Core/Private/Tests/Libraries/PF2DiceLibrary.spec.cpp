// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Libraries/PF2DiceLibrary.h"

#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2DiceLibrarySpec,
                     "OpenPF2.Libraries.Dice",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
END_DEFINE_PF_SPEC(FPF2DiceLibrarySpec)

void FPF2DiceLibrarySpec::Define()
{
	struct FDiceRollStringSumTestTuple
	{
		FString RollString;
		int MinSum;
		int MaxSum;
	};

	struct FDiceRollStringTestTuple
	{
		FString RollString;
		int RollCount;
		int MinRoll;
		int MaxRoll;
	};

	struct FDiceRollTestTuple
	{
		int RollCount;
		int DieSize;
		int MinSum;
		int MaxSum;
	};

	struct FParseRollExpressionTestTuple
	{
		FString RollExpression;
		bool bWasParsed;
		int RollCount;
		int DieSize;
	};

	Describe(TEXT("RollStringSum"), [=, this]
	{
		TArray<FDiceRollStringSumTestTuple> ExpectedRanges =
		{
			// String, Min Sum, Max Sum
			{   "0d5",       0,       0 },
			{   "1d0",       0,       0 },
			{   "BAD",       0,       0 },
			{   "1d6",       1,       6 },
			{   "2d6",       1,      12 },
			{   "1d2",       1,       2 },
			{   "3d5",       1,      15 },
		};

		for (const auto& [RollExpression, MinSum, MaxSum] : ExpectedRanges)
		{
			Describe(FString::Format(TEXT("when given '{0}'"), {RollExpression}), [=, this]
			{
				It(FString::Format(TEXT("returns a sum greater than or equal to '{0}' over 100 rolls"), {FString::FormatAsNumber(MinSum)}), [=, this]
				{
					float MinRollSeen = 9999;

					for (int RollIndex = 0; RollIndex < 100; ++RollIndex)
					{
						const float RollSum = UPF2DiceLibrary::RollStringSum(FName(RollExpression));

						MinRollSeen = FMath::Min(MinRollSeen, RollSum);
					}

					TestTrue(
						FString::Format(
							TEXT("{0} >= {1}"),
							{
								FString::FormatAsNumber(MinRollSeen),
								FString::FormatAsNumber(MinSum),
							}
						),
						MinRollSeen >= MinSum
					);
				});

				It(FString::Format(TEXT("returns a sum less than or equal to '{0}' over 100 rolls"), {FString::FormatAsNumber(MaxSum)}), [=, this]
				{
					float MaxRollSeen = -1;

					for (int RollIndex = 0; RollIndex < 100; ++RollIndex)
					{
						const float RollSum = UPF2DiceLibrary::RollStringSum(FName(RollExpression));

						MaxRollSeen = FMath::Max(MaxRollSeen, RollSum);
					}

					TestTrue(
						FString::Format(
							TEXT("{0} <= {1}"),
							{
								FString::FormatAsNumber(MaxRollSeen),
								FString::FormatAsNumber(MaxSum)
							}),
						MaxRollSeen <= MaxSum
					);
				});
			});
		}
	});

	Describe(TEXT("RollSum"), [=, this]
	{
		TArray<FDiceRollTestTuple> ExpectedRanges =
		{
			// Roll Count, Die Size, Min Sum, Max Sum
			{           0,        5,       0,       0 },
			{           1,        0,       0,       0 },
			{           1,        6,       1,       6 },
			{           2,        6,       1,      12 },
			{           1,        2,       1,       2 },
			{           3,        5,       1,      15 },
		};

		for (const auto& [RollCount, DieSize, MinSum, MaxSum] : ExpectedRanges)
		{
			Describe(FString::Format(TEXT("when given '{0}d{1}'"), {FString::FormatAsNumber(RollCount), FString::FormatAsNumber(DieSize)}), [=, this]
			{
				It(FString::Format(TEXT("returns a sum greater than or equal to '{0}' over 100 rolls"), {FString::FormatAsNumber(MinSum)}), [=, this]
				{
					float MinRollSeen = 9999;

					for (int RollIndex = 0; RollIndex < 100; ++RollIndex)
					{
						const float RollSum = UPF2DiceLibrary::RollSum(RollCount, DieSize);

						MinRollSeen = FMath::Min(MinRollSeen, RollSum);
					}

					TestTrue(
						FString::Format(
							TEXT("{0} >= {1}"),
							{
								FString::FormatAsNumber(MinRollSeen),
								FString::FormatAsNumber(MinSum),
							}
						),
						MinRollSeen >= MinSum
					);
				});

				It(FString::Format(TEXT("returns a sum less than or equal to '{0}' over 100 rolls"), {FString::FormatAsNumber(MaxSum)}), [=, this]
				{
					float MaxRollSeen = -1;

					for (int RollIndex = 0; RollIndex < 100; ++RollIndex)
					{
						const float RollSum = UPF2DiceLibrary::RollSum(RollCount, DieSize);

						MaxRollSeen = FMath::Max(MaxRollSeen, RollSum);
					}

					TestTrue(
						FString::Format(
							TEXT("{0} <= {1}"),
							{
								FString::FormatAsNumber(MaxRollSeen),
								FString::FormatAsNumber(MaxSum)
							}),
						MaxRollSeen <= MaxSum
					);
				});
			});
		}
	});

	Describe(TEXT("RollString"), [=, this]
	{
		TArray<FDiceRollStringTestTuple> ExpectedRanges =
		{
			// String, Roll Count, Min Roll, Max Roll
			{   "0d5",          0,        0,        0 },
			{   "1d0",          1,        0,        0 },
			{   "BAD",          0,        0,        0 },
			{   "1d6",          1,        1,        6 },
			{   "2d6",          2,        1,        6 },
			{   "1d2",          1,        1,        2 },
			{   "3d5",          3,        1,        5 },
		};

		for (const auto& [RollString, RollCount, MinRoll, MaxRoll] : ExpectedRanges)
		{
			Describe(FString::Format(TEXT("when given '{0}'"), {RollString}), [=, this]
			{
				It(FString::Format(TEXT("returns an array that contains {0} results"), {FString::FormatAsNumber(RollCount)}), [=, this]
				{
					const TArray<int32> Rolls = UPF2DiceLibrary::RollString(FName(RollString));

					TestEqual(
						FString::Format(TEXT("RollString({0}).Num()"), {RollString, FString::FormatAsNumber(RollCount)}),
						Rolls.Num(),
						RollCount
					);
				});

				It(FString::Format(TEXT("returns rolls greater than or equal to '{0}' over 100 rolls"), {FString::FormatAsNumber(MinRoll)}), [=, this]
				{
					float MinRollSeen = 9999;

					for (int RollIndex = 0; RollIndex < 100; ++RollIndex)
					{
						const TArray<int32> Rolls = UPF2DiceLibrary::RollString(FName(RollString));

						for (const int& Roll : Rolls)
						{
							MinRollSeen = FMath::Min(MinRollSeen, static_cast<float>(Roll));
						}
					}

					TestTrue(
						FString::Format(
							TEXT("{0} >= {1}"),
							{
								FString::FormatAsNumber(MinRollSeen),
								FString::FormatAsNumber(MinRoll),
							}
						),
						MinRollSeen >= MinRoll
					);
				});

				It(FString::Format(TEXT("returns rolls less than or equal to '{0}' over 100 rolls"), {FString::FormatAsNumber(MaxRoll)}), [=, this]
				{
					float MaxRollSeen = -1;

					for (int RollIndex = 0; RollIndex < 100; ++RollIndex)
					{
						const TArray<int32> Rolls = UPF2DiceLibrary::RollString(FName(RollString));

						for (const int& Roll : Rolls)
						{
							MaxRollSeen = FMath::Max(MaxRollSeen, static_cast<float>(Roll));
						}
					}

					TestTrue(
						FString::Format(
							TEXT("{0} <= {1}"),
							{
								FString::FormatAsNumber(MaxRollSeen),
								FString::FormatAsNumber(MaxRoll)
							}),
						MaxRollSeen <= MaxRoll
					);
				});
			});
		}
	});

	Describe(TEXT("Roll"), [=, this]
	{
		for (int DieSize = 1; DieSize < 64; ++DieSize)
		{
			Describe(FString::Format(TEXT("when given a '{0}'-sided die"), {FString::FormatAsNumber(DieSize)}), [=, this]
			{
				It(TEXT("returns an array that contains one result for each roll"), [=, this]
				{
					TestEqual(
						FString::Format(TEXT("Roll(3, {0}).Num()"), {DieSize}),
						UPF2DiceLibrary::Roll(3, DieSize).Num(),
						3
					);

					TestEqual(
						FString::Format(TEXT("Roll(10, {0}).Num()"), {DieSize}),
						UPF2DiceLibrary::Roll(10, DieSize).Num(),
						10
					);
				});

				It(TEXT("returns rolls greater than or equal to '1' over 100 rolls"), [=, this]
				{
					float               MinRollSeen = 9999;
					const TArray<int32> Rolls       = UPF2DiceLibrary::Roll(100, DieSize);

					for (const int& Roll : Rolls)
					{
						MinRollSeen = FMath::Min(MinRollSeen, static_cast<float>(Roll));
					}

					TestTrue(
						FString::Format(TEXT("{0} >= 1"), {FString::FormatAsNumber(MinRollSeen)}),
						MinRollSeen >= 1
					);
				});

				It(FString::Format(TEXT("returns rolls less than or equal to '{0}' over 100 rolls"), {FString::FormatAsNumber(DieSize)}), [=, this]
				{
					float               MaxRollSeen = -1;
					const TArray<int32> Rolls       = UPF2DiceLibrary::Roll(100, DieSize);

					for (const int& Roll : Rolls)
					{
						MaxRollSeen = FMath::Max(MaxRollSeen, static_cast<float>(Roll));
					}

					TestTrue(
						FString::Format(
							TEXT("{0} <= {1}"),
							{
								FString::FormatAsNumber(MaxRollSeen),
								FString::FormatAsNumber(DieSize)
							}),
						MaxRollSeen <= DieSize
					);
				});
			});
		}
	});

	Describe(TEXT("NextSizeString"), [=, this]
	{
		TMap<FString, FString> ExpectedValues =
		{
			{ "1d6",  "1d8"  },
			{ "2d6",  "2d8"  },
			{ "1d3",  "1d5"  },
			{ "8d1",  "8d3"  },
			{ "1d16", "1d18" },
		};

		for (const auto& TestParameters : ExpectedValues)
		{
			const FString RollExpression = TestParameters.Key;
			const FString ExpectedOutput = TestParameters.Value;

			Describe(FString::Format(TEXT("when given '{0}'"), {RollExpression}), [=, this]
			{
				It(FString::Format(TEXT("returns '{0}'"), {ExpectedOutput}), [=, this]
				{
					TestEqual(
						"Result",
						UPF2DiceLibrary::NextSizeString(FName(RollExpression)),
						FName(ExpectedOutput)
					);
				});
			});
		}
	});

	Describe(TEXT("NextSize"), [=, this]
	{
		TMap<int32, int32> ExpectedValues =
		{
			{  6,  8 },
			{  3,  5 },
			{  1,  3 },
			{ 16, 18 },
		};

		for (const auto& TestParameters : ExpectedValues)
		{
			const int32 DieSize        = TestParameters.Key;
			const int32 ExpectedOutput = TestParameters.Value;

			Describe(FString::Format(TEXT("when given '{0}'"), {FString::FormatAsNumber(DieSize)}), [=, this]
			{
				It(FString::Format(TEXT("returns '{0}'"), {FString::FormatAsNumber(ExpectedOutput)}), [=, this]
				{
					TestEqual(
						"Result",
						UPF2DiceLibrary::NextSize(DieSize),
						ExpectedOutput
					);
				});
			});
		}
	});

	Describe(TEXT("ParseRollExpression"), [=, this]
	{
		TArray<FParseRollExpressionTestTuple> ExpectedValues =
		{
			{"1d6",  true,  1, 6},
			{"1d3",  true,  1, 3},
			{"8d1",  true,  8, 1},
			{"8d-1", false, 0, 0},
			{"BAD",  false, 0, 0},
		};

		for (const auto& [RollExpression, bExpectedWasParsed, ExpectedRollCount, ExpectedDieSize] : ExpectedValues)
		{
			Describe(FString::Format(TEXT("when given '{0}'"), {RollExpression}), [=, this]
			{
				It(FString::Format(TEXT("{0} as roll count {1} and dice size {2}"), {bExpectedWasParsed ? "parses" : "fails to parse", FString::FormatAsNumber(ExpectedRollCount), FString::FormatAsNumber(ExpectedDieSize)}), [=, this]
				{
					int32 RollCount,
					      DieSize;

					TestEqual(
						"Result",
						UPF2DiceLibrary::ParseRollExpression(*RollExpression, RollCount, DieSize),
						bExpectedWasParsed
					);

					TestEqual("RollCount", RollCount, ExpectedRollCount);
					TestEqual("DieSize", DieSize, ExpectedDieSize);
				});
			});
		}
	});
}
