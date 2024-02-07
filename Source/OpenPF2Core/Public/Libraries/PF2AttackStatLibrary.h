// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//   - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//   - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//   - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
//
// Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
// as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
// file other than the material designated as Open Game Content may be reproduced in any form without written
// permission.

#pragma once

#include <GameplayTagContainer.h>

#include <Kismet/BlueprintFunctionLibrary.h>

#include "Calculations/PF2DegreeOfSuccess.h"

#include "PF2AttackStatLibrary.generated.h"

/**
 * Function library for standard OpenPF2 attack statistic and distance calculations.
 */
UCLASS()
class UPF2AttackStatLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:
	/**
	 * The default maximum distance (in centimeters) at which movement to a location is considered acceptable.
	 *
	 * For example, if Character A is moving into range of Character B to make a melee attack on Character B, and the
	 * reach of Character A is 1.5 meters (the standard distance for reach in OpenPF2), then Character A and Character B
	 * must be no further than 3.5 meters away from each other (reach + acceptance radius => 1.5 meters + 2.0 meters)
	 * for the attack to proceed.
	 */
	static constexpr float DefaultMovementAcceptanceRadiusCentimeters = 2.0f;

	/**
	 * The maximum multiple of the range of a ranged weapon within which the weapon still has a chance to hit a target.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 6, page 279, "Range":
	 * "Attacks beyond the sixth range increment are impossible."
	 */
	static constexpr float MaxRangeIncrement = 6.0f;

	/**
	 * The amount of additional penalty to add to ranged attack rolls for each additional increment of the weapon range.
	 */
	static constexpr float RangePenaltyPerIncrement = -2.0f;

	/**
	 * The maximum range penalty a ranged weapon can have while still having a chance to hit a target.
	 *
	 * We subtract 1.0 because the range penalty starts at 0 for all distances less than the range increment of the
	 * weapon.
	 */
	static constexpr float MaxRangePenalty = (MaxRangeIncrement - 1.0f) * RangePenaltyPerIncrement;

