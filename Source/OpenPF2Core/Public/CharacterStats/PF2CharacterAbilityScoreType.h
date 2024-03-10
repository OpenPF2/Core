// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Misc/EnumRange.h>

#include "PF2CharacterAbilityScoreType.generated.h"

/**
 * Enumerated type for the abilities that OpenPF2 characters possess (aka "boostable" abilities).
 *
 * The name of each enum value exactly matches the name of the corresponding character attribute in the GAS attribute
 * set for the character.
 *
 * @see UPF2CharacterAttributeSet
 */
UENUM(BlueprintType)
enum class EPF2CharacterAbilityScoreType : uint8
{
	// The key/machine name of each enum value MUST exactly match the name of the corresponding character attribute from
	// PF2CharacterAttributeSet.
	AbStrength     UMETA(DisplayName = "Strength"),
	AbDexterity    UMETA(DisplayName = "Dexterity"),
	AbConstitution UMETA(DisplayName = "Constitution"),
	AbIntelligence UMETA(DisplayName = "Intelligence"),
	AbWisdom       UMETA(DisplayName = "Wisdom"),
	AbCharisma     UMETA(DisplayName = "Charisma"),

	Count          UMETA(Hidden)
};

// Allow enum to be iterated by foreach loops.
ENUM_RANGE_BY_COUNT(EPF2CharacterAbilityScoreType, EPF2CharacterAbilityScoreType::Count)
