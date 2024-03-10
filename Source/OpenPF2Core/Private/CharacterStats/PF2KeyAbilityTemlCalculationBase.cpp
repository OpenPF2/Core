// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
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

#include "CharacterStats/PF2KeyAbilityTemlCalculationBase.h"

#include "CharacterStats/PF2CharacterAttributeSet.h"
#include "CharacterStats/PF2TemlCalculation.h"

#include "Libraries/PF2AbilitySystemLibrary.h"

#include "Utilities/PF2GameplayAbilityUtilities.h"

UPF2KeyAbilityTemlCalculationBase::UPF2KeyAbilityTemlCalculationBase() :
	UPF2KeyAbilityTemlCalculationBase(
		TEXT(""),
		TEXT("KeyAbility")
	)
{
}

UPF2KeyAbilityTemlCalculationBase::UPF2KeyAbilityTemlCalculationBase(
	const FString& StatGameplayTagPrefix,
	const FString& KeyAbilityGameplayTagPrefix,
	const float    BaseValue) :
	StatGameplayTagPrefix(StatGameplayTagPrefix),
	BaseValue(BaseValue)
{
	this->DefineKeyAbilityCapture(
		KeyAbilityGameplayTagPrefix + ".Strength",
		UPF2CharacterAttributeSet::GetAbStrengthModifierAttribute()
	);

	this->DefineKeyAbilityCapture(
		KeyAbilityGameplayTagPrefix + ".Dexterity",
		UPF2CharacterAttributeSet::GetAbDexterityModifierAttribute()
	);

	this->DefineKeyAbilityCapture(
		KeyAbilityGameplayTagPrefix + ".Constitution",
		UPF2CharacterAttributeSet::GetAbConstitutionModifierAttribute()
	);

	this->DefineKeyAbilityCapture(
		KeyAbilityGameplayTagPrefix + ".Intelligence",
		UPF2CharacterAttributeSet::GetAbIntelligenceModifierAttribute()
	);

	this->DefineKeyAbilityCapture(
		KeyAbilityGameplayTagPrefix + ".Wisdom",
		UPF2CharacterAttributeSet::GetAbWisdomModifierAttribute()
	);

	this->DefineKeyAbilityCapture(
		KeyAbilityGameplayTagPrefix + ".Charisma",
		UPF2CharacterAttributeSet::GetAbCharismaModifierAttribute()
	);
}

void UPF2KeyAbilityTemlCalculationBase::DefineKeyAbilityCapture(
	const FString&            KeyAbilityTagName,
	const FGameplayAttribute& Attribute)
{
	const FGameplayEffectAttributeCaptureDefinition CaptureDefinition =
		PF2GameplayAbilityUtilities::BuildSourceCaptureFor(Attribute);

	this->KeyAbilityCaptureDefinitions.Add(
		KeyAbilityTagName,
		CaptureDefinition
	);

	this->RelevantAttributesToCapture.Add(CaptureDefinition);
}

float UPF2KeyAbilityTemlCalculationBase::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Logic shared by the "Class DC", "Spell Attack Roll", and "Spell DC" calculations.
	// "A class DC ... equals 10 plus their proficiency bonus for their class DC (+3 for most 1st-level characters) plus
	// the modifier for the class’s key ability score."
	//
	// Source: Pathfinder 2E Core Rulebook, page 29, "Class DC".
	//
	//
	// "Spell attack roll = your spellcasting ability modifier + proficiency bonus + other bonuses + penalties
	// Spell DC = 10 + your spellcasting ability modifier + proficiency bonus + other bonuses + penalties"
	//
	// Source: Pathfinder 2E Core Rulebook, page 298, "Spell Attack Roll and Spell DC".
	const float ProficiencyBonus   = FPF2TemlCalculation(this->StatGameplayTagPrefix, Spec).GetValue(),
	            KeyAbilityModifier = this->CalculateKeyAbilityModifier(Spec),
	            AbilityScore       = this->BaseValue + ProficiencyBonus + KeyAbilityModifier;

	UE_LOG(
		LogPf2Core,
		VeryVerbose,
		TEXT("Calculated key ability score ('%s'): %f + %f + %f = %f"),
		*(this->StatGameplayTagPrefix),
		this->BaseValue,
		ProficiencyBonus,
		KeyAbilityModifier,
		AbilityScore
	);

	return AbilityScore;
}

float UPF2KeyAbilityTemlCalculationBase::CalculateKeyAbilityModifier(const FGameplayEffectSpec& Spec) const
{
	float                        KeyAbilityModifier = 0.0f;
	const FGameplayTagContainer* SourceTags         = Spec.CapturedSourceTags.GetAggregatedTags();

	const FGameplayEffectAttributeCaptureDefinition KeyAbilityCaptureDefinition =
		this->DetermineKeyAbility(SourceTags);

	if (KeyAbilityCaptureDefinition.AttributeToCapture.IsValid())
	{
		const FAggregatorEvaluateParameters EvaluationParameters =
			UPF2AbilitySystemLibrary::BuildEvaluationParameters(Spec);

		this->GetCapturedAttributeMagnitude(
			KeyAbilityCaptureDefinition,
			Spec,
			EvaluationParameters,
			KeyAbilityModifier
		);
	}

	return KeyAbilityModifier;
}

FGameplayEffectAttributeCaptureDefinition UPF2KeyAbilityTemlCalculationBase::DetermineKeyAbility(
	const FGameplayTagContainer* SourceTags) const
{
	FGameplayEffectAttributeCaptureDefinition KeyAbilityCaptureDefinition;

	for (auto PairIterator = this->KeyAbilityCaptureDefinitions.CreateConstIterator(); PairIterator; ++PairIterator)
	{
		const FString TagName = PairIterator.Key();

		if (PF2GameplayAbilityUtilities::HasTag(SourceTags, TagName))
		{
			KeyAbilityCaptureDefinition = PairIterator.Value();
			break;
		}
	}

	return KeyAbilityCaptureDefinition;
}
