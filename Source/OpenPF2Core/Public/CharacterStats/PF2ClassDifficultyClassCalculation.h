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

#include "PF2ClassDifficultyClassCalculation.generated.h"

/**
 * MMC that calculates the "Class DC" stat.
 *
 * From the Pathfinder 2E Core Rulebook, page 29:
 * "A class DC sets the difficulty for certain abilities granted by your character’s class. This DC equals 10 plus their
 * proficiency bonus for their class DC (+3 for most 1st-level characters) plus the modifier for the class’s key ability
 * score."
 */
UCLASS()
class OPENPF2CORE_API UPF2ClassDifficultyClassCalculation : public UPF2KeyAbilityTemlCalculationBase
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2ClassDifficultyClassCalculation.
	 */
	explicit UPF2ClassDifficultyClassCalculation();
};
