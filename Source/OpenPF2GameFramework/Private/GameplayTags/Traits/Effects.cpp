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

#include "GameplayTags/Traits/Effects.h"

// =====================================================================================================================
// Effect Traits
// =====================================================================================================================
// Source: Pathfinder 2E Core Rulebook, Conditions Appendix, pages 628-638.
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitEffectDeath,
	"Trait.Effect.Death",
	"An effect with the death trait kills you immediately if it reduces you to 0 HP. Some death effects can bring you closer to death or slay you outright without reducing you to 0 HP."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitEffectDisease,
	"Trait.Effect.Disease",
	"An effect with this trait applies one or more diseases. A disease is typically an affliction."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitEffectLinguistic,
	"Trait.Effect.Linguistic",
	"An effect with this trait depends on language comprehension. A linguistic effect that targets a creature works only if the target understands the language you are using."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitEffectNonlethal,
	"Trait.Effect.Nonlethal",
	"An effect with this trait is not inherently deadly. Damage from a nonlethal effect knocks a creature out rather than killing it."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitEffectPoison,
	"Trait.Effect.Poison",
	"An effect with this trait delivers a poison or deals poison damage. An item with this trait is poisonous and might cause an affliction."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitEffectSonic,
	"Trait.Effect.Sonic",
	"An effect with the sonic trait functions only if it makes sound, meaning it has no effect in an area of silence or in a vacuum. This is different from an auditory effect, which functions only if the target can hear it. A sonic effect might deal sonic damage."
)
