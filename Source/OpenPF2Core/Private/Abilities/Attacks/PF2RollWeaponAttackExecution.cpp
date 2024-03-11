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

#include "Abilities/Attacks/PF2RollWeaponAttackExecution.h"

#include "PF2CharacterInterface.h"

#include "Abilities/Attacks/PF2AttackAttributeStatics.h"

#include "CharacterStats/PF2CharacterAbilitySystemInterface.h"
#include "CharacterStats/PF2SourceCharacterAttributeStatics.h"
#include "CharacterStats/PF2TargetCharacterAttributeStatics.h"

#include "Items/Weapons/PF2WeaponInterface.h"

#include "Libraries/PF2AbilitySystemLibrary.h"
#include "Libraries/PF2AttackStatLibrary.h"

void UPF2RollWeaponAttackExecution::AttemptAttack(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                  const IPF2WeaponInterface*                      Weapon,
                                                  const IPF2CharacterAbilitySystemInterface*      SourceAscIntf,
                                                  const IPF2CharacterAbilitySystemInterface*      TargetAscIntf,
                                                  FGameplayEffectCustomExecutionOutput&           OutExecutionOutput)
{
	float                             TargetAc;
	EPF2DegreeOfSuccess               AttackRollResult;
	const FPF2AttackAttributeStatics& AttackCaptures    = FPF2AttackAttributeStatics::GetInstance();

	const FAggregatorEvaluateParameters EvaluationParameters =
		UPF2AbilitySystemLibrary::BuildEvaluationParameters(ExecutionParams);

	UE_LOG(
		LogPf2CoreAbilities,
		Verbose,
		TEXT("Character ('%s') attempting attack with weapon ('%s') against character ('%s')."),
		*(SourceAscIntf->GetCharacter()->GetIdForLogs()),
		*(Weapon->GetIdForLogs()),
		*(TargetAscIntf->GetCharacter()->GetIdForLogs())
	);

	TargetAc         = GetTargetArmorClass(ExecutionParams, EvaluationParameters);
	AttackRollResult = PerformAttackRoll(ExecutionParams, EvaluationParameters, Weapon, SourceAscIntf, TargetAc);

	// "When the result of your attack roll with a weapon or unarmed attack equals or exceeds your target’s AC, you hit
	// your target!"
	//
	// Source: Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Damage Rolls".
	if (UPF2AttackStatLibrary::IsSuccess(AttackRollResult))
	{
		const FGameplayTag       DamageTypeTag    = Weapon->GetDamageType();
		const float              DamageRoll       = PerformDamageRoll(ExecutionParams, EvaluationParameters, Weapon);
		float                    DamageMultiplier,
		                         DamageAmount;

		const FGameplayEffectAttributeCaptureDefinition* DamageCapture =
			AttackCaptures.GetDamageCaptureForDamageType(DamageTypeTag);

		// "When you make an attack and succeed with a natural 20 (the number on the die is 20), or if the result of
		// your attack exceeds the target’s AC by 10, you achieve a critical success (also known as a critical hit)."
		//
		// Source: Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Critical Hits".
		if (AttackRollResult == EPF2DegreeOfSuccess::CriticalSuccess)
		{
			DamageMultiplier = 2.0f;
		}
		else
		{
			DamageMultiplier = 1.0f;
		}

		DamageAmount = DamageRoll * DamageMultiplier;

		UE_LOG(
			LogPf2CoreStats,
			VeryVerbose,
			TEXT("Damage Roll (%f) * Damage Multiplier (%f) = %f (%s)."),
			DamageRoll,
			DamageMultiplier,
			DamageAmount,
			*(DamageTypeTag.ToString())
		);

		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				DamageCapture->AttributeToCapture,
				EGameplayModOp::Additive,
				DamageAmount
			)
		);
	}

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			AttackCaptures.TmpAttackDegreeOfSuccessProperty,
			EGameplayModOp::Override,
			UPF2AttackStatLibrary::DegreeOfSuccessStatFromEnum(AttackRollResult)
		)
	);
}

