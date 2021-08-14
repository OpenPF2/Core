﻿// OpenPF2 for UE Game Logic, Copyright 2021, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Calculations/Modifiers/Skills/PF2ThieveryModifierCalculation.h"

#include "Abilities/PF2AttributeSet.h"

UPF2ThieveryModifierCalculation::UPF2ThieveryModifierCalculation() :
	UPF2ModifierCalculationBase(
		UPF2AttributeSet::GetAbDexterityModifierAttribute(),
		FString("Skill.Thievery")
	)
{
}