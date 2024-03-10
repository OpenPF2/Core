// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
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

#include <GameplayEffectExecutionCalculation.h>

#include "Abilities/PF2DegreeOfSuccess.h"

#include "CharacterStats/PF2CharacterAbilitySystemInterface.h"

#include "Items/Weapons/PF2WeaponInterface.h"

#include "PF2RollWeaponAttackExecution.generated.h"

/**
 * Gameplay effect execution calculation for attempting a weapon attack and calculating appropriate damage amount(s).
 *
 * After invocation, the degree of success for the attack is available in the
 * @code TmpAttackDegreeOfSuccessProperty @endcode transient attack attribute, even if the attack was unsuccessful
 * (i.e., a failure or critical failure).
 *
 * This execution does not directly inflict damage on the target. Instead, on a successful attack, this execution
 * calculates damage and applies it to the transient attack attribute that corresponds to the primary damage type for
 * the weapon the source has equipped. If there is additional damage that the weapon should inflict (e.g., from a rune),
 * that can be handled by separate GEs that apply buffs and additional damage to the desired transient attack
 * attributes on the source.
 *
 * Once all the damage from the current attack has been calculated (including bonuses, penalties, runes, etc.), damage
 * can be inflicted on each target through the PF2ApplyDamageFromSourceExecution, which factors in each target's
 * resistances to each type of damage being applied and then applies the total resulting damage to the transient
 * incoming damage property on the target.
 *
 * Modifiers in the output of an execution can only affect attributes on the GE target, so this execution is designed to
 * be used only in a GE that is being applied to the SOURCE of an attack (i.e., the GE should be applied to the
 * character who is attacking, as part of the "Source Gameplay Effects" on the weapon).
 */
