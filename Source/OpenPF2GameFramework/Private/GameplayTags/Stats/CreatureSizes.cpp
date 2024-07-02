﻿//  OpenPF2 Game Framework for Unreal Engine, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
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

#include "GameplayTags/Stats/CreatureSizes.h"

// =====================================================================================================================
// Creature Size - The approximate physical amount of space a creature occupies.
// =====================================================================================================================
// Source: Pathfinder 2E Core Rulebook, page 474, Table 9-1: Size and Reach.
UE_DEFINE_GAMEPLAY_TAG(Pf2TagCreatureSizeTiny, "CreatureSize.Tiny")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagCreatureSizeSmall, "CreatureSize.Small")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagCreatureSizeMedium, "CreatureSize.Medium")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagCreatureSizeLarge, "CreatureSize.Large")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagCreatureSizeHuge, "CreatureSize.Huge")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagCreatureSizeGargantuan, "CreatureSize.Gargantuan")