EPF2DegreeOfSuccess UPF2RollWeaponAttackExecution::PerformAttackRoll(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters&            EvaluationParameters,
	const IPF2WeaponInterface*                      Weapon,
	const IPF2CharacterAbilitySystemInterface*      SourceAsc,
	const float                                     TargetArmorClass)
{
	EPF2DegreeOfSuccess                 Result;
	const int32                         CharacterLevel         = SourceAsc->GetCharacterLevel();
	const FGameplayTagContainer         CharacterTags          = SourceAsc->GetActiveGameplayTags();
	const EPF2CharacterAbilityScoreType AttackScoreType        = Weapon->GetAttackAbilityModifierType();
	const FGameplayTagContainer         ProficiencyTagPrefixes = Weapon->GetProficiencyTagPrefixes();
	float                               AttackAbilityModifier  = 0.0f,
	                                    MultipleAttackPenalty  = 0.0f,
	                                    RollCount              = 0.0f,
	                                    RollSize               = 0.0f;

	const FPF2AttackAttributeStatics&          AttackCaptures = FPF2AttackAttributeStatics::GetInstance();
	const FPF2SourceCharacterAttributeStatics& SourceCaptures = FPF2SourceCharacterAttributeStatics::GetInstance();

	const FGameplayEffectAttributeCaptureDefinition* AbilityScoreCapture =
		SourceCaptures.GetModifierCaptureByAbilityScoreType(AttackScoreType);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		*AbilityScoreCapture,
		EvaluationParameters,
		AttackAbilityModifier
	);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		SourceCaptures.EncMultipleAttackPenaltyDef,
		EvaluationParameters,
		MultipleAttackPenalty
	);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		AttackCaptures.TmpAttackRollCountDef,
		EvaluationParameters,
		RollCount
	);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		AttackCaptures.TmpAttackRollSizeDef,
		EvaluationParameters,
		RollSize
	);

	UE_LOG(
		LogPf2CoreStats,
		Verbose,
		TEXT("Performing attack roll (%dd%d) using '%s' (%f) as attack ability modifier type."),
		static_cast<int>(RollCount),
		static_cast<int>(RollSize),
		*PF2EnumUtilities::ToString(AttackScoreType),
		AttackAbilityModifier
	);

	Result = UPF2AttackStatLibrary::CalculateAttackRoll(
		CharacterLevel,
		CharacterTags,
		AttackAbilityModifier,
		MultipleAttackPenalty,
		ProficiencyTagPrefixes,
		TargetArmorClass,
		RollCount,
		RollSize
	);

	return Result;
}

float UPF2RollWeaponAttackExecution::PerformDamageRoll(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters&            EvaluationParameters,
	const IPF2WeaponInterface*                      Weapon)
{
	const FPF2AttackAttributeStatics&          AttackCaptures = FPF2AttackAttributeStatics::GetInstance();
	const FPF2SourceCharacterAttributeStatics& SourceCaptures = FPF2SourceCharacterAttributeStatics::GetInstance();

	const EPF2CharacterAbilityScoreType DamageScoreType       = Weapon->GetDamageAbilityModifierType();
	float                               DamageAbilityModifier = 0.0f,
	                                    RollCount             = 0.0f,
	                                    RollSize              = 0.0f;

	const FGameplayEffectAttributeCaptureDefinition* AbilityScoreCapture =
		SourceCaptures.GetModifierCaptureByAbilityScoreType(DamageScoreType);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		*AbilityScoreCapture,
		EvaluationParameters,
		DamageAbilityModifier
	);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		AttackCaptures.TmpDmgRollCountDef,
		EvaluationParameters,
		RollCount
	);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		AttackCaptures.TmpDmgRollSizeDef,
		EvaluationParameters,
		RollSize
	);

	UE_LOG(
		LogPf2CoreStats,
		Verbose,
		TEXT("Performing damage roll (%dd%d) using '%s' (%f) as damage ability modifier type."),
		static_cast<int>(RollCount),
		static_cast<int>(RollSize),
		*PF2EnumUtilities::ToString(DamageScoreType),
		DamageAbilityModifier
	);

	return UPF2AttackStatLibrary::CalculateDamageRoll(
		RollCount,
		RollSize,
		DamageAbilityModifier
	);
}

