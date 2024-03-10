// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayEffectExecutionCalculation.h>

#include "PF2SimpleDamageExecution.generated.h"

/**
 * Gameplay effect execution calculation that applies a fixed amount of damage to a target, factoring in resistance.
 *
 * The amount of damage to inflict and the amount of resistance that the target has to that type of damage are passed-in
 * via "Transient Aggregators" (a.k.a., "Temporary Variables" in the Editor UI). The damage is typically a
 * "set by caller" input to the GE while the resistance value is sourced from a resistance attribute on the target.
 */
UCLASS()
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2SimpleDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Constants
	// =================================================================================================================
	/**
	 * Name of the tag that is used to pass a dynamic damage amount into the calculation.
	 */
	const FName DamageParameterTagName = "GameplayEffect.Parameter.Damage";

	/**
	 * Name of the tag that is used to pass a dynamic resistance amount into the calculation.
	 */
	const FName ResistanceParameterTagName = "GameplayEffect.Parameter.Resistance";

	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The tag for the parameter that is used to pass a dynamic damage amount into the calculation.
	 *
	 * The parameter is typically read from a set-by-caller tag-based value so that GAs can populate damage dynamically,
	 * unless the GE is expected to hard-code a specific amount of damage or read it from a data table.
	 */
	FGameplayTag DamageParameterTag;

	/**
	 * The tag that is used to pass a dynamic resistance amount into the calculation.
	 *
	 * The parameter should be provided via an attribute-based modifier that reads it from the appropriate resistance
	 * attribute of the character's attribute set (e.g., typically an attribute having a name that starts with "Rst",
	 * such as "RstPhysicalBludgeoning", "RstEnergyCold", etc.).
	 */
	FGameplayTag ResistanceParameterTag;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	UPF2SimpleDamageExecution();

	// =================================================================================================================
	// Public Methods - UGameplayEffectExecutionCalculation Implementation
	// =================================================================================================================
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
										OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
