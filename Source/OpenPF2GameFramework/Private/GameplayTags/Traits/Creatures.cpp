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

#include "GameplayTags/Traits/Creatures.h"

// =====================================================================================================================
// Creature Traits
// =====================================================================================================================
// Source: Pathfinder 2E Core Rulebook, Glossary, pages 628-638.
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitCreatureElf,
	"Trait.Creature.Elf",
	"A creature with this trait is a member of the elf ancestry. Elves are mysterious people with rich traditions of magic and scholarship who typically have low-light vision. An ability with this trait can be used or selected only by elves. A weapon with this trait is created and used by elves."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitCreatureHalfElf,
	"Trait.Creature.HalfElf",
	"A creature with this trait is part human and part elf. An ability with this trait can be used or selected only by half-elves."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitCreatureHalfOrc,
	"Trait.Creature.HalfOrc",
	"A creature with this trait is part human and part orc. An ability with this trait can be used or selected only by half-orcs."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitCreatureHuman,
	"Trait.Creature.Human",
	"A creature with this trait is a member of the human ancestry. Humans are a diverse array of people known for their adaptability. An ability with this trait can be used or selected only by humans."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitCreatureHumanoid,
	"Trait.Creature.Humanoid",
	"Humanoid creatures reason and act much like humans. They typically stand upright and have two arms and two legs."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitCreatureOrc,
	"Trait.Creature.Orc",
	"A creature with this trait is a member of the orc ancestry. These green-skinned people tend to have darkvision. An ability with this trait can be used or selected only by orcs. An item with this trait is created and used by orcs."
)