float UPF2RollWeaponAttackExecution::GetTargetArmorClass(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters&            EvaluationParameters)
{
	float                                      TargetArmorClass = 0.0f;
	const FPF2TargetCharacterAttributeStatics& TargetCaptures   = FPF2TargetCharacterAttributeStatics::GetInstance();

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		TargetCaptures.ArmorClassDef,
		EvaluationParameters,
		TargetArmorClass
	);

	return TargetArmorClass;
}

UPF2RollWeaponAttackExecution::UPF2RollWeaponAttackExecution()
{
	const FPF2AttackAttributeStatics&          AttackCaptures = FPF2AttackAttributeStatics::GetInstance();
	const FPF2SourceCharacterAttributeStatics& SourceCaptures = FPF2SourceCharacterAttributeStatics::GetInstance();
	const FPF2TargetCharacterAttributeStatics& TargetCaptures = FPF2TargetCharacterAttributeStatics::GetInstance();

	// Capture all ability score attributes, since attacks in OpenPF2 could theoretically be based on any ability score
	// stat even though the Core Rulebook only anticipates attacks that use Strength (for regular melee attacks) or
	// Dexterity (for ranged attacks and melee finesse attacks).
	for (const FGameplayEffectAttributeCaptureDefinition* Capture : SourceCaptures.GetAllAbilityScoreCaptures())
	{
		this->RelevantAttributesToCapture.Add(*Capture);
	}

	// Capture the multiple attack penalty, which grows with each additional attack taken during the same turn.
	this->RelevantAttributesToCapture.Add(SourceCaptures.EncMultipleAttackPenaltyDef);

	// Capture the target Armor Class (AC) for checks against attack rolls, to see if the target was hit at all.
	this->RelevantAttributesToCapture.Add(TargetCaptures.ArmorClassDef);

	// Capture the attack dice size and count.
	this->RelevantAttributesToCapture.Add(AttackCaptures.TmpAttackRollCountDef);
	this->RelevantAttributesToCapture.Add(AttackCaptures.TmpAttackRollSizeDef);

	// Capture the damage dice size and count.
	this->RelevantAttributesToCapture.Add(AttackCaptures.TmpDmgRollCountDef);
	this->RelevantAttributesToCapture.Add(AttackCaptures.TmpDmgRollSizeDef);
}

void UPF2RollWeaponAttackExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput&           OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec         = ExecutionParams.GetOwningSpec();
	AActor*                    EffectCauser = Spec.GetEffectContext().GetEffectCauser();

	if (const TScriptInterface<IPF2WeaponInterface> Weapon = IPF2WeaponInterface::EffectCauserToWeapon(EffectCauser);
		Weapon == nullptr)
	{
		// "An unarmed attack uses your body rather than a manufactured weapon. An unarmed attack isn’t a weapon, though
		// it’s categorized with weapons for weapon groups, and it might have weapon traits."
		//
		// Source: Pathfinder 2E Core Rulebook, Chapter 6, page 283, "Weapon Traits".
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Cannot calculate attack because no weapon was provided in the effect context. If this is an unarmed attack, the source of the attack must still implement IPF2WeaponInterface.")
		);
	}
	else
	{
		const UAbilitySystemComponent             *SourceAsc          = ExecutionParams.GetSourceAbilitySystemComponent(),
		                                          *TargetAsc          = ExecutionParams.GetTargetAbilitySystemComponent();
		const IPF2CharacterAbilitySystemInterface *SourceCharacterAsc = Cast<IPF2CharacterAbilitySystemInterface>(SourceAsc),
		                                          *TargetCharacterAsc = Cast<IPF2CharacterAbilitySystemInterface>(TargetAsc);

		if (SourceCharacterAsc == nullptr)
		{
			UE_LOG(
				LogPf2CoreAbilities,
				Error,
				TEXT("Cannot calculate attack because source ASC is not compatible with OpenPF2.")
			);
		}
		else if (TargetCharacterAsc == nullptr)
		{
			UE_LOG(
				LogPf2CoreAbilities,
				Error,
				TEXT("Cannot calculate attack because target ASC is not compatible with OpenPF2.")
			);
		}
		else
		{
			AttemptAttack(
				ExecutionParams,
				Weapon.GetInterface(),
				SourceCharacterAsc,
				TargetCharacterAsc,
				OutExecutionOutput
			);
		}
	}
}
