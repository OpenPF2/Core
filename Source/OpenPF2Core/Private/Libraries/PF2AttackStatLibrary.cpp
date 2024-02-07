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

#include "Libraries/PF2AttackStatLibrary.h"

#include "Calculations/PF2TemlCalculation.h"

#include "Libraries/PF2DiceLibrary.h"

#include "Utilities/PF2EnumUtilities.h"

EPF2DegreeOfSuccess UPF2AttackStatLibrary::CalculateAttackRoll(const int32                  CharacterLevel,
                                                               const FGameplayTagContainer& CharacterTags,
                                                               const float                  AttackAbilityModifier,
                                                               const float                  MultipleAttackPenalty,
                                                               const FGameplayTagContainer& ProficiencyTagPrefixes,
                                                               const float                  TargetArmorClass,
                                                               const int                    RollCount,
                                                               const int                    RollSize)
{
	EPF2DegreeOfSuccess Result;
	const int32         DiceRoll               = UPF2DiceLibrary::RollSum(RollCount, RollSize);
	const bool          bIsNatural20           = (DiceRoll == RollSize);
	float               WeaponProficiencyBonus = 0,
	                    AttackRoll;

	if (MultipleAttackPenalty > 0)
	{
		UE_LOG(
			LogPf2CoreStats,
			Error,
			TEXT("CalculateAttackRoll(): The Multiple Attack Penalty should be negative or zero (was given '%f')."),
			MultipleAttackPenalty
		);
	}

	// "When attempting a check that involves something you have some training in, you will also add your proficiency
	// bonus. This bonus depends on your proficiency rank [...] if you have multiple bonuses of the same type, you can
	// use only the highest bonus on a given roll—in other words, they don’t "stack."
	//
	// Source: Pathfinder 2E Core Rulebook, Chapter 9, page 444, "Step 1: Roll D20 and Identify The Modifiers, Bonuses,
	// and Penalties That Apply".
	for (const FGameplayTag& ProficiencyTagPrefix : ProficiencyTagPrefixes)
	{
		FPF2TemlCalculation Calculation(ProficiencyTagPrefix, &CharacterTags, CharacterLevel);

		WeaponProficiencyBonus = FMath::Max(WeaponProficiencyBonus, Calculation.GetValue());
	}

	// Melee attack modifier = Strength modifier (or optionally Dexterity for a finesse weapon) + proficiency bonus +
	// other bonuses + penalties
	//
	// Ranged attack modifier = Dexterity modifier + proficiency bonus + other bonuses + penalties
	//
	// Source: Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Attack Rolls".
	AttackRoll = DiceRoll + AttackAbilityModifier + WeaponProficiencyBonus + MultipleAttackPenalty;
	Result     = DetermineCheckDegreeOfSuccess(AttackRoll, TargetArmorClass);

	// If you rolled a 20 on the die (a “natural 20”), your result is one degree of success better than it would be by
	// numbers alone."
	if (bIsNatural20 && (Result != EPF2DegreeOfSuccess::CriticalSuccess))
	{
		Result = IncreaseDegreeOfSuccess(Result);
	}

	UE_LOG(
		LogPf2CoreStats,
		VeryVerbose,
		TEXT("Attack Roll (%d%s) + Attack Ability Modifier (%f) + Weapon Proficiency Bonus (%f) + Multiple Attack Penalty (%f) = %f vs. AC %f: %s."),
		DiceRoll,
		bIsNatural20 ? TEXT(" [CRIT]") : TEXT(""),
		AttackAbilityModifier,
		WeaponProficiencyBonus,
		MultipleAttackPenalty,
		AttackRoll,
		TargetArmorClass,
		*(PF2EnumUtilities::ToString(Result))
	);

	return Result;
}

EPF2DegreeOfSuccess UPF2AttackStatLibrary::CalculateFlatCheck(const float DifficultyClass)
{
	int32 UnusedDieRoll;

	return CalculateFlatCheck(DifficultyClass, UnusedDieRoll);
}

