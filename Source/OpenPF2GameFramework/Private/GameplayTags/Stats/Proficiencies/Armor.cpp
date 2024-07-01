//  OpenPF2 Game Framework for Unreal Engine, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
//  Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//    - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//    - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//    - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
//
//  Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
//  as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
//  file other than the material designated as Open Game Content may be reproduced in any form without written
//  permission.

#include "GameplayTags/Stats/Proficiencies/Armor.h"

// =====================================================================================================================
// Proficiency Ranks for Different Classes of Armor (Unarmored, Light, Medium, or Heavy)
// =====================================================================================================================
// Sources:
//   - Pathfinder 2E Core Rulebook, Chapter 1: "Introduction", page 11, "Playing the Game".
//   - Pathfinder 2E Core Rulebook, Chapter 1: "Introduction", page 13, "Proficiency".
//   - Pathfinder 2E Core Rulebook, Chapter 1: "Introduction", page 27, "Character Sheet".
//   - Pathfinder 2E Core Rulebook, Appendix, page 624, "Armor Class".

// === Unarmored
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryUnarmoredUntrained,
	"Armor.Category.Unarmored.Untrained",
	"Character is Untrained in Unarmored Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryUnarmoredTrained,
	"Armor.Category.Unarmored.Trained",
	"Character is Trained in Unarmored Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryUnarmoredExpert,
	"Armor.Category.Unarmored.Expert",
	"Character is Expert in Unarmored Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryUnarmoredMaster,
	"Armor.Category.Unarmored.Master",
	"Character is Master in Unarmored Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryUnarmoredLegendary,
	"Armor.Category.Unarmored.Legendary",
	"Character is Legendary in Unarmored Defense."
)

// === Light
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryLightUntrained,
	"Armor.Category.Light.Untrained",
	"Character is Untrained in Light Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryLightTrained,
	"Armor.Category.Light.Trained",
	"Character is Trained in Light Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryLightExpert,
	"Armor.Category.Light.Expert",
	"Character is Expert in Light Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryLightMaster,
	"Armor.Category.Light.Master",
	"Character is Master in Light Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryLightLegendary,
	"Armor.Category.Light.Legendary",
	"Character is Legendary in Light Armor Defense."
)

// === Medium
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryMediumUntrained,
	"Armor.Category.Medium.Untrained",
	"Character is Untrained in Medium Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryMediumTrained,
	"Armor.Category.Medium.Trained",
	"Character is Trained in Medium Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryMediumExpert,
	"Armor.Category.Medium.Expert",
	"Character is Expert in Medium Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryMediumMaster,
	"Armor.Category.Medium.Master",
	"Character is Master in Medium Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryMediumLegendary,
	"Armor.Category.Medium.Legendary",
	"Character is Legendary in Medium Armor Defense."
)

// === Heavy
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryHeavyUntrained,
	"Armor.Category.Heavy.Untrained",
	"Character is Untrained in Heavy Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryHeavyTrained,
	"Armor.Category.Heavy.Trained",
	"Character is Trained in Heavy Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryHeavyExpert,
	"Armor.Category.Heavy.Expert",
	"Character is Expert in Heavy Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryHeavyMaster,
	"Armor.Category.Heavy.Master",
	"Character is Master in Heavy Armor Defense."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagArmorCategoryHeavyLegendary,
	"Armor.Category.Heavy.Legendary",
	"Character is Legendary in Heavy Armor Defense."
)
