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

#pragma once

#include <NativeGameplayTags.h>

// =====================================================================================================================
// Proficiency Ranks for Different Classes of Armor (Unarmored, Light, Medium, or Heavy)
// =====================================================================================================================
// Sources:
//   - Pathfinder 2E Core Rulebook, Chapter 1: "Introduction", page 11, "Playing the Game".
//   - Pathfinder 2E Core Rulebook, Chapter 1: "Introduction", page 13, "Proficiency".
//   - Pathfinder 2E Core Rulebook, Chapter 1: "Introduction", page 27, "Character Sheet".
//   - Pathfinder 2E Core Rulebook, Appendix, page 624, "Armor Class".

// === Unarmored
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryUnarmoredUntrained)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryUnarmoredTrained)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryUnarmoredExpert)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryUnarmoredMaster)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryUnarmoredLegendary)

// === Light
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryLightUntrained)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryLightTrained)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryLightExpert)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryLightMaster)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryLightLegendary)

// === Medium
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryMediumUntrained)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryMediumTrained)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryMediumExpert)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryMediumMaster)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryMediumLegendary)

// === Heavy
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryHeavyUntrained)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryHeavyTrained)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryHeavyExpert)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryHeavyMaster)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagArmorCategoryHeavyLegendary)
