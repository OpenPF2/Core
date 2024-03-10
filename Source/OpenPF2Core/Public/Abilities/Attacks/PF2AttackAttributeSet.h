// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
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

#pragma once

#include <AbilitySystemComponent.h>
#include <AttributeSet.h>

#include "CharacterStats/PF2AttributeSetMacros.h"

#include "PF2AttackAttributeSet.generated.h"

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * An attribute set for tracking transient stats during attacks, including die rolls, die sizes, and damage types.
 */
UCLASS()
class OPENPF2CORE_API UPF2AttackAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Attributes
	// =================================================================================================================

	// Transient Attributes for Attack Rolls ---------------------------------------------------------------------------
	/**
	 * Temporary attribute for tracking the number of dice to use for the current attack roll.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Attack Rolls")
	FGameplayAttributeData TmpAttackRollCount;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpAttackRollCount)

	/**
	 * Temporary attribute for tracking the size dice to use for the current attack roll.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Attack Rolls")
	FGameplayAttributeData TmpAttackRollSize;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpAttackRollSize)

	/**
	 * Temporary attribute for tracking the success of the current attack roll.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Attack Rolls")
	FGameplayAttributeData TmpAttackDegreeOfSuccess;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpAttackDegreeOfSuccess)

	// Transient Attributes for Damage Rolls ---------------------------------------------------------------------------
	/**
	 * Temporary attribute for tracking the number of dice to use for the current damage roll.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Damage Rolls")
	FGameplayAttributeData TmpDmgRollCount;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgRollCount)

	/**
	 * Temporary attribute for tracking the size dice to use for the current damage roll.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Damage Rolls")
	FGameplayAttributeData TmpDmgRollSize;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgRollSize)

	// Transient Attributes for Outgoing Damage ------------------------------------------------------------------------
	/**
	 * Temporary attribute for the Bludgeoning damage (DamageType.Physical.Bludgeoning) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Bludgeoning (B) damage comes from weapons and hazards that deal blunt-force trauma, like a hit from a club or
	 * being dashed against rocks."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypePhysicalBludgeoning;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypePhysicalBludgeoning)

	/**
	 * Temporary attribute for the Piercing damage (DamageType.Physical.Piercing) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Piercing (P) damage is dealt from stabs and punctures, whether from a dragon's fangs or the thrust of a spear."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypePhysicalPiercing;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypePhysicalPiercing)

	/**
	 * Temporary attribute for the Slashing damage (DamageType.Physical.Slashing) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Slashing (S) damage is delivered by a cut, be it the swing of the sword or the blow from a scythe blades trap."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypePhysicalSlashing;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypePhysicalSlashing)

	/**
	 * Temporary attribute for the Acid damage (DamageType.Energy.Acid) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Acid damage can be delivered by gases, liquids, and certain solids that dissolve flesh, and sometimes harder
	 * materials."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeEnergyAcid;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeEnergyAcid)

	/**
	 * Temporary attribute for the Cold damage (DamageType.Energy.Cold) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Cold damage freezes material by way of contact with chilling gases and ice. Electricity damage comes from the
	 * discharge of powerful lightning and sparks."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeEnergyCold;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeEnergyCold)

	/**
	 * Temporary attribute for the Electricity damage (DamageType.Energy.Electricity) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Electricity comes from the discharge of powerful lightning and sparks."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeEnergyElectricity;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeEnergyElectricity)

	/**
	 * Temporary attribute for the Fire damage (DamageType.Energy.Fire) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Fire damage burns through heat and combustion."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeEnergyFire;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeEnergyFire)

	/**
	 * Temporary attribute for the Sonic damage (DamageType.Energy.Sonic) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Sonic damage assaults matter with high-frequency vibration and sound waves."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeEnergySonic;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeEnergySonic)

	/**
	 * Temporary attribute for the Positive damage (DamageType.Energy.Positive) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Positive damage harms only undead creatures, withering undead bodies and disrupting incorporeal undead."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeEnergyPositive;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeEnergyPositive)

	/**
	 * Temporary attribute for the Negative damage (DamageType.Energy.Negative) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Negative damage saps life, damaging only living creatures."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeEnergyNegative;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeEnergyNegative)

	/**
	 * Temporary attribute for the Force damage (DamageType.Energy.Force) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Powerful and pure magical energy can manifest itself as force damage. Few things can resist this type of
	 * damage—not even incorporeal creatures such as ghosts and wraiths."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeEnergyForce;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeEnergyForce)

	/**
	 * Temporary attribute for the Chaotic damage (DamageType.Alignment.Chaotic) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Chaotic damage harms only lawful creatures."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeAlignmentChaotic;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeAlignmentChaotic)

	/**
	 * Temporary attribute for the Evil damage (DamageType.Alignment.Evil) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Evil damage harms only good creatures."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeAlignmentEvil;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeAlignmentEvil)

	/**
	 * Temporary attribute for the Good damage (DamageType.Alignment.Good) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Good damage harms only evil creatures."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeAlignmentGood;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeAlignmentGood)

	/**
	 * Temporary attribute for the Lawful damage (DamageType.Alignment.Lawful) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Lawful damage harms only chaotic creatures."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeAlignmentLawful;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeAlignmentLawful)

	/**
	 * Temporary attribute for the Mental damage (DamageType.Mental) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Sometimes an effect can target the mind with enough psychic force to actually deal damage to the creature. When
	 * it does, it deals mental damage. Mindless creatures and those with only programmed or rudimentary intelligence
	 * are often immune to mental damage and effects."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeMental;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeMental)

	/**
	 * Temporary attribute for the Poison damage (DamageType.Poison) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Venoms, toxins and the like can deal poison damage, which affects creatures by way of contact, ingestion,
	 * inhalation, or injury. In addition to coming from monster attacks, alchemical items, and spells, poison damage is
	 * often caused by ongoing afflictions, which follow special rules."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypePoison;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypePoison)

	/**
	 * Temporary attribute for the Bleed damage (DamageType.Bleed) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "This is persistent damage that represents loss of blood. As such, it has no effect on nonliving creatures or
	 * living creatures that don't need blood to live."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypeBleed;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypeBleed)

	/**
	 * Temporary attribute for the Precision damage (DamageType.Precision) the current attack will inflict.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "When a character hits with an ability that grants precision damage, the character increase the attack's listed
	 * damage, using the same damage type, rather than tracking a separate pool of damage."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Outgoing Damage")
	FGameplayAttributeData TmpDmgTypePrecision;
	ATTRIBUTE_ACCESSORS(UPF2AttackAttributeSet, TmpDmgTypePrecision)

	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	explicit UPF2AttackAttributeSet() :
		TmpAttackRollCount(1.0f),
		TmpAttackRollSize(20.0f),
		TmpAttackDegreeOfSuccess(0.0f),
		TmpDmgRollCount(0.0f),
		TmpDmgRollSize(0.0f),
		TmpDmgTypePhysicalBludgeoning(0.0f),
		TmpDmgTypePhysicalPiercing(0.0f),
		TmpDmgTypePhysicalSlashing(0.0f),
		TmpDmgTypeEnergyAcid(0.0f),
		TmpDmgTypeEnergyCold(0.0f),
		TmpDmgTypeEnergyElectricity(0.0f),
		TmpDmgTypeEnergyFire(0.0f),
		TmpDmgTypeEnergySonic(0.0f),
		TmpDmgTypeEnergyPositive(0.0f),
		TmpDmgTypeEnergyNegative(0.0f),
		TmpDmgTypeEnergyForce(0.0f),
		TmpDmgTypeAlignmentChaotic(0.0f),
		TmpDmgTypeAlignmentEvil(0.0f),
		TmpDmgTypeAlignmentGood(0.0f),
		TmpDmgTypeAlignmentLawful(0.0f),
		TmpDmgTypeMental(0.0f),
		TmpDmgTypePoison(0.0f),
		TmpDmgTypeBleed(0.0f),
		TmpDmgTypePrecision(0.0f)
	{
	}

};
