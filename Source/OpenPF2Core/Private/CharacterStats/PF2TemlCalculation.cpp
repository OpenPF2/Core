// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
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

#include "CharacterStats/PF2TemlCalculation.h"

#include "Utilities/PF2GameplayAbilityUtilities.h"

FPF2TemlCalculation::FPF2TemlCalculation(const FString& TagPrefix, const FGameplayEffectSpec& Spec) :
	FPF2TemlCalculation(PF2GameplayAbilityUtilities::GetTag(TagPrefix), Spec)
{
}

FPF2TemlCalculation::FPF2TemlCalculation(const FName TagPrefix, const FGameplayEffectSpec& Spec) :
	FPF2TemlCalculation(PF2GameplayAbilityUtilities::GetTag(TagPrefix), Spec)
{
}

FPF2TemlCalculation::FPF2TemlCalculation(const FGameplayTag TagPrefix, const FGameplayEffectSpec& Spec) :
	FPF2TemlCalculation(TagPrefix, Spec.CapturedSourceTags.GetAggregatedTags(), Spec.GetLevel())
{
}

FPF2TemlCalculation::FPF2TemlCalculation(const FGameplayTag TagPrefix,
										 const FGameplayTagContainer* CharacterTags,
										 const float CharacterLevel)
{
	float ProficiencyBonus = 0.0f;

	// Bypass additional checks if the character has no proficiency with this skill, to avoid checking every TEML
	// option.
	if (CharacterTags->HasTag(TagPrefix))
	{
		const FString TagPrefixString = TagPrefix.GetTagName().GetPlainNameString();

		// "When attempting a check that involves something you have some training in, you will also add your
		// proficiency bonus. This bonus depends on your proficiency rank: untrained, trained, expert, master, or
		// legendary. If you’re untrained, your bonus is +0—you must rely on raw talent and any bonuses from the
		// situation. Otherwise, the bonus equals your character’s level plus a certain amount depending on your rank.
		// If your proficiency rank is trained, this bonus is equal to your level + 2, and higher proficiency ranks
		// further increase the amount you add to your level."
		//
		// Source: Pathfinder 2E Core Rulebook, page 444, "Step 1: Roll D20 and Identify The Modifiers, Bonuses, and
		// Penalties That Apply".
		if (PF2GameplayAbilityUtilities::HasTag(CharacterTags, TagPrefixString + ".Legendary"))
		{
			// Legendary -> Your level + 8
			ProficiencyBonus = CharacterLevel + 8;
		}
		else if (PF2GameplayAbilityUtilities::HasTag(CharacterTags, TagPrefixString + ".Master"))
		{
			// Master -> Your level + 6
			ProficiencyBonus = CharacterLevel + 6;
		}
		else if (PF2GameplayAbilityUtilities::HasTag(CharacterTags, TagPrefixString + ".Expert"))
		{
			// Expert -> Your level + 4
			ProficiencyBonus = CharacterLevel + 4;
		}
		else if (PF2GameplayAbilityUtilities::HasTag(CharacterTags, TagPrefixString + ".Trained"))
		{
			// Trained -> Your level + 2
			ProficiencyBonus = CharacterLevel + 2;
		}
		else
		{
			// Untrained -> No bonus at all, no matter what level.
			ProficiencyBonus = 0;
		}
	}

	this->Value = ProficiencyBonus;
}

FORCEINLINE float FPF2TemlCalculation::GetValue() const
{
	return this->Value;
}
