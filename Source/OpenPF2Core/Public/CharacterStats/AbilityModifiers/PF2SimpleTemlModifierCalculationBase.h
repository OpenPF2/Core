// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayModMagnitudeCalculation.h>

#include "CharacterStats/PF2AbilityCalculationBase.h"

#include "PF2SimpleTemlModifierCalculationBase.generated.h"

/**
 * Base class for MMCs that perform a simple calculation to determine the TEML proficiency modifier for a character
 * attribute. This base class is only used to calculate proficiencies that are a function of the value of a character's
 * ability score modifier (Strength, Dexterity, Intelligence, etc.) and a TEML proficiency (e.g. skills, saving throws,
 * and perception).
 *
 * More advanced TEML-based calculations (Armor Class, Class DC, Spell Attack Roll, and Spell DC) derive from
 * UGameplayModMagnitudeCalculation directly instead of deriving from this base class, as they have more much more
 * complex business logic.
 */
UCLASS(Abstract)
class OPENPF2CORE_API UPF2SimpleTemlModifierCalculationBase : public UPF2AbilityCalculationBase
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The root tag (aka tag prefix) of the tags that indicate the character's training in the attribute.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="OpenPF2 - Attributes")
	FGameplayTag ProficiencyRootTag;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	// Avoid warnings C4263 and C4264 by ensuring we inherit base class implementation.
	using UPF2AbilityCalculationBase::DoCalculation;

	virtual float DoCalculation(
		const FGameplayEffectSpec& Spec,
		const FGameplayAttribute   AbilityAttribute,
		const float                AbilityScore) const override;

	/**
	 * Calculates an attribute proficiency based on the captured attribute value and TEML proficiency value.
	 *
	 * @param Spec
	 *	The Gameplay Effect (GE) specification that provides information about the ability score for which a TEML
	 *	proficiency value is desired.
	 * @param AbilityAttribute
	 *	The type of ability score for which a calculated value is desired.
	 * @param AbilityScore
	 *	The current base value of the ability attribute.
	 * @param TemlProficiencyBonus
	 *	The proficiency the character has in this ability, as derived from tags on the character corresponding to the
	 *	proficiency root tag.
	 *
	 * @return
	 *	The calculated attribute proficiency.
	 */
	virtual float DoCalculation(
		const FGameplayEffectSpec& Spec,
		const FGameplayAttribute   AbilityAttribute,
		const float                AbilityScore,
		const float                TemlProficiencyBonus) const;
};
