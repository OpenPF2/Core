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

#include "CharacterStats/PF2CharacterAttributeSet.h"

#include <AbilitySystemBlueprintLibrary.h>
#include <GameplayEffectExtension.h>

#include <GameFramework/Controller.h>

#include <Net/UnrealNetwork.h>

#include "PF2CharacterInterface.h"

#include "Libraries/PF2AbilitySystemLibrary.h"

#include "Utilities/PF2InterfaceUtilities.h"

UPF2CharacterAttributeSet::UPF2CharacterAttributeSet() :
	Experience(0.0f),
	AbBoostCount(0.0f),
	AbBoostLimit(0.0f),
	AbStrength(10.0f),
	AbStrengthModifier(0.0f),
	AbDexterity(10.0f),
	AbDexterityModifier(0.0f),
	AbConstitution(10.0f),
	AbConstitutionModifier(0.0f),
	AbIntelligence(10.0f),
	AbIntelligenceModifier(0.0f),
	AbWisdom(10.0f),
	AbWisdomModifier(0.0f),
	AbCharisma(10.0f),
	AbCharismaModifier(0.0f),
	ClassDifficultyClass(0.0f),
	Speed(1.0f),
	MaxSpeed(1.0f),
	Reach(150.0f),
	ArmorClass(10.0f),
	StFortitudeModifier(0.0f),
	StReflexModifier(0.0f),
	StWillModifier(0.0f),
	HitPoints(1.0f),
	MaxHitPoints(1.0f),
	RstPhysicalBludgeoning(0.0f),
	RstPhysicalPiercing(0.0f),
	RstPhysicalSlashing(0.0f),
	RstEnergyAcid(0.0f),
	RstEnergyCold(0.0f),
	RstEnergyElectricity(0.0f),
	RstEnergyFire(0.0f),
	RstEnergySonic(0.0f),
	RstEnergyPositive(0.0f),
	RstEnergyNegative(0.0f),
	RstEnergyForce(0.0f),
	RstAlignmentChaotic(0.0f),
	RstAlignmentEvil(0.0f),
	RstAlignmentGood(0.0f),
	RstAlignmentLawful(0.0f),
	RstMental(0.0f),
	RstPoison(0.0f),
	RstBleed(0.0f),
	RstPrecision(0.0f),
	PerceptionModifier(0.0f),
	SkAcrobaticsModifier(0.0f),
	SkArcanaModifier(0.0f),
	SkAthleticsModifier(0.0f),
	SkCraftingModifier(0.0f),
	SkDeceptionModifier(0.0f),
	SkDiplomacyModifier(0.0f),
	SkIntimidationModifier(0.0f),
	SkLore1Modifier(0.0f),
	SkLore2Modifier(0.0f),
	SkMedicineModifier(0.0f),
	SkNatureModifier(0.0f),
	SkOccultismModifier(0.0f),
	SkPerformanceModifier(0.0f),
	SkReligionModifier(0.0f),
	SkSocietyModifier(0.0f),
	SkStealthModifier(0.0f),
	SkSurvivalModifier(0.0f),
	SkThieveryModifier(0.0f),
	SpellAttackRoll(0.0f),
	SpellDifficultyClass(0.0f),
	FeAncestryFeatCount(0.0f),
	FeAncestryFeatLimit(0.0f),
	EncActionPoints(0.0f),
	EncMaxActionPoints(0.0f),
	EncReactionPoints(0.0f),
	EncMaxReactionPoints(0.0f),
	TmpDamageIncoming(0.0f)
{
	// Cache the tags to avoid lookup overhead.
	this->DamageReceivedEventTag   = PF2GameplayAbilityUtilities::GetTag(DamageReceivedEventTagName);
	this->HitPointsChangedEventTag = PF2GameplayAbilityUtilities::GetTag(HitPointsChangedEventTagName);
}

