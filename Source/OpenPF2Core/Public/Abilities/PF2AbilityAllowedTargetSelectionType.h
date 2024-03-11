// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/ObjectMacros.h>

/**
 * An enumeration of the type of selection that a player is allowed to make when invoking a particular ability.
 */
UENUM(BlueprintType)
enum class EPF2AbilityAllowedTargetSelectionType : uint8
{
	/**
	 * Either a location or character can be selected.
	 */
	LocationOrCharacter,

	/**
	 * Only a location can be selected.
	 */
	OnlyLocation,

	/**
	 * Only a character can be selected.
	 */
	OnlyCharacter,
};