EPF2DegreeOfSuccess UPF2AttackStatLibrary::CalculateFlatCheck(const float DifficultyClass, int32& DieRoll)
{
	EPF2DegreeOfSuccess Result;

	DieRoll = UPF2DiceLibrary::RollSum(1, 20);
	Result  = DetermineCheckDegreeOfSuccess(DieRoll, DifficultyClass);

	// "If you rolled a 20 on the die (a 'natural 20'), your result is one degree of success better than it would be by
	// numbers alone. If you roll a 1 on the d20 (a 'natural 1'), your result is one degree worse. This means that a
	// natural 20 usually results in a critical success and natural 1 usually results in a critical failure."
	//
	// Source: Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 4: Determine the Degree of Success and Effect"
	if ((DieRoll == 20) && (Result != EPF2DegreeOfSuccess::CriticalSuccess))
	{
		Result = IncreaseDegreeOfSuccess(Result);
	}
	else if ((DieRoll == 1) && (Result != EPF2DegreeOfSuccess::CriticalFailure))
	{
		Result = DecreaseDegreeOfSuccess(Result);
	}

	UE_LOG(
		LogPf2CoreStats,
		VeryVerbose,
		TEXT("Flat Check Die Roll (1d20) = %d vs. DC %f: %s."),
		DieRoll,
		DifficultyClass,
		*(PF2EnumUtilities::ToString(Result))
	);

	return Result;
}

EPF2DegreeOfSuccess UPF2AttackStatLibrary::DegreeOfSuccessStatToEnum(const float DegreeOfSuccessValue)
{
	return PF2EnumUtilities::EnumValueOf(static_cast<int8>(DegreeOfSuccessValue), EPF2DegreeOfSuccess::None);
}

float UPF2AttackStatLibrary::DegreeOfSuccessStatFromEnum(const EPF2DegreeOfSuccess DegreeOfSuccess)
{
	return static_cast<float>(DegreeOfSuccess);
}

float UPF2AttackStatLibrary::CalculateDamageRoll(const FName DamageDie, const float DamageAbilityModifier)
{
	int RollCount = 0,
	    RollSize  = 0;

	UPF2DiceLibrary::ParseRollExpression(DamageDie, RollCount, RollSize);

	return CalculateDamageRoll(RollCount, RollSize, DamageAbilityModifier);
}

float UPF2AttackStatLibrary::CalculateDamageRoll(const int RollCount,
                                                 const int RollSize,
                                                 const float DamageAbilityModifier)
{
	float       DamageRoll;
	const int32 DamageDieRoll = UPF2DiceLibrary::RollSum(RollCount, RollSize);

	// Melee damage roll  = damage die of weapon or unarmed attack + Strength modifier + bonuses + penalties
	// Ranged damage roll = damage die of weapon + Strength modifier for thrown weapons + bonuses + penalties
	//
	// Source: Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Damage Rolls".
	DamageRoll = DamageDieRoll + DamageAbilityModifier;

	UE_LOG(
		LogPf2CoreStats,
		VeryVerbose,
		TEXT("Damage Die Roll (%dd%d: %d) + Damage Ability Modifier (%f) = %f."),
		RollCount,
		RollSize,
		DamageDieRoll,
		DamageAbilityModifier,
		DamageRoll
	);

	return DamageRoll;
}

int32 UPF2AttackStatLibrary::CalculateRecoveryCheck(const uint8 DyingConditionLevel)
{
	// "... DC equal to 10 + your current dying value ..."
	//
	// Source: Pathfinder 2E Core Rulebook, Chapter 9, page 459, "Recovery Checks".
	int8                      Result;
	const int32               TargetDC    = 10 + DyingConditionLevel;
	const EPF2DegreeOfSuccess CheckResult = CalculateFlatCheck(TargetDC);

	UE_LOG(
		LogPf2CoreStats,
		VeryVerbose,
		TEXT("Recovery check result against Dying condition level %d: %s."),
		DyingConditionLevel,
		*(PF2EnumUtilities::ToString(CheckResult))
	);

	// "The effects of this check are as follows.
	// - Critical Success: Your dying value is reduced by 2.
	// - Success: Your dying value is reduced by 1.
	// - Failure: Your dying value increases by 1.
	// - Critical Failure: Your dying value increases by 2."
	//
	// Source: Pathfinder 2E Core Rulebook, Chapter 9, page 459, "Recovery Checks".
	switch (CheckResult)
	{
		case EPF2DegreeOfSuccess::CriticalSuccess:
			Result = -2;
			break;

		case EPF2DegreeOfSuccess::Success:
			Result = -1;
			break;

		default:
		case EPF2DegreeOfSuccess::Failure:
			Result = +1;
			break;

		case EPF2DegreeOfSuccess::CriticalFailure:
			Result = +2;
			break;
	}

	return Result;
}