public:
	/**
	 * Gets the default maximum distance (in centimeters) at which movement to a location is considered acceptable.
	 *
	 * @return
	 *	The default acceptance radius, in centimeters.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static UPARAM(DisplayName="Acceptance radius (cm)") FORCEINLINE float GetDefaultMovementAcceptanceRadius()
	{
		return DefaultMovementAcceptanceRadiusCentimeters;
	}

	/**
	 * Calculates the attack roll, which determines if an attack was successful (it hit its target).
	 *
	 * The returned result is based on a random dice roll, so there is no guarantee of getting the same return value
	 * given the same combination of character level, character tags, and attack ability modifier.
	 *
	 * "When making an attack roll, determine the result by rolling 1d20 and adding your attack modifier for the weapon
	 * or unarmed attack you’re using. Modifiers for melee and ranged attacks are calculated differently.
	 *
	 * Melee attack modifier = Strength modifier (or optionally Dexterity for a finesse weapon) + proficiency bonus +
	 * other bonuses + penalties
	 * Ranged attack modifier = Dexterity modifier + proficiency bonus + other bonuses + penalties
	 *
	 * Bonuses, and penalties apply to these rolls just like with other types of checks. Weapons with potency runes add
	 * an item bonus to your attack rolls."
	 *
	 * Source: Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Attack Rolls".
	 *
	 * @see EPF2DegreeOfSuccess
	 *
	 * @param CharacterLevel
	 *	The level of the character.
	 * @param CharacterTags
	 *	The tags applied to the character.
	 * @param AttackAbilityModifier
	 *	The value of the character ability modifier to add to *attack* rolls, based on the type of weapon being used:
	 *	- For melee:
	 *	  - Regular weapons: the value of the "Strength" modifier.
	 *	  - Finesse weapons: the value of the "Dexterity" modifier.
	 *	- For ranged: the value of the the "Dexterity" modifier.
	 * @param MultipleAttackPenalty
	 *	The penalty to apply for attempting multiple attacks during the same turn. This should be a negative number.
	 * @param ProficiencyTagPrefixes
	 *	The root/parent tag of each set of tags that represent a character's TEML proficiencies with the weapon.
	 * @param TargetArmorClass
	 *	The armor class of the attack target. This represents the difficulty class to check against the attack roll.
	 * @param RollCount
	 *	An optional override on the number of dice to use in the attack roll. Defaults to 1, according to standard
	 *	Pathfinder 2e rules.
	 * @param RollSize
	 *	An optional override on the size of each die to use in the attack roll. Defaults to 20, according to standard
	 *	Pathfinder 2e rules.
	 *
	 * @return
	 *	The calculated, random attack roll.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Attack Stats")
	static EPF2DegreeOfSuccess CalculateAttackRoll(const int32                  CharacterLevel,
	                                               const FGameplayTagContainer& CharacterTags,
	                                               const float                  AttackAbilityModifier,
	                                               const float                  MultipleAttackPenalty,
	                                               const FGameplayTagContainer& ProficiencyTagPrefixes,
	                                               const float                  TargetArmorClass,
	                                               const int                    RollCount = 1,
	                                               const int                    RollSize  = 20);

	/**
	 * Calculates a flat check against a difficulty class.
	 *
	 * The returned result is based on a random dice roll, so there is no guarantee of getting the same return value
	 * given the same difficulty class.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 450, "Flat Checks":
	 * "When the chance something will happen or fail to happen is based purely on chance, you’ll attempt a flat check.
	 * A flat check never includes any modifiers, bonuses, or penalties—you just roll a d20 and compare the result on
	 * the die to the DC. [...] In the rare circumstance that a flat check has a DC of 1 or lower, skip rolling; you
	 * automatically succeed. Conversely, if one ever has a DC of 21 or higher, you automatically fail."
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 4: Determine the Degree of Success and Effect":
	 * "If you rolled a 20 on the die (a 'natural 20'), your result is one degree of success better than it would be by
	 * numbers alone. If you roll a 1 on the d20 (a 'natural 1'), your result is one degree worse. This means that a
	 * natural 20 usually results in a critical success and natural 1 usually results in a critical failure."
	 *
	 * @param [in] DifficultyClass
	 *	The difficulty class to check the value against.
	 *
	 * @return
	 *	The calculated result of the check.
	 */
	static EPF2DegreeOfSuccess CalculateFlatCheck(const float DifficultyClass);

	/**
	 * Calculates a flat check against a difficulty class.
	 *
	 * The returned result is based on a random dice roll, so there is no guarantee of getting the same return value
	 * given the same difficulty class.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 450, "Flat Checks":
	 * "When the chance something will happen or fail to happen is based purely on chance, you’ll attempt a flat check.
	 * A flat check never includes any modifiers, bonuses, or penalties—you just roll a d20 and compare the result on
	 * the die to the DC. [...] In the rare circumstance that a flat check has a DC of 1 or lower, skip rolling; you
	 * automatically succeed. Conversely, if one ever has a DC of 21 or higher, you automatically fail."
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 4: Determine the Degree of Success and Effect":
	 * "If you rolled a 20 on the die (a 'natural 20'), your result is one degree of success better than it would be by
	 * numbers alone. If you roll a 1 on the d20 (a 'natural 1'), your result is one degree worse. This means that a
	 * natural 20 usually results in a critical success and natural 1 usually results in a critical failure."
	 *
	 * @param [in] DifficultyClass
	 *	The difficulty class to check the value against.
	 * @param [out] DieRoll
	 *	An optional variable to receive the raw die roll (the value before the degree of success has been calculated).
	 *
	 * @return
	 *	The calculated result of the check.
	 */
	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="DieRoll"), Category="OpenPF2|Attack Stats")
	static EPF2DegreeOfSuccess CalculateFlatCheck(const float DifficultyClass, int32& DieRoll);

	/**
	 * Determines the outcome of a check of a value against a Difficulty Class (DC).
	 *
	 * Given the same combination of value and DC, the returned value is always the same.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 4: Determine the Degree of Success and Effect":
	 * "You critically succeed at a check when the check’s result meets or exceeds the DC by 10 or more. [...] The rules
	 * for critical failure [...] are the same [...], but in the other direction: if you fail a check by 10 or more,
	 * that’s a critical failure."
	 *
	 * @see EPF2DegreeOfSuccess
	 *
	 * @param Value
	 *	The value to check.
	 * @param DifficultyClass
	 *	The difficulty class to check the value against.
	 *
	 * @return
	 *	The calculated result of the check.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static EPF2DegreeOfSuccess DetermineCheckDegreeOfSuccess(const float Value, const float DifficultyClass);

	/**
	 * Upgrades a check result to one degree of success better, up to a maximum of "critical success".
	 *
	 * @param Value
	 *	The check result to upgrade.
	 *
	 * @return
	 *	Either:
	 *	  - If CheckResult is not EPF2DegreeOfSuccess::CriticalSuccess: The next-highest check result value.
	 *	  - If CheckResult is EPF2DegreeOfSuccess::CriticalSuccess: No change (EPF2DegreeOfSuccess::CriticalSuccess).
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static EPF2DegreeOfSuccess IncreaseDegreeOfSuccess(const EPF2DegreeOfSuccess Value);

	/**
	 * Downgrades a check result to one degree of success worse, down to a minimum of "critical failure".
	 *
	 * @param Value
	 *	The check result to upgrade.
	 *
	 * @return
	 *	Either:
	 *	  - If CheckResult is not EPF2DegreeOfSuccess::CriticalFailure: The next-lowest check result value.
	 *	  - If CheckResult is EPF2DegreeOfSuccess::CriticalFailure: No change (EPF2DegreeOfSuccess::CriticalFailure).
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static EPF2DegreeOfSuccess DecreaseDegreeOfSuccess(const EPF2DegreeOfSuccess Value);

	/**
	 * Determines if the specified check result is a success or critical success.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 3: Compare the Result to the DC":
	 * "[...] if your result is equal to or greater than the DC, you succeed!"
	 *
	 * @param CheckResult
	 *	The result to check.
	 *
	 * @return
	 *	- true if the result represents a successful outcome (success or critical success).
	 *	- false if the result represents a failed outcome (failure or a critical failure/fumble).
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static FORCEINLINE bool IsSuccess(const EPF2DegreeOfSuccess CheckResult)
	{
		return ((CheckResult == EPF2DegreeOfSuccess::Success) || (CheckResult == EPF2DegreeOfSuccess::CriticalSuccess));
	}

	/**
	 * Determines if the specified check result is a failure or critical failure.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 3: Compare the Result to the DC":
	 * "If [you] roll anything less than the DC, you fail."
	 *
	 * @param CheckResult
	 *	The result to check.
	 *
	 * @return
	 *	- true if the result represents a failed outcome (failure or a critical failure/fumble).
	 *	- false if the result represents a successful outcome (success or critical success).
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static FORCEINLINE bool IsFailure(const EPF2DegreeOfSuccess CheckResult)
	{
		return ((CheckResult == EPF2DegreeOfSuccess::Failure) || (CheckResult == EPF2DegreeOfSuccess::CriticalFailure));
	}

	/**
	 * Converts a "degree of success" that has been encoded in the value on an ASC attribute set into an enum value.
	 *
	 * @param DegreeOfSuccessValue
	 *	The degree of success value that was encoded as a float. Though a float, all decimal points must be equal to
	 *	zero (i.e., it must be an integer that is represented in a float), and the integer value of this parameter
	 *	must be equal to one of the value degree-of-success enum values.
	 *
	 * @return
	 *	The decoded degree of success.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static EPF2DegreeOfSuccess DegreeOfSuccessStatToEnum(const float DegreeOfSuccessValue);

	/**
	 * Converts a "degree of success" enum value into a value for an ASC attribute set.
	 *
	 * @param DegreeOfSuccess
	 *   The degree of success enum value to encode as a float.
	 *
	 * @return
	 *	The encoded degree of success.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static float DegreeOfSuccessStatFromEnum(const EPF2DegreeOfSuccess DegreeOfSuccess);

	/**
	 * Calculates the damage roll, which determines how much of an effect an attack has on the target.
	 *
	 * The returned result is based on a random dice roll, so there is no guarantee of getting the same return value
	 * given the same combination of damage die and damage ability modifier.
	 *
	 * "When the result of your attack roll with a weapon or unarmed attack equals or exceeds your target’s AC, you hit
	 * your target! Roll the weapon or unarmed attack’s damage die and add the relevant modifiers, bonuses, and
	 * penalties to determine the amount of damage you deal. Calculate a damage roll as follows.
	 *
	 * Melee damage roll  = damage die of weapon or unarmed attack + Strength modifier + bonuses + penalties
	 * Ranged damage roll = damage die of weapon + Strength modifier for thrown weapons + bonuses + penalties"
	 *
	 * Source: Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Damage Rolls".
	 *
	 * @param DamageDie
	 *	The damage die of the weapon or unarmed attack.
	 * @param DamageAbilityModifier
	 *	The modifier for the type of damage (e.g., Strength modifier or Strength modifier for thrown weapons).
	 *
	 * @return
	 *	The calculated, random damage roll value.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Attack Stats")
	static float CalculateDamageRoll(const FName DamageDie,
	                                 const float DamageAbilityModifier);

	/**
	 * Calculates the damage roll, which determines how much of an effect an attack has on the target.
	 *
	 * The returned result is based on a random dice roll, so there is no guarantee of getting the same return value
	 * given the same combination of roll count, roll size, and damage ability modifier.
	 *
	 * "When the result of your attack roll with a weapon or unarmed attack equals or exceeds your target’s AC, you hit
	 * your target! Roll the weapon or unarmed attack’s damage die and add the relevant modifiers, bonuses, and
	 * penalties to determine the amount of damage you deal. Calculate a damage roll as follows.
	 *
	 * Melee damage roll  = damage die of weapon or unarmed attack + Strength modifier + bonuses + penalties
	 * Ranged damage roll = damage die of weapon + Strength modifier for thrown weapons + bonuses + penalties"
	 *
	 * Source: Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Damage Rolls".
	 *
	 * @param RollCount
	 *	The number of dice to use in the damage roll, as specified by the statistics of the weapon or unarmed attack.
	 * @param RollSize
	 *	The size of each die to use in the damage roll, as specified by the statistics of the weapon or unarmed attack.
	 * @param DamageAbilityModifier
	 *	The modifier for the type of damage (e.g., Strength modifier or Strength modifier for thrown weapons).
	 *
	 * @return
	 *	The calculated, random damage roll value.
	 */
	static float CalculateDamageRoll(const int   RollCount,
	                                 const int   RollSize,
	                                 const float DamageAbilityModifier);

	/**
	 * Performs a recovery flat check and returns the result.
	 *
	 * The returned result is based on a random dice roll, so there is no guarantee of getting the same return value
	 * given the same dying condition level.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 450, "Flat Checks":
	 * "When the chance something will happen or fail to happen is based purely on chance, you’ll attempt a flat check.
	 * A flat check never includes any modifiers, bonuses, or penalties—you just roll a d20 and compare the result on
	 * the die to the DC."
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 459, "Recovery Checks":
	 * "When you’re dying, at the start of each of your turns, you must attempt a flat check with a DC equal to
	 * 10 + your current dying value to see if you get better or worse. This is called a recovery check. The effects of
	 * this check are as follows.
	 * - Critical Success: Your dying value is reduced by 2.
	 * - Success: Your dying value is reduced by 1.
	 * - Failure: Your dying value increases by 1.
	 * - Critical Failure: Your dying value increases by 2."
	 *
	 * @param DyingConditionLevel
	 *	The current level of the dying condition trait.
	 *
	 * @return
	 *	An integer offset representing the effect on the dying value, as a value in the range of -2 (inclusive) to
	 *	2 (inclusive). This is the calculated, random result of performing the recovery check.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Attack Stats")
	static int32 CalculateRecoveryCheck(const uint8 DyingConditionLevel);

	/**
	 * Calculates the penalty at the specified distance from a target for a weapon that has the given range increment.
	 *
	 * Given the same combination of weapon range increment and distance, the returned value is always the same.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 6, page 279, "Range":
	 * "Ranged and thrown weapons have a range increment. Attacks with these weapons work normally up to that distance.
	 * Attack rolls beyond a weapon’s range increment take a –2 penalty for each additional multiple of that increment
	 * between you and the target. Attacks beyond the sixth range increment are impossible."
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 446, "Range Penalty":
	 * "Ranged and thrown weapons each have a listed range increment, and attacks with them grow less accurate against
	 * targets farther away [...] As long as your target is at or within the listed range increment, also called the
	 * first range increment, you take no penalty to the attack roll. If you’re attacking beyond that range increment,
	 * you take a –2 penalty for each additional increment beyond the first. You can attempt to attack with a ranged
	 * weapon or thrown weapon up to six range increments away, but the farther away you are, the harder it is to hit
	 * your target."
	 *
	 * @param WeaponRangeIncrementCentimeters
	 *	The range increment (in centimeters) of the ranged or thrown weapon. This is the distance up to which a ranged
	 *	weapon works "normally"
	 * @param DistanceCentimeters
	 *	The distance (in centimeters) to the target.
	 *
	 * @return
	 *	The range increment penalty for the given range.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static float CalculateRangePenalty(
		UPARAM(DisplayName="Weapon Range Increment (cm)")
		const float WeaponRangeIncrementCentimeters,

		UPARAM(DisplayName="Distance (cm)")
		const float DistanceCentimeters);

	/**
	 * Calculates the maximum range of a weapon that has the given range increment.
	 *
	 * Given the same weapon range increment, the returned value is always the same.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 6, page 279, "Range":
	 * "Ranged and thrown weapons have a range increment. Attacks with these weapons work normally up to that distance.
	 * Attack rolls beyond a weapon’s range increment take a –2 penalty for each additional multiple of that increment
	 * between you and the target. Attacks beyond the sixth range increment are impossible."
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 446, "Range":
	 * "Ranged and thrown weapons each have a listed range increment, and attacks with them grow less accurate against
	 * targets farther away [...] As long as your target is at or within the listed range increment, also called the
	 * first range increment, you take no penalty to the attack roll. If you’re attacking beyond that range increment,
	 * you take a –2 penalty for each additional increment beyond the first. You can attempt to attack with a ranged
	 * weapon or thrown weapon up to six range increments away, but the farther away you are, the harder it is to hit
	 * your target."
	 *
	 * @param WeaponRangeIncrementCentimeters
	 *	The range increment (in centimeters) of the ranged or thrown weapon. This is the distance up to which a ranged
	 *	weapon works "normally"
	 *
	 * @return
	 *	The maximum range (in centimeters) of a weapon with the given range increment.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static float CalculateMaximumRange(
		UPARAM(DisplayName="Weapon Range Increment (cm)")
		const float WeaponRangeIncrementCentimeters);

	/**
	 * Determines if the specified distance is within range of the weapon having the given range increment.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 6, page 279, "Range":
	 * "Ranged and thrown weapons have a range increment. Attacks with these weapons work normally up to that distance.
	 * Attack rolls beyond a weapon’s range increment take a –2 penalty for each additional multiple of that increment
	 * between you and the target. Attacks beyond the sixth range increment are impossible."
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 446, "Range":
	 * "Ranged and thrown weapons each have a listed range increment, and attacks with them grow less accurate against
	 * targets farther away [...] As long as your target is at or within the listed range increment, also called the
	 * first range increment, you take no penalty to the attack roll. If you’re attacking beyond that range increment,
	 * you take a –2 penalty for each additional increment beyond the first. You can attempt to attack with a ranged
	 * weapon or thrown weapon up to six range increments away, but the farther away you are, the harder it is to hit
	 * your target."
	 *
	 * @param WeaponRangeIncrementCentimeters
	 *	The range increment of the ranged or thrown weapon.
	 * @param DistanceCentimeters
	 *	The distance to the target.
	 *
	 * @return
	 *	Whether the specified distance is within range of the weapon.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Attack Stats")
	static bool IsWithinRange(
		UPARAM(DisplayName="Weapon Range Increment (cm)")
		const float WeaponRangeIncrementCentimeters,

		UPARAM(DisplayName="Distance (cm)")
		const float DistanceCentimeters);
};
