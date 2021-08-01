﻿// OpenPF2 for UE Game Logic, Copyright 2021, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <CoreMinimal.h>

#include "Calculations/Modifiers/RPGModifierCalculationBase.h"
#include "RPGWillModifierCalculation.generated.h"

/**
* MMC that calculates the modifier a character has in Will Saving Throws.
*/
UCLASS()
class TEENWOLFRPG_API URPGWillModifierCalculation : public URPGModifierCalculationBase
{
	GENERATED_BODY()

public:
	explicit URPGWillModifierCalculation();
};
