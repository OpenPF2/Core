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

#include "GameplayTags/Traits/Conditions.h"

// =====================================================================================================================
// Condition Traits
// =====================================================================================================================
// Source: Pathfinder 2E Core Rulebook, Conditions Appendix, pages 618-623.

// === Blinded
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionBlinded,
	"Trait.Condition.Blinded",
	"You can't see. All normal terrain is difficult terrain to you. You can't detect anything using vision. You are immune to visual effects. Blinded overrides dazzled."
)

// === Broken
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionBroken,
	"Trait.Condition.Broken",
	"Broken is a condition that affects objects. A broken object can't be used for its normal function, nor does it grant bonuses—with the exception of armor. A broken item still imposes penalties and limitations normally incurred by carrying, holding, or wearing it."
)

// === Clumsy
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionClumsy,
	"Trait.Condition.Clumsy",
	"Your movements become clumsy and inexact. Clumsy always includes a value."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionClumsy1, "Trait.Condition.Clumsy.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionClumsy2, "Trait.Condition.Clumsy.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionClumsy3, "Trait.Condition.Clumsy.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionClumsy4, "Trait.Condition.Clumsy.4")

// === Concealed
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionConcealed,
	"Trait.Condition.Concealed",
	"While you are concealed from a creature, such as in a thick fog, you are difficult for that creature to see. You can still be observed, but you're tougher to target."
)

// === Confused
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionConfused,
	"Trait.Condition.Confused",
	"You don't have your wits about you, and you attack wildly. You are flat-footed, you don't treat anyone as your ally (though they might still treat you as theirs), and you can't Delay, Ready, or use reactions."
)

// === Controlled
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionControlled,
	"Trait.Condition.Controlled",
	"Someone else is making your decisions for you, usually because you're being commanded or magically dominated."
)

// === Dazzled
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionDazzled,
	"Trait.Condition.Dazzled",
	"Your eyes are overstimulated. If vision is your only precise sense, all creatures and objects are concealed from you."
)

// === Deafened
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionDeafened,
	"Trait.Condition.Deafened",
	"You can't hear. You automatically critically fail Perception checks that require you to be able to hear. You are immune to auditory effects."
)

// === Doomed
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionDoomed,
	"Trait.Condition.Doomed",
	"A powerful force has gripped your soul, calling you closer to death. Doomed always includes a value. When you die, you're no longer doomed."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDoomed1, "Trait.Condition.Doomed.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDoomed2, "Trait.Condition.Doomed.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDoomed3, "Trait.Condition.Doomed.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDoomed4, "Trait.Condition.Doomed.4")

// === Drained
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionDrained,
	"Trait.Condition.Drained",
	"When a creature successfully drains you of blood or life force, you become less healthy. Drained always includes a value."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDrained1, "Trait.Condition.Drained.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDrained2, "Trait.Condition.Drained.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDrained3, "Trait.Condition.Drained.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDrained4, "Trait.Condition.Drained.4")

// === Dying
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionDying,
	"Trait.Condition.Dying",
	"You are bleeding out or otherwise at death's door. While you have this condition, you are unconscious. Dying always includes a value, and if it ever reaches dying 4, you die."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDying1, "Trait.Condition.Dying.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDying2, "Trait.Condition.Dying.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDying3, "Trait.Condition.Dying.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDying4, "Trait.Condition.Dying.4")

// === Encumbered
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionEncumbered,
	"Trait.Condition.Encumbered",
	"You are carrying more weight than you can manage."
)

// === Enfeebled
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionEnfeebled,
	"Trait.Condition.Enfeebled",
	"You're physically weakened. Enfeebled always includes a value."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionEnfeebled1, "Trait.Condition.Enfeebled.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionEnfeebled2, "Trait.Condition.Enfeebled.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionEnfeebled3, "Trait.Condition.Enfeebled.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionEnfeebled4, "Trait.Condition.Enfeebled.4")

// === Fascinated
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionFascinated,
	"Trait.Condition.Fascinated",
	"You are compelled to focus your attention on something, distracting you from whatever else is going on around you."
)

// === Fatigued
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionFatigued,
	"Trait.Condition.Fatigued",
	"You're tired and can't summon much energy."
)

// === Flat footed
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionFlatFooted,
	"Trait.Condition.FlatFooted",
	"You're distracted or otherwise unable to focus your full attention on defense."
)

// === Fleeing
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionFleeing,
	"Trait.Condition.Fleeing",
	"You're forced to run away due to fear or some other compulsion."
)

// === Friendly
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionFriendly,
	"Trait.Condition.Friendly",
	"This condition reflects a creature's disposition toward a particular character. A creature that is friendly to a character likes that character."
)

// === Frightened
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionFrightened,
	"Trait.Condition.Frightened",
	"You're gripped by fear and struggle to control your nerves. The frightened condition always includes a value."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionFrightened1, "Trait.Condition.Frightened.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionFrightened2, "Trait.Condition.Frightened.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionFrightened3, "Trait.Condition.Frightened.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionFrightened4, "Trait.Condition.Frightened.4")

// === Grabbed
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionGrabbed,
	"Trait.Condition.Grabbed",
	"You're held in place by another creature, giving you the flat-footed and immobilized conditions."
)