UCLASS()
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2RollWeaponAttackExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Static Methods
	// =================================================================================================================
	/**
	 * Attempts an an attack from the source character to the target character.
	 *
	 * An attack roll is performed to determine if the attack against the target is successful. If it is successful, a
	 * damage roll is performed to determine how much damage is inflicted. The degree of success for the attack roll
	 * and the amount of damage inflicted are applied to corresponding transient attributes on the attack attribute set
	 * of the source.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 446, "Attack Rolls":
	 * "When you use a Strike action or make a spell attack, you attempt a check called an attack roll. Attack rolls
	 * take a variety of forms and are often highly variable based on the weapon you are using for the attack..."
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Damage Rolls":
	 * "When the result of your attack roll with a weapon or unarmed attack equals or exceeds your target’s AC, you hit
	 * your target! Roll the weapon or unarmed attack’s damage die and add the relevant modifiers, bonuses, and
	 * penalties to determine the amount of damage you deal."
	 *
	 * @param ExecutionParams
	 *	The context of the gameplay effect calculation that is being executed.
	 * @param Weapon
	 *	The weapon with which the attack is being attempted.
	 * @param SourceAscIntf
	 *	The Ability System Component of the character attempting the attack.
	 * @param TargetAscIntf
	 *	The Ability System Component of the character receiving the attack.
	 * @param [out] OutExecutionOutput
	 *	A reference to the output of the execution. Modifiers will be added to this struct to perform updates to
	 *	transient attack attributes.
	 */
	static void AttemptAttack(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                          const IPF2WeaponInterface*                      Weapon,
	                          const IPF2CharacterAbilitySystemInterface*      SourceAscIntf,
	                          const IPF2CharacterAbilitySystemInterface*      TargetAscIntf,
	                          FGameplayEffectCustomExecutionOutput&           OutExecutionOutput);

	/**
	 * Calculates the attack roll, determining if an attack was successful (it hit its target) as well as its degree.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Attack Rolls":
	 * "When making an attack roll, determine the result by rolling 1d20 and adding your attack modifier for the weapon
	 * or unarmed attack you’re using. Modifiers for melee and ranged attacks are calculated differently.
	 *
	 * Melee attack modifier = Strength modifier (or optionally Dexterity for a finesse weapon) + proficiency bonus +
	 * other bonuses + penalties
	 *
	 * Ranged attack modifier = Dexterity modifier + proficiency bonus + other bonuses + penalties
	 *
	 * Bonuses, and penalties apply to these rolls just like with other types of checks. Weapons with potency runes add
	 * an item bonus to your attack rolls."
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 4: Determine the Degree of Success and Effect":
	 * "Many times, it’s important to determine not only if you succeed or fail, but also how spectacularly you succeed
	 * or fail. Exceptional results—either good or bad—can cause you to critically succeed at or critically fail a
	 * check.
	 *
	 * You critically succeed at a check when the check’s result meets or exceeds the DC by 10 or more. If the check is
	 * an attack roll, this is sometimes called a critical hit. You can also critically fail a check. The rules for
	 * critical failure—sometimes called a fumble—are the same as those for a critical success, but in the other
	 * direction: if you fail a check by 10 or more, that’s a critical failure.
	 *
	 * If you rolled a 20 on the die (a “natural 20”), your result is one degree of success better than it would be by
	 * numbers alone. If you roll a 1 on the d20 (a “natural 1”), your result is one degree worse."
	 *
	 * @param ExecutionParams
	 *	The context of the gameplay effect calculation that is being executed.
	 * @param EvaluationParameters
	 *	Context about the source and target to pass in when obtaining captured attribute values.
	 * @param Weapon
	 *	The weapon with which the attack is being attempted.
	 * @param SourceAsc
	 *	The Ability System Component of the character attempting the attack.
	 * @param TargetArmorClass
	 *	The armor class of the attack target. This represents the difficulty class to check against the attack roll.
	 *
	 * @return
	 *	The outcome of the attack roll for the weapon.
	 */
	static EPF2DegreeOfSuccess PerformAttackRoll(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                             const FAggregatorEvaluateParameters&            EvaluationParameters,
	                                             const IPF2WeaponInterface*                      Weapon,
	                                             const IPF2CharacterAbilitySystemInterface*      SourceAsc,
	                                             const float                                     TargetArmorClass);

	/**
	 * Calculates the damage roll, which determines how much of an effect an attack has on the target.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Damage Rolls":
	 * "When the result of your attack roll with a weapon or unarmed attack equals or exceeds your target’s AC, you hit
	 * your target! Roll the weapon or unarmed attack’s damage die and add the relevant modifiers, bonuses, and
	 * penalties to determine the amount of damage you deal. Calculate a damage roll as follows.
	 *
	 * Melee damage roll = damage die of weapon or unarmed attack + Strength modifier + bonuses + penalties
	 * Ranged damage roll = damage die of weapon + Strength modifier for thrown weapons + bonuses + penalties"
	 *
	 * @param ExecutionParams
	 *	The context of the gameplay effect calculation that is being executed.
	 * @param EvaluationParameters
	 *	Context about the source and target to pass in when obtaining captured attribute values.
	 * @param Weapon
	 *	The weapon with which the attack is being attempted.
	 *
	 * @return
	 *	The calculated damage roll for the weapon.
	 */
	static float PerformDamageRoll(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                               const FAggregatorEvaluateParameters&            EvaluationParameters,
	                               const IPF2WeaponInterface*                      Weapon);

	/**
	 * Gets the armor class (AC) value of the target of an attack.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Damage Rolls":
	 * "When the result of your attack roll with a weapon or unarmed attack equals or exceeds your target’s AC, you hit
	 * your target!"
	 *
	 * @param ExecutionParams
	 *	The context of the gameplay effect calculation that is being executed.
	 * @param EvaluationParameters
	 *	Context about the source and target to pass in when obtaining captured attribute values.
	 *
	 * @return
	 *	The armor class of the target of the attack.
	 */
	static float GetTargetArmorClass(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                 const FAggregatorEvaluateParameters&            EvaluationParameters);

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor.
	 */
	UPF2RollWeaponAttackExecution();

	// =================================================================================================================
	// Public Methods - UGameplayEffectExecutionCalculation Implementation
	// =================================================================================================================
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
