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

#include "GameplayTags/Traits/Actions.h"

// =====================================================================================================================
// Action Traits
// =====================================================================================================================
// Source: Pathfinder 2E Core Rulebook, Glossary, pages 628-638.
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitActionAuditory,
	"Trait.Action.Auditory",
	"Auditory actions rely on sound. An action with the auditory trait can be successful only if the creature using the action can speak or otherwise produce the required sounds."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitActionConcentrate,
	"Trait.Action.Concentrate",
	"An action with this trait requires a degree of mental concentration and discipline."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitActionFlourish,
	"Trait.Action.Flourish",
	"Flourish actions are actions that require too much exertion to perform a large number in a row. You can use only 1 action with the flourish trait per turn."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitActionManipulate,
	"Trait.Action.Manipulate",
	"You must physically manipulate an item or make gestures to use an action with this trait. Creatures without a suitable appendage can't perform actions with this trait. Manipulate actions often trigger reactions."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitActionMetamagic,
	"Trait.Action.Metamagic",
	"Actions with the metamagic trait, usually from metamagic feats, tweak the properties of your spells. You must use a metamagic action directly before Casting the Spell you want to alter. If you use any action (including free actions and reactions) other than Cast a Spell directly after, you waste the benefits of the metamagic action. Any additional effects added by a metamagic action are part of the spell's effect, not of the metamagic action itself."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitActionMove,
	"Trait.Action.Move",
	"An action with this trait involves moving from one space to another."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitActionOpen,
	"Trait.Action.Open",
	"These maneuvers work only as the first salvo on your turn. You can use an open only if you haven't used an action with the attack or open trait yet this turn."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitActionPress,
	"Trait.Action.Press",
	"Actions with this trait allow you to follow up earlier attacks. An action with the press trait can be used only if you are currently affected by a multiple attack penalty. Some actions with the press trait also grant an effect on a failure. The effects that are added on a failure don't apply on a critical failure. If your press action succeeds, you can choose to apply the failure effect instead. (For example, you may wish to do this when an attack deals no damage due to resistance.) Because a press action requires a multiple attack penalty, you can't use one when it's not your turn, even if you use the Ready activity."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitActionStance,
	"Trait.Action.Stance",
	"A stance is a general combat strategy that you enter by using an action with the stance trait, and that you remain in for some time. A stance lasts until you get knocked out, until its requirements (if any) are violated, until the encounter ends, or until you enter a new stance, whichever comes first. After you use an action with the stance trait, you can't use another one for 1 round. You can enter or be in a stance only in encounter mode."
)