// === Helpful
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionHelpful,
	"Trait.Condition.Helpful",
	"This condition reflects a creature's disposition toward a particular character. A creature that is helpful to a character wishes to actively aid that character."
)

// === Hidden
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionHidden,
	"Trait.Condition.Hidden",
	"While you're hidden from a creature, that creature knows the space you're in but can't tell precisely where you are."
)

// === Hostile
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionHostile,
	"Trait.Condition.Hostile",
	"This condition reflects a creature's disposition toward a particular character. A creature that is hostile to a character actively seeks to harm that character."
)

// === Immobilized
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionImmobilized,
	"Trait.Condition.Immobilized",
	"You can't use any action with the move trait."
)

// === Indifferent
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionIndifferent,
	"Trait.Condition.Indifferent",
	"This condition reflects a creature's disposition toward a particular character. A creature that is indifferent to a character doesn't really care one way or the other about that character."
)

// === Invisible
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionInvisible,
	"Trait.Condition.Invisible",
	"While invisible, you can't be seen. You're undetected to everyone."
)

// === Observed
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionObserved,
	"Trait.Condition.Observed",
	"Anything in plain view is observed by you."
)

// === Paralyzed
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionParalyzed,
	"Trait.Condition.Paralyzed",
	"You are frozen in place. You have the flat-footed condition and can't act except to Recall Knowledge and use actions that require only your mind. Your senses still function, but only in the areas you can perceive without moving."
)

// === Petrified
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionPetrified,
	"Trait.Condition.Petrified",
	"You have been turned to stone. You can't act, nor can you sense anything. While petrified, your mind and body are in stasis, so you don't age or notice the passing of time."
)

// === Prone
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionProne,
	"Trait.Condition.Prone",
	"You're lying on the ground. You are flat-footed."
)

// === Quickened
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionQuickened,
	"Trait.Condition.Quickened",
	"You gain 1 additional action at the start of your turn each round. Many effects that make you quickened specify the types of actions you can use with this additional action."
)

// === Restrained
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionRestrained,
	"Trait.Condition.Restrained",
	"You're tied up and can barely move, or a creature has you pinned. You have the flat-footed and immobilized conditions, and you can't use any actions with the attack or manipulate traits except to attempt to Escape or Force. Open your bonds. Restrained overrides grabbed."
)

// === Sickened
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionSickened,
	"Trait.Condition.Sickened",
	"You feel ill. Sickened always includes a value. You can't willingly ingest anything—including elixirs and potions—while sickened."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionSickened1, "Trait.Condition.Sickened.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionSickened2, "Trait.Condition.Sickened.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionSickened3, "Trait.Condition.Sickened.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionSickened4, "Trait.Condition.Sickened.4")

// === Slowed
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionSlowed,
	"Trait.Condition.Slowed",
	"You have fewer actions. Slowed always includes a value."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionSlowed1, "Trait.Condition.Slowed.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionSlowed2, "Trait.Condition.Slowed.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionSlowed3, "Trait.Condition.Slowed.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionSlowed4, "Trait.Condition.Slowed.4")

// === Stunned
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionStunned,
	"Trait.Condition.Stunned",
	"You've become senseless. You can't act while stunned. Stunned usually includes a value. Stunned overrides slowed."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionStunned1, "Trait.Condition.Stunned.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionStunned2, "Trait.Condition.Stunned.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionStunned3, "Trait.Condition.Stunned.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionStunned4, "Trait.Condition.Stunned.4")

// === Stupefied
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionStupefied,
	"Trait.Condition.Stupefied",
	"Your thoughts and instincts are clouded. Stupefied always includes a value."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionStupefied1, "Trait.Condition.Stupefied.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionStupefied2, "Trait.Condition.Stupefied.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionStupefied3, "Trait.Condition.Stupefied.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionStupefied4, "Trait.Condition.Stupefied.4")

// === Unconscious
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionUnconscious,
	"Trait.Condition.Unconscious",
	"You're sleeping, or you've been knocked out. You can't act."
)

// === Undetected
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionUndetected,
	"Trait.Condition.Undetected",
	"When you are undetected by a creature, that creature cannot see you at all, has no idea what space you occupy, and can't target you, though you still can be affected by abilities that target an area."
)

// === Unfriendly
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionUnfriendly,
	"Trait.Condition.Unfriendly",
	"This condition reflects a creature's disposition toward a particular character. A creature that is unfriendly to a character dislikes and specifically distrusts that character."
)

// === Unnoticed
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionUnnoticed,
	"Trait.Condition.Unnoticed",
	"If you are unnoticed by a creature, that creature has no idea you are present at all. When you’re unnoticed, you’re also undetected by the creature."
)

// === Wounded
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitConditionWounded,
	"Trait.Condition.Wounded",
	"You have been seriously injured."
)

UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionWounded1, "Trait.Condition.Wounded.1")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionWounded2, "Trait.Condition.Wounded.2")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionWounded3, "Trait.Condition.Wounded.3")
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionWounded4, "Trait.Condition.Wounded.4")

// The following conditions do not appear in the Core Rulebook but were added by OpenPF2.

// === Dead
UE_DEFINE_GAMEPLAY_TAG(Pf2TagTraitConditionDead, "Trait.Condition.Dead")
