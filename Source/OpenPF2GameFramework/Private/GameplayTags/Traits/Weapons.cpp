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

#include "GameplayTags/Traits/Weapons.h"

// =====================================================================================================================
// Weapon Traits
// =====================================================================================================================
// Source: Pathfinder 2E Core Rulebook, "Weapon Traits", pages 282-283.
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponAgile,
	"Trait.Weapon.Agile",
	"A weapon with this trait has a different multiple attack penalty. The second attack on a character's turn is –4 instead of –5, and –8 instead of –10 on the third and subsequent attacks in the turn."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponAttachedToShield,
	"Trait.Weapon.Attached.ToShield",
	"A weapon with this trait must be combined with a shield to be used."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponBackstabber,
	"Trait.Weapon.Backstabber",
	"A weapon with this trait behaves differently when hitting a flat-footed creature. This weapon deals 1 precision damage in addition to its normal damage. The precision damage increases to 2 if the weapon is a +3 weapon."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponBackswing,
	"Trait.Weapon.Backswing",
	"A weapon with this trait allows a character to use the momentum from a missed attack to lead into their next attack. After missing with this weapon on their turn, a character gains a +1 circumstance bonus to their next attack with this weapon before the end of their turn."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponDeadlyD6,
	"Trait.Weapon.Deadly.D6",
	"A weapon with this trait gets a D6 damage die added to critical hits. The die is rolled after doubling the weapon's damage. This increases to two dice if the weapon has a greater striking rune and three dice if the weapon has a major striking rune."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponDeadlyD8,
	"Trait.Weapon.Deadly.D8",
	"A weapon with this trait gets a D8 damage die added to critical hits. The die is rolled after doubling the weapon's damage. This increases to two dice if the weapon has a greater striking rune and three dice if the weapon has a major striking rune."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponDeadlyD10,
	"Trait.Weapon.Deadly.D10",
	"A weapon with this trait gets a D10 damage die added to critical hits. The die is rolled after doubling the weapon's damage. This increases to two dice if the weapon has a greater striking rune and three dice if the weapon has a major striking rune."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponDisarm,
	"Trait.Weapon.Disarm",
	"A weapon with this trait allows a character to Disarm with the Athletics skill even if the character doesn't have a free hand. This uses the weapon's reach (if different from the character's own) and adds the weapon's item bonus to attack rolls (if any) as an item bonus to the Athletics check. If a character critically fails a check to Disarm using the weapon, they can drop the weapon to take the effects of a failure instead of a critical failure. On a critical success, they still need a free hand if they want to take the item."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponDwarf,
	"Trait.Weapon.Dwarf",
	"A weapon with this trait is crafted and used by Dwarves."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponElf,
	"Trait.Weapon.Elf",
	"A weapon with this trait is crafted and used by Elves."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponFatalD8,
	"Trait.Weapon.Fatal.D8",
	"A weapon with this trait has a D8 damage die on a critical hit instead of the normal die size, along with one additional D8 damage die."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponFatalD10,
	"Trait.Weapon.Fatal.D10",
	"A weapon with this trait has a D10 damage die on a critical hit instead of the normal die size, along with one additional D10 damage die."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponFatalD12,
	"Trait.Weapon.Fatal.D12",
	"A weapon with this trait has a D12 damage die on a critical hit instead of the normal die size, along with one additional D12 damage die."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponFinesse,
	"Trait.Weapon.Finesse",
	"A melee weapon with this trait allows a character to use their Dexterity modifier instead of their Strength modifier on attack rolls. The character still uses their Strength modifier when calculating damage."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponForceful,
	"Trait.Weapon.Forceful",
	"A weapon with this trait becomes more dangerous as a character builds momentum. When they attack with it more than once on their turn, the second attack gains a circumstance bonus to damage equal to the number of weapon damage dice, and each subsequent attack gains a circumstance bonus to damage equal to double the number of weapon damage dice."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponFreeHand,
	"Trait.Weapon.FreeHand",
	"A weapon with this trait doesn't take up a character's hand, usually because it is built into the character's armor. A free-hand weapon can't be Disarmed. The character can use the hand covered by the free-hand weapon to wield other items, perform manipulate actions, and so on. They can't attack with a free-hand weapon if they're wielding anything in that hand or otherwise using that hand. When they're not wielding anything and not otherwise using the hand, they can use abilities that require them to have a hand free as well as those that require them to be wielding a weapon in that hand. Each of their hands can have only one free-hand weapon on it."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponGnome,
	"Trait.Weapon.Gnome",
	"A weapon with this trait is crafted and used by Gnomes."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponGoblin,
	"Trait.Weapon.Goblin",
	"A weapon with this trait is crafted and used by Goblins."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponHalfling,
	"Trait.Weapon.Halfling",
	"A weapon with this trait is crafted and used by Halflings."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponJoustingD6,
	"Trait.Weapon.Jousting.D6",
	"A weapon with this trait is suited for mounted combat with a harness or similar means. When mounted, if the character moved at least 3 meters on the action before their attack, add a circumstance bonus to damage for that attack equal to the number of damage dice for the weapon. In addition, while mounted, they can wield the weapon in one hand, changing the damage die to a D6."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponMonk,
	"Trait.Weapon.Monk",
	"A weapon with this trait monks often learn to use."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponNonlethal,
	"Trait.Weapon.Nonlethal",
	"A weapon with this trait often has a nonlethal attack, and is used to knock creatures unconscious instead of kill them. A character can use a nonlethal weapon to make a lethal attack with a –2 circumstance penalty."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponOrc,
	"Trait.Weapon.Orc",
	"A weapon with this trait is crafted and used by Orcs."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponParry,
	"Trait.Weapon.Parry",
	"A weapon with this trait can be used defensively to block attacks. While wielding this weapon, if a character's proficiency with it is trained or better, they can spend a single action to position their weapon defensively, gaining a +1 circumstance bonus to AC until the start of their next turn."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponPropulsive,
	"Trait.Weapon.Propulsive",
	"A ranged weapon with this trait enables a character to add half their Strength modifier (if positive) to damage rolls. If the character has a negative Strength modifier, they add their full Strength modifier instead."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponReach,
	"Trait.Weapon.Reach",
	"A weapon with this trait is long and can be used to attack creatures up to 3 meters away instead of only adjacent creatures. For creatures that already have reach with the limb or limbs that wield the weapon, the weapon increases their reach by 1.5 meters."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponShove,
	"Trait.Weapon.Shove",
	"A weapon with this trait can be used by a character to Shove with the Athletics skill even if they don't have a free hand. This uses the weapon's reach (if different from their own) and adds the weapon's item bonus to attack rolls as an item bonus to the Athletics check. If they critically fail a check to Shove using the weapon, they can drop the weapon to take the effects of a failure instead of a critical failure."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponSweep,
	"Trait.Weapon.Sweep",
	"A weapon with this trait makes wide sweeping or spinning attacks, making it easier to attack multiple enemies. When a character attacks with this weapon, they gain a +1 circumstance bonus to their attack roll if they already attempted to attack a different target this turn using this weapon."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponThrown3m,
	"Trait.Weapon.Thrown.3m",
	"A weapon with this trait can be thrown as a ranged attack; it is a ranged weapon when thrown. The character adds their Strength modifier to damage as they would for a melee weapon. When this trait appears on a melee weapon, it uses a range increment of 3 meters. Ranged weapons with this trait use the range increment in the weapon's Range entry."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponThrown6m,
	"Trait.Weapon.Thrown.6m",
	"A weapon with this trait can be thrown as a ranged attack; it is a ranged weapon when thrown. The character adds their Strength modifier to damage as they would for a melee weapon. When this trait appears on a melee weapon, it uses a range increment of 6 meters. Ranged weapons with this trait use the range increment in the weapon's Range entry."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponTrip,
	"Trait.Weapon.Trip",
	"A weapon with this trait can be used to Trip with the Athletics skill even if the character doesn't have a free hand. This uses the weapon's reach (if different from their own) and adds the weapon's item bonus to attack rolls as an item bonus to the Athletics check. If they critically fail a check to Trip using the weapon, they can drop the weapon to take the effects of a failure instead of a critical failure."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponTwin,
	"Trait.Weapon.Twin",
	"Two weapons with this trait are used as a pair, complementing each other. When a character attacks with a twin weapon, they add a circumstance bonus to the damage roll equal to the weapon's number of damage dice if they have previously attacked with a different weapon of the same type this turn. The weapons must be of the same type to benefit from this trait, but they don't need to have the same runes."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponTwoHandD8,
	"Trait.Weapon.Two-hand.D8",
	"A weapon with this trait can be wielded with two hands. Doing so changes its weapon damage die to a D8. This change applies to all the weapon's damage dice, such as those from striking runes."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponTwoHandD10,
	"Trait.Weapon.Two-hand.D10",
	"A weapon with this trait can be wielded with two hands. Doing so changes its weapon damage die to a D10. This change applies to all the weapon's damage dice, such as those from striking runes."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponTwoHandD12,
	"Trait.Weapon.Two-hand.D12",
	"A weapon with this trait can be wielded with two hands. Doing so changes its weapon damage die to a D12. This change applies to all the weapon's damage dice, such as those from striking runes."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponUnarmed,
	"Trait.Weapon.Unarmed",
	"An unarmed attack uses a character's body rather than a manufactured weapon. An unarmed attack isn't a weapon, though it's categorized with weapons for weapon groups, and it might have weapon traits. Since it's part of a character's body, an unarmed attack can't be Disarmed. It also doesn't take up a hand, though a fist or other grasping appendage generally works like a free-hand weapon."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponVersatileBludgeoning,
	"Trait.Weapon.Versatile.Bludgeoning",
	"A weapon with this trait can be used to deal Bludgeoning (B) damage instead of the type of damage listed in the Damage entry. The character chooses the damage type each time they make an attack."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponVersatilePiercing,
	"Trait.Weapon.Versatile.Piercing",
	"A weapon with this trait can be used to deal Piercing (P) damage instead of the type of damage listed in the Damage entry. The character chooses the damage type each time they make an attack."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponVersatileSlashing,
	"Trait.Weapon.Versatile.Slashing",
	"A weapon with this trait can be used to deal Slashing (S) damage instead of the type of damage listed in the Damage entry. The character chooses the damage type each time they make an attack."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
	Pf2TagTraitWeaponVolley9m,
	"Trait.Weapon.Volley.9m",
	"A ranged weapon with this trait is less effective at close distances. A character's attacks against targets that are at a distance of 9 meters take a –2 penalty."
)
