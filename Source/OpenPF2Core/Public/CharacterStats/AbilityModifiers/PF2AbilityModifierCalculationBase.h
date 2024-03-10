// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayModMagnitudeCalculation.h>

#include "CharacterStats/PF2AbilityCalculationBase.h"

#include "PF2AbilityModifierCalculationBase.generated.h"

/**
 * Base MMC used for determining ability modifiers from normal ability scores.
 *
 * There is expected to be one blueprint sub-class of this base class for each ability (Strength, Dexterity, Constitution,
 * etc). The ability for which the modifier is being calculated *must* be the one and ONLY attribute being captured.
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2AbilityModifierCalculationBase : public UPF2AbilityCalculationBase
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Calculates an ability modifier based on the attribute captured by the provided GE specification.
	 *
	 * @see UPF2CharacterStatLibrary::CalculateAbilityModifier
	 *
	 * @param Spec
	 *	The Gameplay Effect (GE) specification that provides information about the ability score for which a calculated
	 *	value is desired.
	 * @param AbilityAttribute
	 *	The type of ability score for which an ability modifier is desired.
	 * @param AbilityScore
	 *	The current base value of the ability attribute.
	 *
	 * @return
	 *	The ability modifier.
	 */
	virtual float DoCalculation(
		const FGameplayEffectSpec& Spec,
		const FGameplayAttribute   AbilityAttribute,
		const float                AbilityScore) const override;
};