void UPF2CharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPF2CharacterAttributeSet, Experience);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbBoostCount);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbBoostLimit);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbStrength);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbStrengthModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbDexterity);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbDexterityModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbConstitution);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbConstitutionModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbIntelligence);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbIntelligenceModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbWisdom);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbWisdomModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbCharisma);
	DOREPLIFETIME(UPF2CharacterAttributeSet, AbCharismaModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, ClassDifficultyClass);
	DOREPLIFETIME(UPF2CharacterAttributeSet, Speed);
	DOREPLIFETIME(UPF2CharacterAttributeSet, MaxSpeed);
	DOREPLIFETIME(UPF2CharacterAttributeSet, Reach);
	DOREPLIFETIME(UPF2CharacterAttributeSet, ArmorClass);
	DOREPLIFETIME(UPF2CharacterAttributeSet, StFortitudeModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, StReflexModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, StWillModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, HitPoints);
	DOREPLIFETIME(UPF2CharacterAttributeSet, MaxHitPoints);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstPhysicalBludgeoning);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstPhysicalPiercing);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstPhysicalSlashing);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstEnergyAcid);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstEnergyCold);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstEnergyElectricity);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstEnergyFire);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstEnergySonic);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstEnergyPositive);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstEnergyNegative);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstEnergyForce);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstAlignmentChaotic);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstAlignmentEvil);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstAlignmentGood);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstAlignmentLawful);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstMental);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstPoison);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstBleed);
	DOREPLIFETIME(UPF2CharacterAttributeSet, RstPrecision);
	DOREPLIFETIME(UPF2CharacterAttributeSet, PerceptionModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkAcrobaticsModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkArcanaModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkAthleticsModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkCraftingModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkDeceptionModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkDiplomacyModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkIntimidationModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkLore1Modifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkLore2Modifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkMedicineModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkNatureModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkOccultismModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkPerformanceModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkReligionModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkSocietyModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkStealthModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkSurvivalModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SkThieveryModifier);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SpellAttackRoll);
	DOREPLIFETIME(UPF2CharacterAttributeSet, SpellDifficultyClass);
	DOREPLIFETIME(UPF2CharacterAttributeSet, FeAncestryFeatCount);
	DOREPLIFETIME(UPF2CharacterAttributeSet, FeAncestryFeatLimit);
	DOREPLIFETIME(UPF2CharacterAttributeSet, EncActionPoints);
	DOREPLIFETIME(UPF2CharacterAttributeSet, EncMaxActionPoints);
	DOREPLIFETIME(UPF2CharacterAttributeSet, EncReactionPoints);
	DOREPLIFETIME(UPF2CharacterAttributeSet, EncMaxReactionPoints);
	DOREPLIFETIME(UPF2CharacterAttributeSet, EncMultipleAttackPenalty);
	DOREPLIFETIME(UPF2CharacterAttributeSet, EncMaxMultipleAttackPenalty);
}

