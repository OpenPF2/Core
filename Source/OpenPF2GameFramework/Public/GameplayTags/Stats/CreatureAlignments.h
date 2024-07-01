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
// Creature Alignment - The indicator of the character's morality and personality.
// =====================================================================================================================
// Source: Pathfinder 2E Core Rulebook, page 29, Table 1-2: The Nine Alignments.
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentLawful)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentLawfulGood)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentLawfulNeutral)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentLawfulEvil)

OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentNeutral)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentNeutralGood)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentNeutralNeutral)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentNeutralEvil)

OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentChaotic)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentChaoticGood)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentChaoticNeutral)
OPENPF2GAMEFRAMEWORK_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pf2TagCreatureAlignmentChaoticEvil)
