// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
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

#include "PF2DegreeOfSuccess.generated.h"

/**
 * An enumeration of the degrees of success/outcomes possible after performing a check or attack roll.
 *
 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 3: Compare the Result to the DC":
 * "[...] if your result is equal to or greater than the DC, you succeed! If your roll anything less than the DC,
 * you fail."
 *
 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 4: Determine the Degree of Success and Effect":
 * "Many times, it’s important to determine not only if you succeed or fail, but also how spectacularly you succeed or
 * fail. Exceptional results—either good or bad—can cause you to critically succeed at or critically fail a check.
 *
 * You critically succeed at a check when the check’s result meets or exceeds the DC by 10 or more. If the check is an
 * attack roll, this is sometimes called a critical hit. You can also critically fail a check. The rules for critical
 * failure—sometimes called a fumble—are the same as those for a critical success, but in the other direction: if you
 * fail a check by 10 or more, that’s a critical failure.
 *
 * If you rolled a 20 on the die (a “natural 20”), your result is one degree of success better than it would be by
 * numbers alone. If you roll a 1 on the d20 (a “natural 1”), your result is one degree worse."
 */
UENUM(BlueprintType)
enum class EPF2DegreeOfSuccess : uint8
{
	/**
	 * No result could be calculated (typically for error cases).
	 */
	None,

	/**
	 * The check result was equal to or lower than the DC by 10 or more.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 4: Determine the Degree of Success and Effect":
	* "[...] if you fail a check by 10 or more, that’s a critical failure"
	 */
	CriticalFailure,

	/**
	 * The check result did not exceed the DC.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 3: Compare the Result to the DC":
	 * "If [you] roll anything less than the DC, you fail."
	 */
	Failure,

	/**
	 * The check result exceeded the DC.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 3: Compare the Result to the DC":
	 * "[...] if your result is equal to or greater than the DC, you succeed!"
	 */
	Success,

	/**
	 * The check result met or exceeded the DC by 10 or more.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 445, "Step 4: Determine the Degree of Success and Effect":
	 * "You critically succeed at a check when the check’s result meets or exceeds the DC by 10 or more. If the check is
	 * an attack roll, this is sometimes called a critical hit."
	 */
	CriticalSuccess,
};
