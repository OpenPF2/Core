﻿// OpenPF2 Game Framework for Unreal Engine, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayModMagnitudeCalculation.h>

#include "PF2AncestryFeatCapCalculation.generated.h"

/**
 * An MMC for calculating how many ancestry feats a character is entitled to have at their current level.
 */
UCLASS()
class OPENPF2GAMEFRAMEWORK_API UPF2AncestryFeatCapCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods - UGameplayModMagnitudeCalculation Implementation
	// =================================================================================================================
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
