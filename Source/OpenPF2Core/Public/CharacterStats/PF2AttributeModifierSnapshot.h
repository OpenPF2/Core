// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "PF2AttributeModifierSnapshot.generated.h"

/**
 * A snapshot of the value of a character attribute and its modifier.
 */
USTRUCT(BlueprintType)
struct OPENPF2CORE_API FPF2AttributeModifierSnapshot
{
	GENERATED_BODY()

	/**
	 * The value of the score itself.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ScoreValue;

	/**
	 * The modifier calculated from the score.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ModifierValue;

	/**
	 * Default constructor for FPF2AttributeModifierSnapshot.
	 */
	explicit FPF2AttributeModifierSnapshot() : FPF2AttributeModifierSnapshot(0.0f, 0.0f)
	{
	}

	/**
	 * Constructor for FPF2AttributeModifierSnapshot.
	 *
	 * @param ScoreValue
	 *	The value of the score itself.
	 * @param ModifierValue
	 *	The modifier calculated from the score.
	 */
	explicit FPF2AttributeModifierSnapshot(const float ScoreValue, const float ModifierValue) :
		ScoreValue(ScoreValue), ModifierValue(ModifierValue)
	{
	};
};