float UPF2AttackStatLibrary::CalculateRangePenalty(const float WeaponRangeIncrementCentimeters,
                                                   const float DistanceCentimeters)
{
	// From the Pathfinder 2E Core Rulebook, page 279, "Range":
	// "Ranged and thrown weapons have a range increment. Attacks with these weapons work normally up to that distance.
	// Attack rolls beyond a weapon’s range increment take a –2 penalty for each additional multiple of that increment
	// between you and the target."
	//
	// From the Pathfinder 2E Core Rulebook, Chapter 9, page 446, "Range Penalty":
	// "Ranged and thrown weapons each have a listed range increment, and attacks with them grow less accurate against
	// targets farther away [...] As long as your target is at or within the listed range increment, also called the
	// first range increment, you take no penalty to the attack roll. If you’re attacking beyond that range increment,
	// you take a –2 penalty for each additional increment beyond the first. You can attempt to attack with a ranged
	// weapon or thrown weapon up to six range increments away, but the farther away you are, the harder it is to hit
	// your target."
	const float RangeIncrement =
		FMath::Max(0, FMath::FloorToFloat((DistanceCentimeters - 1.0f) / WeaponRangeIncrementCentimeters));

	float Penalty = RangeIncrement * RangePenaltyPerIncrement;

	// These are both negative values, so "less than" is equivalent to "greater than" for positive values.
	if (Penalty < MaxRangePenalty)
	{
		Penalty = -UE_MAX_FLT;
	}

	return Penalty;
}

float UPF2AttackStatLibrary::CalculateMaximumRange(const float WeaponRangeIncrementCentimeters)
{
	return WeaponRangeIncrementCentimeters * MaxRangeIncrement;
}

bool UPF2AttackStatLibrary::IsWithinRange(const float WeaponRangeIncrementCentimeters, const float DistanceCentimeters)
{
	const float MaximumRange = CalculateMaximumRange(WeaponRangeIncrementCentimeters);
	const bool  bInRange     = (DistanceCentimeters <= MaximumRange);

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("IsWithinRange(%f,%f): %s (Max Range = %f)"),
		WeaponRangeIncrementCentimeters,
		DistanceCentimeters,
		bInRange ? TEXT("true") : TEXT("false"),
		MaximumRange
	);

	return bInRange;
}

EPF2DegreeOfSuccess UPF2AttackStatLibrary::DetermineCheckDegreeOfSuccess(const float Value, const float DifficultyClass)
{
	EPF2DegreeOfSuccess Result;

	// "You critically succeed at a check when the check’s result meets or exceeds the DC by 10 or more. [...] The rules
	// for critical failure [...] are the same [...], but in the other direction: if you fail a check by 10 or more,
	// that’s a critical failure."
	//
	// Source: Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 4: Determine the Degree of Success and Effect"
	if (Value >= DifficultyClass + 10.0f)
	{
		Result = EPF2DegreeOfSuccess::CriticalSuccess;
	}
	else if (Value >= DifficultyClass)
	{
		Result = EPF2DegreeOfSuccess::Success;
	}
	else if (Value <= DifficultyClass - 10.0f)
	{
		Result = EPF2DegreeOfSuccess::CriticalFailure;
	}
	else
	{
		Result = EPF2DegreeOfSuccess::Failure;
	}

	return Result;
}

EPF2DegreeOfSuccess UPF2AttackStatLibrary::IncreaseDegreeOfSuccess(const EPF2DegreeOfSuccess Value)
{
	const int32 MaxAllowedResult = static_cast<int32>(EPF2DegreeOfSuccess::CriticalSuccess),
	            NewValue         = static_cast<int32>(Value) + 1,
	            ClampedNewValue  = FMath::Min(NewValue, MaxAllowedResult);

	if (NewValue != ClampedNewValue)
	{
		UE_LOG(
			LogPf2CoreStats,
			Warning,
			TEXT("Attempted to increment degree of success above maximum allowed value (%s)."),
			*(PF2EnumUtilities::ToString(EPF2DegreeOfSuccess::CriticalSuccess))
		);
	}

	return static_cast<EPF2DegreeOfSuccess>(ClampedNewValue);
}

EPF2DegreeOfSuccess UPF2AttackStatLibrary::DecreaseDegreeOfSuccess(const EPF2DegreeOfSuccess Value)
{
	const int32 MinAllowedResult = static_cast<int32>(EPF2DegreeOfSuccess::CriticalFailure),
	            NewValue         = static_cast<int32>(Value) - 1,
	            ClampedNewValue  = FMath::Max(NewValue, MinAllowedResult);

	if (NewValue != ClampedNewValue)
	{
		UE_LOG(
			LogPf2CoreStats,
			Warning,
			TEXT("Attempted to decrement degree of success below minimum allowed value (%s)."),
			*(PF2EnumUtilities::ToString(EPF2DegreeOfSuccess::CriticalFailure))
		);
	}

	return static_cast<EPF2DegreeOfSuccess>(ClampedNewValue);
}
