// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayEffectExecutionCalculation.h>

#include "PF2ApplyDamageFromSourceExecution.generated.h"

/**
 * A gameplay effect execution to apply damage from a source onto damage targets, factoring in the target's resistances.
 *
 * The damage to be applied should be accumulated on the source in the form of values in transient attack attributes in
 * the "attack attribute set". Consider this example:
 *
 * - Character A is attacking Character B with an attack that does 5 fire damage and 3 cold damage.
 * - Character B has a +2 resistance to fire damage and a +1 resistance to cold damage.
 *
 * In this example, for Character A's attack attribute set, the "TmpDmgTypeEnergyFire" attribute would have a value of
 * 5 and the "TmpDmgTypeEnergyCold" attribute would have a value of 3. This execution would then read Character B's
 * resistances to fire and cold damage, calculate a total damage of (5-2) + (3-1) = 5, and then apply that damage to the
 * target's incoming damage transient attribute.
 */
UCLASS()
class OPENPF2CORE_API UPF2ApplyDamageFromSourceExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	/**
	 * The gameplay tag for gameplay cues that activate upon damage being inflicted to the target.
	 */
	FGameplayTag InflictDamageCueTag;

public:
	// =================================================================================================================
	// Constructors
	// =================================================================================================================
	/**
	 * Default constructor.
	 */
	explicit UPF2ApplyDamageFromSourceExecution();

	// =================================================================================================================
	// Public Methods - UGameplayEffectExecutionCalculation Implementation
	// =================================================================================================================
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	/**
	 * Gets the gameplay tag for gameplay cues that activate upon damage being inflicted to the target.
	 *
	 * @return
	 *	The gameplay tag for the damage cue.
	 */
	FORCEINLINE const FGameplayTag& GetInflictDamageCueTag() const
	{
		return this->InflictDamageCueTag;
	}

	/**
	 * Populates parameters from a gameplay cue from the parameters of the current GE execution.
	 *
	 * @param [in] ExecutionParams
	 *	The parameters passed to the current GE execution.
	 *
	 * @return
	 *	The new Gameplay Cue parameters.
	 */
	FORCEINLINE FGameplayCueParameters PopulateGameplayCueParameters(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams) const
	{
		return FGameplayCueParameters(ExecutionParams.GetOwningSpec().GetEffectContext());
	}

};
