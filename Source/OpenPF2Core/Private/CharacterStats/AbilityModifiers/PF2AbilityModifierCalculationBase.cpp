// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "CharacterStats/AbilityModifiers/PF2AbilityModifierCalculationBase.h"

#include "CharacterStats/PF2CharacterAttributeSet.h"

#include "Libraries/PF2CharacterStatLibrary.h"

float UPF2AbilityModifierCalculationBase::DoCalculation(
	const FGameplayEffectSpec& Spec,
	const FGameplayAttribute   AbilityAttribute,
	const float                AbilityScore) const
{
	float AbilityModifier;

	AbilityModifier = UPF2CharacterStatLibrary::CalculateAbilityModifier(AbilityScore);

	UE_LOG(
		LogPf2CoreStats,
		VeryVerbose,
		TEXT("Calculated ability modifier for attribute ('%s': %f): %f"),
		*(AbilityAttribute.GetName()),
		AbilityScore,
		AbilityModifier
	);

	return AbilityModifier;
}
