// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayEffectTypes.h>
#include <GameplayTagContainer.h>

/**
 * A single TEML proficiency calculation for a character attribute.
 *
 * TEML proficiency in an attribute is represented by a tag under a unique prefix for that attribute. For example, a
 * character with the "SavingThrow.Reflex.Trained" and "Perception.Master" tags has a "Trained" proficiency in reflex
 * saving throws and "Master" proficiency in Perception, with the "SavingThrow.Reflex" and "Perception" tag prefixes,
 * respectively.
 */
class OPENPF2CORE_API FPF2TemlCalculation
{
protected:
	/**
	 * The value that was calculated for this TEML proficiency.
	 */
	float Value;

public:
	/**
	 * Constructor for FPF2TemlCalculation based on a the given prefix/root tag prefix and GE spec.
	 *
	 * @param TagPrefix
	 *	The root/parent tag of tags that represent the TEML proficiency to examine; as a tag.
	 * @param Spec
	 *	The Gameplay Effect (GE) specification that provides information about the character and their proficiencies.
	 */
	explicit FPF2TemlCalculation(const FGameplayTag TagPrefix, const FGameplayEffectSpec& Spec);

	/**
	 * Constructor for FPF2TemlCalculation based on a the given prefix/root tag prefix (as an FName) and GE spec.
	 *
	 * @param TagPrefix
	 *	The root/parent tag of tags that represent the TEML proficiency to examine; as a tag name.
	 * @param Spec
	 *	The Gameplay Effect (GE) specification that provides information about the character and their proficiencies.
	 */
	explicit FPF2TemlCalculation(const FName TagPrefix, const FGameplayEffectSpec& Spec);

	/**
	 * Constructor for FPF2TemlCalculation based on a the given prefix/root tag prefix (as an FString) and GE spec.
	 *
	 * @param TagPrefix
	 *	The root/parent tag of tags that represent the TEML proficiency to examine; as a string.
	 * @param Spec
	 *	The Gameplay Effect (GE) specification that provides information about the character and their proficiencies.
	 */
	explicit FPF2TemlCalculation(const FString& TagPrefix, const FGameplayEffectSpec& Spec);

	/**
	 * Constructor for FPF2TemlCalculation based on a the given prefix/root tag prefix, tag container, and char. level.
	 *
	 * @param TagPrefix
	 *	The root/parent tag of tags that represent the TEML proficiency to examine; as a string.
	 * @param CharacterTags
	 *	The tags on the character of interest, for which proficiencies will be determined.
	 * @param CharacterLevel
	 *	The level of the character, which affects the magnitude of the calculation.
	 */
	explicit FPF2TemlCalculation(
		const FGameplayTag TagPrefix,
		const FGameplayTagContainer* CharacterTags,
		float CharacterLevel);

	/**
	 * Gets the calculated TEML proficiency.
	 *
	 * @return
	 *	The amount that the character's proficiency in a stat contributes to the modifier for the stat.
	 */
	float GetValue() const;
};
