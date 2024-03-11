// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "CharacterStats/PF2AbilityCalculationBase.h"

#include "PF2AbilityBoostCalculationBase.generated.h"

/**
 * An MMC for boosting a single ability score.
 *
 * Ability boosts are available via both an MMC and a "Gameplay Effect Execution Calculation" (GEX). Prefer the GEX
 * variation instead of this MMC variation whenever possible, as the GEX variation automatically increments the count of
 * how many boosts are applied.
 */
UCLASS(Abstract)
class OPENPF2CORE_API UPF2AbilityBoostCalculationBase : public UPF2AbilityCalculationBase
{
	GENERATED_BODY()

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Calculates an ability boost.
	 *
	 * @see UPF2CharacterStatLibrary::CalculateAbilityBoostAmount
	 *
	 * @param Spec
	 *	The Gameplay Effect (GE) specification that provides information about the ability score for which a calculated
	 *	value is desired.
	 * @param AbilityAttribute
	 *	The type of ability score for which an ability boost is desired.
	 * @param AbilityScore
	 *	The current base value of the ability attribute.
	 *
	 * @return
	 *	The amount of the boost to apply to the ability score. (This is just the boost; the ability score has not been
	 *	added to the result).
	 */
	virtual float DoCalculation(
		const FGameplayEffectSpec& Spec,
		const FGameplayAttribute   AbilityAttribute,
		const float                AbilityScore) const override;
};
