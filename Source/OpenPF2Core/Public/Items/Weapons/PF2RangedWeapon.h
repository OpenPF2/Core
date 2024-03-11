// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//   - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//   - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//   - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
//
// Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
// as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
// file other than the material designated as Open Game Content may be reproduced in any form without written
// permission.

#pragma once

#include "Items/Weapons/PF2RangedWeaponAmmo.h"
#include "Items/Weapons/PF2Weapon.h"

#include "PF2RangedWeapon.generated.h"

/**
 * Primary data class for weapons that a character can use to attack another character from a distance.
 *
 * This is the top-most C++ base class for ranged weapons, but there are several Blueprint sub-classes of this class
 * that offer functionality that tends to be a more complete fit for your own ranged weapon implementations.
 *
 * (Though this is an abstract base class, it does not have the "Base" suffix in its name because the asset manager uses
 * the name of the closest native parent class as the asset type).
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2RangedWeapon : public UPF2Weapon
{
	GENERATED_BODY()

protected:
	/**
	 * The range increment of this ranged weapon (in meters, not feet).
	 *
	 * From the Pathfinder 2E Core Rulebook, page 279, "Range":
	 * "Ranged and thrown weapons have a range increment. Attacks with these weapons work normally up to that distance.
	 * Attack rolls beyond a weapon’s range increment take a –2 penalty for each additional multiple of that increment
	 * between you and the target. Attacks beyond the sixth range increment are impossible. For example, a shortbow
	 * takes no penalty against a target up to [18 meters] away, a –2 penalty against a target beyond [18 meters] but up
	 * to [36 meters] away, and a –4 penalty against a target beyond [36 meters] but up to [54 meters] away, and so on,
	 * up to [108 meters]."
	 *
	 * NOTE ON UNITS: The Pathfinder 2E Core Rulebook uses imperial units (feet), but Unreal Engine uses metric units,
	 * so OpenPF2 does as well. To convert to the units used in OpenPF2, multiply the provided range (in feet) by a
	 * ratio of 1.5/5. For example, if a weapon has a range of 60 feet, then its range in OpenPF2 is:
	 * 60 feet * 1.5 meters/5 feet = 18 meters.
	 */
	UPROPERTY(EditAnywhere, Category="OpenPF2 - Ranged Weapon Statistics")
	int32 RangeIncrement;

	/**
	 * The number of interactions required to reload this ranged weapon.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 279, "Reload":
	 * "While all weapons need some amount of time to get into position, many ranged weapons also need to be loaded and
	 * reloaded. This entry indicates how many Interact actions it takes to reload such weapons. This can be 0 if
	 * drawing ammunition and firing the weapon are part of the same action.
	 * ...
	 * An item with an entry of “—” [-1 in OpenPF2] must be drawn to be thrown, which usually takes an Interact action
	 * just like drawing any other weapon. Reloading a ranged weapon and drawing a thrown weapon both require a free
	 * hand. Switching your grip to free a hand and then to place your hands in the grip necessary to wield the weapon
	 * are both included in the actions you spend to reload a weapon."
	 *
	 * In OpenPF2:
	 *   - If an item takes 2 or more actions to reload, they must be performed together as an activity.
	 *   - The special value of "-1" corresponds to the drawn ("—") value from the Pathfinder 2E Core Rulebook.
	 */
	UPROPERTY(EditAnywhere, Category="OpenPF2 - Ranged Weapon Statistics")
	int32 ReloadActionsRequired;

	/**
	 * The standard type of ammo this weapon comes loaded with.
	 */
	UPROPERTY(EditAnywhere, Category="OpenPF2 - Ammunition")
	TSubclassOf<UPF2RangedWeaponAmmo> StandardAmmunitionType;

	/**
	 * The standard number of ammo units this weapon comes loaded with.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 279, "Reload":
	 * "Some entries in the ranged weapons tables are followed by an entry for the type of ammunition that weapon
	 * launches. The damage die is determined by the weapon, not the ammunition. Because that and other relevant
	 * statistics vary by weapon, ammunition entries list only the name, quantity, Price, and Bulk. Using ammunition
	 * destroys it."
	 */
	UPROPERTY(EditAnywhere, Category="OpenPF2 - Ammunition")
	int32 StandardAmmunitionCount;

	/**
	 * The type of ammo currently loaded into this ranged weapon.
	 */
	UPROPERTY(EditAnywhere, Category="OpenPF2 - Ammunition")
	TSubclassOf<UPF2RangedWeaponAmmo> LoadedAmmunitionType;

	/**
	 * The number of ammo units this weapon currently has loaded.
	 */
	UPROPERTY(EditAnywhere, Category="OpenPF2 - Ammunition")
	int32 LoadedAmmunitionCount;
};
