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

#include "PF2MonetaryValue.h"

#include "Items/PF2Item.h"

#include "PF2RangedWeaponAmmo.generated.h"

/**
 * Primary data class for ammo used by ranged weapons.
 *
 * (Though this is an abstract base class, it does not have the "Base" suffix in its name because the asset manager uses
 * the name of the closest native parent class as the asset type).
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2RangedWeaponAmmo : public UPF2Item
{
	GENERATED_BODY()

protected:
	/**
	 * The human-friendly name for this ammo.
	 */
	UPROPERTY(EditAnywhere, Category="OpenPF2 - Ammo Statistics")
	FText Name;

	/**
	 * The value or price of this ammo.
	 */
	UPROPERTY(EditDefaultsOnly, Category="OpenPF2 - Ammo Statistics")
	FPF2MonetaryValue Value;

	/**
	 * How large or bulky this ammo is to carry.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 629, "Bulk":
	 * "[Bulk is a] value indicating an item's size, weight, and general awkwardness. It takes 10 items of light Bulk to
	 * equal 1 Bulk [...] A character becomes encumbered if they carry Bulk greater than 5 + their Str modifier, and
	 * they can't carry more than 10 + their Str modifier."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 271-272, "Bulk":
	 * "Carrying especially heavy or unwieldy items can make it more difficult for you to move, as can overloading
	 * yourself with too much gear. The Bulk value of an item reflects how difficult the item is to handle,
	 * representing its size, weight, and general awkwardness. If you have a high Strength score, you usually don't need
	 * to worry about Bulk unless you're carrying numerous substantial items."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 272, "Bulk Values":
	 * "Items can have a number to indicate their Bulk value, or they can be light (indicated by an L) or negligible
	 * (indicated by a —) for the purpose of determining Bulk. For instance, full plate armor is 4 Bulk, a longsword is
	 * 1 Bulk, a dagger or scroll is light, and a piece of chalk is negligible. Ten light items count as 1 Bulk, and you
	 * round down fractions (so 9 light items count as 0 Bulk, and 11 light items count as 1 Bulk). Items of negligible
	 * Bulk don't count toward Bulk unless you try to carry vast numbers of them, as determined by the GM."
	 *
	 * In OpenPF2, the following Bulk values correspond to special values from the Pathfinder 2E Core Rulebook:
	 *	- "0.01" corresponds to "negligible" (—) bulk.
	 *	- "0.10" corresponds to "light" (L) bulk.
	 */
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0), Category="OpenPF2 - Ammo Statistics")
	float Bulk;
};
