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

#pragma once

#include <GameplayModMagnitudeCalculation.h>

#include "PF2ArmorClassCalculation.generated.h"

/**
 * MMC that calculates the "Armor Class (AC)" stat.
 *
 * From the Pathfinder 2E Core Rulebook, page 29:
 * "Your character’s Armor Class represents how difficult they are to hit in combat. To calculate your AC, add 10 plus
 * your character’s Dexterity modifier (up to their armor’s Dexterity modifier cap; page 274), plus their proficiency
 * bonus with their armor, plus their armor’s item bonus to AC and any other permanent bonuses and penalties."
 */
UCLASS()
class OPENPF2CORE_API UPF2ArmorClassCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2ArmorClassCalculation.
	 */
	explicit UPF2ArmorClassCalculation();

	// =================================================================================================================
	// Public Methods - UGameplayModMagnitudeCalculation Implementation
	// =================================================================================================================
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * Capture definition for the Dexterity Ability Modifier.
	 */
	FGameplayEffectAttributeCaptureDefinition DexterityModifierCaptureDefinition;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the captured dexterity modifier.
	 *
	 * @return
	 *	The character's dexterity modifier.
	 */
	float GetDexterityModifier(const FGameplayEffectSpec& Spec) const;

	/**
	 * Calculates the bonus to AC gained from the type of armor being worn.
	 *
	 * @param Spec
	 *	The Gameplay Effect (GE) specification that provides information about the character's attributes and tags.
	 *
	 * @return
	 *	The amount that the current armor type contributes to the character's AC modifier.
	 */
	float CalculateArmorTypeProficiencyBonus(const FGameplayEffectSpec& Spec) const;

	/**
	 * Returns the type of armor that the character is wearing.
	 *
	 * @param SourceTags
	 *	The tags on the character. A tag in this list should indicate the character's equipped armor (e.g.
	 *	"Armor.Equipped.Unarmored", "Armor.Equipped.Light", etc.).
	 *
	 * @return
	 *	The name of the armor type ("Unarmored", "Light", "Medium", or "Heavy").
	 */
	FString DetermineArmorType(const FGameplayTagContainer *SourceTags) const;
};