void UPF2CharacterAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, Experience, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbBoostCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbBoostCount, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbBoostLimit(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbBoostLimit, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbStrength, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbStrengthModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbStrengthModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbDexterity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbDexterity, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbDexterityModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbDexterityModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbConstitution(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbConstitution, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbConstitutionModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbConstitutionModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbIntelligence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbIntelligence, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbIntelligenceModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbIntelligenceModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbWisdom(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbWisdom, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbWisdomModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbWisdomModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbCharisma(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbCharisma, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_AbCharismaModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, AbCharismaModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_ClassDifficultyClass(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, ClassDifficultyClass, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, Speed, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_MaxSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, MaxSpeed, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_Reach(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, Reach, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_ArmorClass(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, ArmorClass, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_StFortitudeModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, StFortitudeModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_StReflexModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, StReflexModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_StWillModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, StWillModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_HitPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, HitPoints, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_MaxHitPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, MaxHitPoints, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstPhysicalBludgeoning(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstPhysicalBludgeoning, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstPhysicalPiercing(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstPhysicalPiercing, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstPhysicalSlashing(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstPhysicalSlashing, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstEnergyAcid(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstEnergyAcid, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstEnergyCold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstEnergyCold, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstEnergyElectricity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstEnergyElectricity, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstEnergyFire(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstEnergyFire, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstEnergySonic(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstEnergySonic, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstEnergyPositive(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstEnergyPositive, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstEnergyNegative(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstEnergyNegative, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstEnergyForce(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstEnergyForce, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstAlignmentChaotic(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstAlignmentChaotic, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstAlignmentEvil(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstAlignmentEvil, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstAlignmentGood(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstAlignmentGood, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstAlignmentLawful(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstAlignmentLawful, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstMental(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstMental, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstPoison(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstPoison, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstBleed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstBleed, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_RstPrecision(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, RstPrecision, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_PerceptionModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, PerceptionModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkAcrobaticsModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkAcrobaticsModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkArcanaModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkArcanaModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkAthleticsModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkAthleticsModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkCraftingModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkCraftingModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkDeceptionModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkDeceptionModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkDiplomacyModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkDiplomacyModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkIntimidationModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkIntimidationModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkLore1Modifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkLore1Modifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkLore2Modifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkLore2Modifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkMedicineModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkMedicineModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkNatureModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkNatureModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkOccultismModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkOccultismModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkPerformanceModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkPerformanceModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkReligionModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkReligionModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkSocietyModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkSocietyModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkStealthModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkStealthModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkSurvivalModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkSurvivalModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SkThieveryModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SkThieveryModifier, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SpellAttackRoll(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SpellAttackRoll, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_SpellDifficultyClass(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, SpellDifficultyClass, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_FeAncestryFeatCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, FeAncestryFeatCount, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_FeAncestryFeatLimit(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, FeAncestryFeatLimit, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_EncActionPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, EncActionPoints, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_EncMaxActionPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, EncMaxActionPoints, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_EncReactionPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, EncReactionPoints, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_EncMaxReactionPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, EncMaxReactionPoints, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_EncMultipleAttackPenalty(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, EncMultipleAttackPenalty, OldValue);
}

void UPF2CharacterAttributeSet::OnRep_EncMaxMultipleAttackPenalty(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2CharacterAttributeSet, EncMaxMultipleAttackPenalty, OldValue);
}

void UPF2CharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UPF2CharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	const FGameplayEffectSpec& EffectSpec        = Data.EffectSpec;
	const FGameplayAttribute   ModifiedAttribute = Data.EvaluatedData.Attribute;
	IPF2CharacterInterface*    TargetCharacter   = PF2GameplayAbilityUtilities::GetEffectTarget(&Data);
	float                      ValueDelta        = 0;

	Super::PostGameplayEffectExecute(Data);

	checkf(
		((TargetCharacter == nullptr) || (TargetCharacter->ToActor() == this->GetOwningActor())),
		TEXT("The target of the effect should be the owner of the attribute set that is being modified.")
	);

	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		ValueDelta = Data.EvaluatedData.Magnitude;
	}

	if (ModifiedAttribute == this->GetTmpDamageIncomingAttribute())
	{
		this->Native_OnDamageIncomingChanged(EffectSpec, TargetCharacter);
	}
	else if (ModifiedAttribute == this->GetHitPointsAttribute())
	{
		this->Native_OnHitPointsChanged(EffectSpec, TargetCharacter, ValueDelta);
	}
	else if (ModifiedAttribute == this->GetSpeedAttribute())
	{
		this->Native_OnSpeedChanged(EffectSpec, TargetCharacter, ValueDelta);
	}
	else if (ModifiedAttribute == this->GetEncMultipleAttackPenaltyAttribute())
	{
		this->Native_OnMultipleAttackPenaltyChanged(EffectSpec, TargetCharacter, ValueDelta);
	}
}

void UPF2CharacterAttributeSet::EmitGameplayEvent(const FGameplayTag&        EventTag,
                                                  const float                EventMagnitude,
                                                  IPF2CharacterInterface*    TargetCharacter,
                                                  const FGameplayEffectSpec& SourceEffectSpec) const
{
	UAbilitySystemComponent*                 OwningAsc     = this->GetOwningAbilitySystemComponent();
	FGameplayEventData                       EventPayload;
	TScriptInterface<IPF2CharacterInterface> Instigator;
	AActor*                                  DamageSource  = nullptr;

	UPF2AbilitySystemLibrary::DetermineDamageInstigatorAndSource(SourceEffectSpec, Instigator, DamageSource);

	EventPayload.EventTag       = EventTag;
	EventPayload.EventMagnitude = EventMagnitude;
	EventPayload.OptionalObject = DamageSource;
	EventPayload.ContextHandle  = SourceEffectSpec.GetContext();

	if (Instigator != nullptr)
	{
		EventPayload.Instigator = Instigator->ToActor();
	}

	if (TargetCharacter != nullptr)
	{
		EventPayload.Target = TargetCharacter->ToActor();
	}

	OwningAsc->HandleGameplayEvent(EventTag, &EventPayload);
}

void UPF2CharacterAttributeSet::Native_OnDamageIncomingChanged(const FGameplayEffectSpec& SourceEffectSpec,
                                                               IPF2CharacterInterface*    TargetCharacter)
{
	const float LocalDamage = this->GetTmpDamageIncoming();

	if (LocalDamage > 0.0f)
	{
		const float CurrentHitPoints = this->GetHitPoints();

		this->SetTmpDamageIncoming(0.0f);

		if (TargetCharacter == nullptr)
		{
			UE_LOG(
				LogPf2CoreStats,
				Warning,
				TEXT("Incoming damage ('%f') is against a target that is not an OpenPF2-compatible character."),
				LocalDamage
			);
		}
		else
		{
			const FGameplayEffectContextHandle       EffectContext = SourceEffectSpec.GetContext();
			TScriptInterface<IPF2CharacterInterface> Instigator    = nullptr;
			AActor*                                  DamageSource  = nullptr;

			const FHitResult HitResult = UAbilitySystemBlueprintLibrary::EffectContextGetHitResult(EffectContext);

			UPF2AbilitySystemLibrary::DetermineDamageInstigatorAndSource(SourceEffectSpec, Instigator, DamageSource);

			UE_LOG(
				LogPf2CoreStats,
				VeryVerbose,
				TEXT("[%s] Incoming damage: Pre-damage hit points: '%f', Damage: '%f'"),
				*(TargetCharacter->GetIdForLogs()),
				CurrentHitPoints,
				LocalDamage
			);

			TargetCharacter->Native_OnDamageReceived(
				LocalDamage,
				PF2InterfaceUtilities::FromScriptInterface(Instigator),
				DamageSource,
				GetSourceTags(SourceEffectSpec),
				HitResult
			);
		}

		// Enable condition check GAs to react to incoming damage.
		this->EmitGameplayEvent(
			DamageReceivedEventTag,
			LocalDamage,
			TargetCharacter,
			SourceEffectSpec
		);

		// We don't clamp hit points here; it gets clamped by Native_OnHitPointsChanged().
		this->SetHitPoints(CurrentHitPoints - LocalDamage);

		this->Native_OnHitPointsChanged(
			SourceEffectSpec,
			TargetCharacter,
			-LocalDamage
		);
	}
}

void UPF2CharacterAttributeSet::Native_OnHitPointsChanged(const FGameplayEffectSpec& SourceEffectSpec,
                                                          IPF2CharacterInterface*    TargetCharacter,
                                                          const float                ValueDelta)
{
	const float RawHitPoints     = this->GetHitPoints(),
	            ClampedHitPoints = FMath::Clamp(RawHitPoints, 0.0f, this->GetMaxHitPoints());

	if (RawHitPoints != ClampedHitPoints)
	{
		this->SetHitPoints(ClampedHitPoints);
	}

	if (ValueDelta == 0.0f)
	{
		UE_LOG(
			LogPf2CoreStats,
			VeryVerbose,
			TEXT("[%s] Stat. update (Hit Points): No change ('%f')."),
			*(TargetCharacter->GetIdForLogs()),
			ClampedHitPoints
		);
	}
	else
	{
		UE_LOG(
			LogPf2CoreStats,
			VeryVerbose,
			TEXT("[%s] Stat. update (Hit Points): Changed to '%f'."),
			*(TargetCharacter->GetIdForLogs()),
			ClampedHitPoints
		);

		if (TargetCharacter != nullptr)
		{
			TargetCharacter->Native_OnHitPointsChanged(ValueDelta, ClampedHitPoints, GetSourceTags(SourceEffectSpec));
		}

		this->EmitGameplayEvent(
			HitPointsChangedEventTag,
			ValueDelta,
			TargetCharacter,
			SourceEffectSpec
		);
	}
}

void UPF2CharacterAttributeSet::Native_OnSpeedChanged(const FGameplayEffectSpec& SourceEffectSpec,
                                                      IPF2CharacterInterface*    TargetCharacter,
                                                      const float                ValueDelta)
{
	const float RawSpeed     = this->GetSpeed();
	const float ClampedSpeed = FMath::Clamp(RawSpeed, 0.0f, this->GetMaxSpeed());

	if (RawSpeed != ClampedSpeed)
	{
		this->SetSpeed(ClampedSpeed);
	}

	if (ValueDelta == 0.0f)
	{
		UE_LOG(
			LogPf2CoreStats,
			VeryVerbose,
			TEXT("[%s] Stat. update (Speed): No change ('%f')."),
			*(TargetCharacter->GetIdForLogs()),
			ClampedSpeed
		);
	}
	else
	{
		UE_LOG(
			LogPf2CoreStats,
			VeryVerbose,
			TEXT("[%s] Stat. update (Speed): Changed to '%f'."),
			*(TargetCharacter->GetIdForLogs()),
			ClampedSpeed
		);

		if (TargetCharacter != nullptr)
		{
			TargetCharacter->Native_OnSpeedChanged(ValueDelta, ClampedSpeed, GetSourceTags(SourceEffectSpec));
		}
	}
}

void UPF2CharacterAttributeSet::Native_OnMultipleAttackPenaltyChanged(const FGameplayEffectSpec&    SourceEffectSpec,
                                                                      const IPF2CharacterInterface* TargetCharacter,
                                                                      const float                   ValueDelta)
{
	const float RawPenalty     = this->GetEncMultipleAttackPenalty();
	const float ClampedPenalty = FMath::Clamp(RawPenalty, this->GetEncMaxMultipleAttackPenalty(), 0.0f);

	if (RawPenalty != ClampedPenalty)
	{
		this->SetEncMultipleAttackPenalty(ClampedPenalty);
	}

	if (ValueDelta == 0.0f)
	{
		UE_LOG(
			LogPf2CoreStats,
			VeryVerbose,
			TEXT("[%s] Stat. update (Multiple Attack Penalty): No change ('%f')."),
			*(TargetCharacter->GetIdForLogs()),
			ClampedPenalty
		);
	}
	else
	{
		UE_LOG(
			LogPf2CoreStats,
			VeryVerbose,
			TEXT("[%s] Stat. update (Multiple Attack Penalty): Changed to '%f'."),
			*(TargetCharacter->GetIdForLogs()),
			ClampedPenalty
		);
	}
}
