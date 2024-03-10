// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "CharacterStats/PF2TargetCharacterAttributeStatics.h"

const FGameplayEffectAttributeCaptureDefinition* FPF2TargetCharacterAttributeStatics::GetResistanceCaptureForDamageAttribute(
	const FName& DamageAttributeName) const
{
	const FGameplayEffectAttributeCaptureDefinition* Result = nullptr;

	if (this->DamageAttributeToResistanceAttributeMap.Contains(DamageAttributeName))
	{
		const FName ResistanceAttributeName = this->DamageAttributeToResistanceAttributeMap[DamageAttributeName];

		Result = this->GetCaptureByAttributeName(ResistanceAttributeName.ToString());
	}
	else
	{
		UE_LOG(
			LogPf2CoreStats,
			Error,
			TEXT("No resistance attribute corresponds to damage attribute '%s'."),
			*(DamageAttributeName.ToString())
		);
	}

	return Result;
}

FPF2TargetCharacterAttributeStatics::FPF2TargetCharacterAttributeStatics():
	TmpDamageIncomingProperty(nullptr),
	TmpLastIncomingAttackDegreeOfSuccessProperty(nullptr)
{
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, AbBoostCount, Target, false);

	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbCharisma, Target, false);
	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbConstitution, Target, false);
	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbDexterity, Target, false);
	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbIntelligence, Target, false);
	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbStrength, Target, false);
	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbWisdom, Target, false);

	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbCharismaModifier, Target, false);
	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbConstitutionModifier, Target, false);
	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbDexterityModifier, Target, false);
	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbIntelligenceModifier, Target, false);
	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbStrengthModifier, Target, false);
	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbWisdomModifier, Target, false);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, ArmorClass, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, HitPoints, Target, false);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstPhysicalBludgeoning, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstPhysicalPiercing, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstPhysicalSlashing, Target, false);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyAcid, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyCold, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyElectricity, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyFire, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergySonic, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyPositive, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyNegative, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyForce, Target, false);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstAlignmentChaotic, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstAlignmentEvil, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstAlignmentGood, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstAlignmentLawful, Target, false);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstMental, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstPoison, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstBleed, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstPrecision, Target, false);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, TmpDamageIncoming, Target, false);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, TmpLastIncomingAttackDegreeOfSuccess, Target, false);
}
