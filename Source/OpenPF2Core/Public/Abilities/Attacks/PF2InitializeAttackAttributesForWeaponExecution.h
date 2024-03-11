// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayEffectExecutionCalculation.h>

#include "PF2InitializeAttackAttributesForWeaponExecution.generated.h"

/**
 * An execution calculation that should appear at the start of every weapon attack gameplay effect.
 *
 * This execution initializes the transient attack attribute set of the GE target with stats from the weapon that is
 * being used for an attack. This allows subsequent gameplay effects to be applied for bonuses and penalties (e.g.,
 * runes, magical spells, etc.) that affect the weapon stats before they're actually used to calculate the attack.
 *
 * Modifiers in the output of an execution can only affect attributes on the GE target, so this execution is designed to
 * be used only in a GE that is being applied to the SOURCE of an attack (i.e., the GE should be applied to the
 * character who is attacking, as part of the "Source Gameplay Effects" on the weapon).
 */
UCLASS()
class OPENPF2CORE_API UPF2InitializeAttackAttributesForWeaponExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Static Methods
	// =================================================================================================================
	/**
	 * Initializes all transient attack attributes on the source, to prepare for attack calculations.
	 *
	 * Attributes related to the attack degree of success and damage amounts are zeroed-out, while the damage roll count
	 * and damage roll die size are set to the specified values.
	 *
	 * @param WeaponRollCount
	 *	The number of dice to roll for damage with the weapon the source has equipped.
	 * @param WeaponDieSize
	 *	The size die to roll for damage with the weapon the source has equipped.
	 * @param [out] OutExecutionOutput
	 *	A reference to the output of the execution. Modifiers will be added to this struct to perform updates to
	 *	transient attack attributes.
	 */
	static void InitializeAttackStats(const int32                           WeaponRollCount,
	                                  const int32                           WeaponDieSize,
	                                  FGameplayEffectCustomExecutionOutput& OutExecutionOutput);

public:
	// =================================================================================================================
	// Public Methods - UGameplayEffectExecutionCalculation Implementation
	// =================================================================================================================
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
