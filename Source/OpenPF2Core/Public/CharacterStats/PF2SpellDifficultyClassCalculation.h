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

#include "CharacterStats/PF2KeyAbilityTemlCalculationBase.h"

#include "PF2SpellDifficultyClassCalculation.generated.h"

/**
 * MMC that calculates the "Spell DC" stat.
 *
 * From the Pathfinder 2E Core Rulebook, page 448:
 * "Many times, instead of requiring you to make a spell attack roll, the spells you cast will require those within the
 * area or targeted by the spell to attempt a saving throw against your Spell DC to determine how the spell affects
 * them.
 *
 * Your spell DC is calculated using the following formula.
 * Spell DC =
 * 10 + ability modifier used for spellcasting + proficiency bonus + other bonuses + penalties"
 */
UCLASS()
class OPENPF2CORE_API UPF2SpellDifficultyClassCalculation : public UPF2KeyAbilityTemlCalculationBase
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Constructors
	// =================================================================================================================
	/**
	 * Constructor for UPF2SpellDifficultyClassCalculation.
	 */
	explicit UPF2SpellDifficultyClassCalculation();
};
