// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "CharacterStats/PF2CharacterAttributeStaticsBase.h"

#include "GameplayEffectTypes.h"

#include "Utilities/PF2ArrayUtilities.h"

TArray<const FGameplayEffectAttributeCaptureDefinition*> FPF2CharacterAttributeStaticsBase::GetAllAbilityScoreCaptures() const
{
	return PF2ArrayUtilities::Map<const FGameplayEffectAttributeCaptureDefinition*>(
		this->GetAbilityNames(),
		[this](const FString& AbilityScoreAttributeName)
		{
			return this->GetCaptureByAttributeName(AbilityScoreAttributeName);
		}
	);
}

const FGameplayEffectAttributeCaptureDefinition* FPF2CharacterAttributeStaticsBase::GetResistanceCaptureForDamageType(
	const FName& DamageTypeName) const
{
	const FGameplayEffectAttributeCaptureDefinition* Result = nullptr;

	if (this->DamageTypeToResistanceAttributeMap.Contains(DamageTypeName))
	{
		const FName ResistanceAttributeName = this->DamageTypeToResistanceAttributeMap[DamageTypeName];

		Result = this->GetCaptureByAttributeName(ResistanceAttributeName.ToString());
	}
	else
	{
		UE_LOG(
			LogPf2CoreStats,
			Error,
			TEXT("No resistance attribute corresponds to damage type '%s'."),
			*(DamageTypeName.ToString())
		);
	}

	return Result;
}

TArray<const FGameplayEffectAttributeCaptureDefinition*> FPF2CharacterAttributeStaticsBase::GetAllResistanceCaptures() const
{
	TArray<FName> ResistanceAttributeNames;

	this->DamageTypeToResistanceAttributeMap.GenerateValueArray(ResistanceAttributeNames);

	return PF2ArrayUtilities::ReduceToArray<const FGameplayEffectAttributeCaptureDefinition*>(
		ResistanceAttributeNames,
		[this](TArray<const FGameplayEffectAttributeCaptureDefinition*> Captures, const FName& ResistanceAttributeName)
		{
			const FGameplayEffectAttributeCaptureDefinition* ResistanceCapture =
				this->CaptureDefinitions[ResistanceAttributeName.ToString()];

			Captures.Add(ResistanceCapture);
		}
	);
}

FPF2CharacterAttributeStaticsBase::FPF2CharacterAttributeStaticsBase():
	AbBoostCountProperty(nullptr),
	AbCharismaProperty(nullptr),
	AbCharismaModifierProperty(nullptr),
	AbConstitutionProperty(nullptr),
	AbConstitutionModifierProperty(nullptr),
	AbDexterityProperty(nullptr),
	AbDexterityModifierProperty(nullptr),
	AbIntelligenceProperty(nullptr),
	AbIntelligenceModifierProperty(nullptr),
	AbStrengthProperty(nullptr),
	AbStrengthModifierProperty(nullptr),
	AbWisdomProperty(nullptr),
	AbWisdomModifierProperty(nullptr),
	ArmorClassProperty(nullptr),
	HitPointsProperty(nullptr),
	RstPhysicalBludgeoningProperty(nullptr),
	RstPhysicalPiercingProperty(nullptr),
	RstPhysicalSlashingProperty(nullptr),
	RstEnergyAcidProperty(nullptr),
	RstEnergyColdProperty(nullptr),
	RstEnergyElectricityProperty(nullptr),
	RstEnergyFireProperty(nullptr),
	RstEnergySonicProperty(nullptr),
	RstEnergyPositiveProperty(nullptr),
	RstEnergyNegativeProperty(nullptr),
	RstEnergyForceProperty(nullptr),
	RstAlignmentChaoticProperty(nullptr),
	RstAlignmentEvilProperty(nullptr),
	RstAlignmentGoodProperty(nullptr),
	RstAlignmentLawfulProperty(nullptr),
	RstMentalProperty(nullptr),
	RstPoisonProperty(nullptr),
	RstBleedProperty(nullptr),
	RstPrecisionProperty(nullptr),
	EncMultipleAttackPenaltyProperty(nullptr)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	TArray<FName> Keys;

	this->DamageTypeToResistanceAttributeMap.GetKeys(Keys);

	// Validate that all tag names are valid.
	for (const FName& CurrentTagName : Keys)
	{
		// Rather than crashing the game/engine, we soften this to a log error so that a game designer can still
		// correct the error by possibly loading/defining tags.
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(CurrentTagName, false);

		if (!Tag.IsValid())
		{
			UE_LOG(
				LogPf2CoreStats,
				Error,
				TEXT("The damage type tag '%s' is missing."),
				*(CurrentTagName.ToString())
			);
		}
	}
#endif
}
