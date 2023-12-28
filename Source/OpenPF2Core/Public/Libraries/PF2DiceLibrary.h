// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Math/RandomStream.h>

#include "Internationalization/Regex.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "PF2DiceLibrary.generated.h"

/**
 * Simple function library for rolling dice of any size.
 *
 * Dice rolls can either be done by specifying the number of sides and number of rolls numerically; or by describing the
 * roll in a string format like "2d6" (for rolling two, six-sided dice). For example, the following calls would produce
 * equivalent results:
 *	- RollString("2d6") and Roll(2,6) - Roll two, six-sided dice.
 *	- RollString("1d4") and Roll(1,4) - Roll one, four-sided dice.
 *	- RollString("10d12") and Roll(10,12) - Roll ten, twelve-sided dice.
 *
 * Unlike other RPG systems like Dungeons and Dragons, P2E game rules do *not* appear to require the ability to evaluate
 * complex dice rolling expressions like "5d4+3", "3d6x10", or "d6/2". Consequently, these types of expressions are not
 * supported at this time.
 */
UCLASS()
class OPENPF2CORE_API UPF2DiceLibrary final : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Constants
	// =================================================================================================================
	/**
	 * Regular expression pattern used to parse dice rolling expressions.
	 */
	static const FRegexPattern DiceRollPattern;

	// =================================================================================================================
	// Protected Static Fields
	// =================================================================================================================
	/**
	 * The random number generator (RNG) used for dice rolls.
	 *
	 * This is a random stream so that dice rolls can be seeded deterministically during tests.
	 */
	static FRandomStream DiceRng;

public:
	// =================================================================================================================
	// Public Static Methods
	// =================================================================================================================
	/**
	 * Returns the sum of a dice roll for the given dice roll expression string.
	 *
	 * Complex roll expressions like "3d6x10" are not supported at this time.
	 *
	 * @param RollExpression
	 *	The description of the roll, in "CdS" format, where "C" represents the count or number of dice to roll, and "S"
	 *	represents the number of sides of each die (the die size). For example, "1d6" represents a single roll of a
	 *	six-sided die, while "2d4" represents rolling two dice having four sides each.
	 *
	* @return
	 *	The sum of the dice roll(s).
	 */
	UFUNCTION(BlueprintPure=false, Category="OpenPF2|Dice", DisplayName="Roll Dice (Roll Expression) and Sum")
	static int32 RollStringSum(const FName RollExpression);

	/**
	 * Returns the sum of a dice roll for the given numeric parameters.
	 *
	 * @param RollCount
	 *	The count or number of dice to roll.
	 * @param DieSize
	 *	The number of sides of each die.
	 *
	 * @return
	 *	The sum of the dice roll(s).
	 */
	UFUNCTION(BlueprintPure=false, Category="OpenPF2|Dice", DisplayName="Roll Dice and Sum")
	static int32 RollSum(const int32 RollCount, const int32 DieSize);

	/**
	 * Returns the result of a dice roll for the given dice roll expression string.
	 *
	 * Complex roll expressions like "3d6x10" are not supported at this time.
	 *
	 * @param RollExpression
	 *	The description of the roll, in "CdS" format, where "C" represents the count or number of dice to roll, and "S"
	 *	represents the number of sides of each die (the die size). For example, "1d6" represents a single roll of a
	 *	six-sided die, while "2d4" represents rolling two dice having four sides each.
	 *
	* @return
	 *	The result of each dice roll.
	 */
	UFUNCTION(BlueprintPure=false, Category="OpenPF2|Dice", DisplayName="Roll Dice (Roll Expression)")
	static TArray<int32> RollString(const FName RollExpression);

	/**
	 * Returns the result of a dice roll for the given numeric parameters.
	 *
	 * @param RollCount
	 *	The count or number of dice to roll.
	 * @param DieSize
	 *	The number of sides of each die.
	 *
	 * @return
	 *	The result of each dice roll.
	 */
	UFUNCTION(BlueprintPure=false, Category="OpenPF2|Dice", DisplayName="Roll Dice")
	static TArray<int32> Roll(const int32 RollCount, const int32 DieSize);

	/**
	 * Increases the size of a given dice expression, returning the next dice size up,
	 *
	 * @param RollExpression
	 *	The description of the roll, in "CdS" format, where "C" represents the count or number of dice to roll, and "S"
	 *	represents the number of sides of each die (the die size). For example, "1d6" represents a single roll of a
	 *	six-sided die, while "2d4" represents rolling two dice having four sides each.
	 *
	 * @return
	 *	A roll expression for the next size up (for example, given "1d6" this would return "1d8"; given "2d4", this
	 *	would return "2d6").
	 */
	UFUNCTION(BlueprintPure=false, Category="OpenPF2|Dice", DisplayName="Next Die Size (Roll Expression)")
	static FName NextSizeString(const FName RollExpression);

	/**
	 * Gets the number of sides of the next-largest die from a die having the given number of sides.
	 *
	 * @param DieSize
	 *	The number of sides of the die.
	 *
	 * @return
	 *	The number of sides on the die of the next size up.
	 */
	UFUNCTION(BlueprintPure=false, Category="OpenPF2|Dice", DisplayName="Next Die Size")
	static int32 NextSize(const int32 DieSize);

	/**
	 * Parses the string description of a roll into distinct roll count and die size components.
	 *
	 * @param RollExpression
	 *	The description of the roll, in "CdS" format, where "C" represents the count or number of dice to roll, and "S"
	 *	represents the number of sides of each die (the die size). For example, "1d6" represents a single roll of a
	 *	six-sided die, while "2d4" represents rolling two dice having four sides each.
	 * @param [out] RollCount
	 *	A reference to the variable to receive the number of dice to roll.
	 * @param [out] DieSize
	 *	A reference to the variable to receive the number of sides of each die.
	 *
	 * @return
	 *	Whether the roll expression could be parsed or not.
	 */
	UFUNCTION(BlueprintPure=false, Category="OpenPF2|Dice")
	static UPARAM(DisplayName="Was Parsed") bool ParseRollExpression(const FName RollExpression,
	                                                                 int32&      RollCount,
	                                                                 int32&      DieSize);

	/**
	 * Gets the random seed of the random number generator (RNG) being used to generate dice rolls.
	 *
	 * @return
	 *	The current random seed.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Dice")
	static int32 GetRandomSeed();

	/**
	 * Sets the random seed of the random number generator (RNG) being used to generate dice rolls.
	 *
	 * This is intended for tests to use to manipulate dice rolls to create reproducible test cases. As such, you only
	 * need to call this method if you intend to manipulate the random seed; otherwise, the dice library will
	 * automatically generate a random seed based on the current system time during the first dice roll of the current
	 * session.
	 *
	 * @param Seed
	 *	The random seed to set.
	 */
	UFUNCTION(BlueprintPure=false, Category="OpenPF2|Dice")
	static void SetRandomSeed(const int32 Seed);

protected:
	// =================================================================================================================
	// Protected Static Methods
	// =================================================================================================================
	/**
	 * Returns a reference to the RNG for dice rolls.
	 *
	 * @return
	 *	The pre-initialized RNG for dice rolls.
	 */
	[[nodiscard]] FORCEINLINE static FRandomStream& GetDiceRng()
	{
		check(DiceRng.GetInitialSeed() != 0);

		return DiceRng;
	}
};
