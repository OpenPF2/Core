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

#include "GameplayTags/Stats/CreatureAlignments.h"

// =====================================================================================================================
// Creature Alignment - The indicator of the character's morality and personality.
// =====================================================================================================================
// Source: Pathfinder 2E Core Rulebook, page 29, Table 1-2: The Nine Alignments.
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentLawful,
	"CreatureAlignment.Lawful",
	"Character values consistency, stability, and predictability over flexibility"
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentLawfulGood,
	"CreatureAlignment.Lawful.Good",
	"Character values consistency, stability, and predictability over flexibility; considers others above themselves and works selflessly to assist others."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentLawfulNeutral,
	"CreatureAlignment.Lawful.Neutral",
	"Character values consistency, stability, and predictability over flexibility; doesn't consider others more or less importantly than themselves."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentLawfulEvil,
	"CreatureAlignment.Lawful.Evil",
	"Character values consistency, stability, and predictability over flexibility; is willing to victimize or harm others for selfish gain."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentNeutral,
	"CreatureAlignment.Neutral",
	"Character obeys law or code of conduct in many situations, but is flexible."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentNeutralGood,
	"CreatureAlignment.Neutral.Good",
	"Character obeys law or code of conduct in many situations, but is flexible; considers others above themselves and works selflessly to assist others."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentNeutralNeutral,
	"CreatureAlignment.Neutral.Neutral",
	"Character obeys law or code of conduct in many situations, but is flexible; doesn't consider others more or less importantly than themselves."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentNeutralEvil,
	"CreatureAlignment.Neutral.Evil",
	"Character obeys law or code of conduct in many situations, but is flexible; is willing to victimize or harm others for selfish gain."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentChaotic,
	"CreatureAlignment.Chaotic",
	"Character values flexibility, creativity, and spontaneity over consistency."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentChaoticGood,
	"CreatureAlignment.Chaotic.Good",
	"Character values flexibility, creativity, and spontaneity over consistency; considers others above themselves and works selflessly to assist others."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentChaoticNeutral,
	"CreatureAlignment.Chaotic.Neutral",
	"Character values flexibility, creativity, and spontaneity over consistency; doesn't consider others more or less importantly than themselves."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagCreatureAlignmentChaoticEvil,
	"CreatureAlignment.Chaotic.Evil",
	"Character values flexibility, creativity, and spontaneity over consistency; is willing to victimize or harm others for selfish gain."
)
