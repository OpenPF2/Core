// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "CharacterStats/PF2SourceCharacterAttributeStatics.h"

FPF2SourceCharacterAttributeStatics::FPF2SourceCharacterAttributeStatics()
{
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, AbBoostCount, Source, true);

	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbCharisma, Source, true);
	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbConstitution, Source, true);
	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbDexterity, Source, true);
	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbIntelligence, Source, true);
	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbStrength, Source, true);
	DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(UPF2CharacterAttributeSet, AbWisdom, Source, true);

	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbCharismaModifier, Source, true);
	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbConstitutionModifier, Source, true);
	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbDexterityModifier, Source, true);
	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbIntelligenceModifier, Source, true);
	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbStrengthModifier, Source, true);
	DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(UPF2CharacterAttributeSet, AbWisdomModifier, Source, true);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, ArmorClass, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, HitPoints, Source, true);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstPhysicalBludgeoning, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstPhysicalPiercing, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstPhysicalSlashing, Source, true);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyAcid, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyCold, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyElectricity, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyFire, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergySonic, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyPositive, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyNegative, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstEnergyForce, Source, true);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstAlignmentChaotic, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstAlignmentEvil, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstAlignmentGood, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstAlignmentLawful, Source, true);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstMental, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstPoison, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstBleed, Source, true);
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, RstPrecision, Source, true);

	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(UPF2CharacterAttributeSet, EncMultipleAttackPenalty, Source, true);
}
