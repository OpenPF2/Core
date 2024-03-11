// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "CharacterStats/AbilityBoosts/PF2AbilityBoostCalculationBase.h"

#include "CharacterStats/PF2CharacterAttributeSet.h"

#include "Libraries/PF2CharacterStatLibrary.h"

float UPF2AbilityBoostCalculationBase::DoCalculation(
	const FGameplayEffectSpec& Spec,
	const FGameplayAttribute   AbilityAttribute,
	const float                AbilityScore) const
{
	const float Boost = UPF2CharacterStatLibrary::CalculateAbilityBoostAmount(AbilityScore);

	UE_LOG(
		LogPf2Core,
		VeryVerbose,
		TEXT("Calculated MMC boost for ability score attribute ('%s'): %f + %f = %f"),
		*(AbilityAttribute.GetName()),
		AbilityScore,
		Boost,
		AbilityScore + Boost
	);

	return Boost;
}
