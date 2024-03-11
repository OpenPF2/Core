// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/ObjectMacros.h>

/**
 * An enumeration of the type of selection a player has made when invoking a particular ability.
 */
UENUM(BlueprintType)
enum class EPF2TargetSelectionType : uint8
{
	/**
	 * The player has made no selection.
	 */
	None,

	/**
	 * An unpopulated location has been selected.
	 */
	Location,

	/**
	 * A character (either friend or foe) has be selected.
	 */
	Character,
};
