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

#include "CharacterStats/PF2ApplyDamageFromSourceExecution.h"

#include "Abilities/Attacks/PF2AttackAttributeStatics.h"

#include "CharacterStats/PF2TargetCharacterAttributeStatics.h"

#include "Libraries/PF2AbilitySystemLibrary.h"

#include "Utilities/PF2GameplayAbilityUtilities.h"

UPF2ApplyDamageFromSourceExecution::UPF2ApplyDamageFromSourceExecution()
{
	const FPF2AttackAttributeStatics          AttackCaptures = FPF2AttackAttributeStatics::GetInstance();
	const FPF2TargetCharacterAttributeStatics TargetCaptures = FPF2TargetCharacterAttributeStatics::GetInstance();

	this->RelevantAttributesToCapture.Add(AttackCaptures.TmpAttackDegreeOfSuccessDef);

	for (const FGameplayEffectAttributeCaptureDefinition* Capture : AttackCaptures.GetAllDamageCaptures())
	{
		this->RelevantAttributesToCapture.Add(*Capture);
	}

	// Cache the tag to avoid lookup overhead.
	this->InflictDamageCueTag = PF2GameplayAbilityUtilities::GetTag(
		FName("GameplayCue.Character.InflictDamage")
	);
}

void UPF2ApplyDamageFromSourceExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput&           OutExecutionOutput) const
{
	UAbilitySystemComponent*                  TargetAsc             = ExecutionParams.GetTargetAbilitySystemComponent();
	const FPF2AttackAttributeStatics          AttackCaptures        = FPF2AttackAttributeStatics::GetInstance();
	const FPF2TargetCharacterAttributeStatics TargetCaptures        = FPF2TargetCharacterAttributeStatics::GetInstance();
	float                                     AttackDegreeOfSuccess = 0.0f;
	bool                                      bHaveAnyDamage        = false;

	const FAggregatorEvaluateParameters EvaluationParameters =
		UPF2AbilitySystemLibrary::BuildEvaluationParameters(ExecutionParams);

	// Capture: Degree of success from attack roll.
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		AttackCaptures.TmpAttackDegreeOfSuccessDef,
		EvaluationParameters,
		AttackDegreeOfSuccess
	);

	// Apply: Degree of success from attack roll.
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			TargetCaptures.TmpLastIncomingAttackDegreeOfSuccessProperty,
			EGameplayModOp::Override,
			AttackDegreeOfSuccess
		)
	);

	for (const FGameplayEffectAttributeCaptureDefinition* Capture : AttackCaptures.GetAllDamageCaptures())
	{
		FString DamageAttributeName            = Capture->AttributeToCapture.AttributeName;
		float   AmountFromDamageType           = 0.0f,
		        AmountOfResistanceToDamageType = 0.0f,
		        EffectiveDamage;

		const FGameplayEffectAttributeCaptureDefinition* ResistanceCapture =
			TargetCaptures.GetResistanceCaptureForDamageAttribute(DamageAttributeName);

		// Capture: Amount of this type of damage source has attempted against the target.
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			*Capture,
			EvaluationParameters,
			AmountFromDamageType
		);

		// Capture: Amount of resistance that the target has for this type of damage.
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			*ResistanceCapture,
			EvaluationParameters,
			AmountOfResistanceToDamageType
		);

		// Apply resistance to reduce damage, but don't allow resistance to make damage negative (i.e., damage can never
		// heal, but it can become ineffectual).
		//
		// From the Pathfinder 2E Core Rulebook, page 453, "Resistance":
		// "If you have resistance to a type of damage, each time you take that type of damage, you reduce the amount of
		// damage you take by the listed amount (to a minimum of 0 damage)."
		EffectiveDamage = FMath::Max(0.0f, AmountFromDamageType - AmountOfResistanceToDamageType);

		UE_LOG(
			LogPf2CoreStats,
			VeryVerbose,
			TEXT("Damage (%s: %f) - Resistance (%s: %f) = %f (CLAMPED >= 0)."),
			*DamageAttributeName,
			AmountFromDamageType,
			*ResistanceCapture->AttributeToCapture.GetName(),
			AmountOfResistanceToDamageType,
			EffectiveDamage
		);

		if (EffectiveDamage > 0)
		{
			FGameplayCueParameters CueParams = PopulateGameplayCueParameters(ExecutionParams);

			// Apply: Damage, less resistance.
			OutExecutionOutput.AddOutputModifier(
				FGameplayModifierEvaluatedData(
					TargetCaptures.TmpDamageIncomingProperty,
					EGameplayModOp::Additive,
					EffectiveDamage
				)
			);

			bHaveAnyDamage = true;

			// For now, pass the damage type along as a source tag. These feels like a hack, but saves us from having to
			// define a custom parameter object and/or context object to pass along inside the parameter object.
			//
			// An alternative would be to pass the damage type along in the OriginalTag field, but the intent of that
			// field appears to be to capture what gameplay tag was emitted by a GE to locate the cue. The
			// MatchedTagName field, meanwhile, appears to be for holding the name of the tag that the selected cue has.
			CueParams.AggregatedSourceTags.AddTag(
				AttackCaptures.GetDamageTypeForDamageAttribute(Capture->AttributeToCapture)
			);

			CueParams.RawMagnitude = EffectiveDamage;

			TargetAsc->ExecuteGameplayCue(
				this->InflictDamageCueTag,
				CueParams
			);
		}
	}

	if (!bHaveAnyDamage)
	{
		// Fire off a cue for a miss (no damage), so that the player can see a zero.
		FGameplayCueParameters CueParams = PopulateGameplayCueParameters(ExecutionParams);

		CueParams.RawMagnitude = 0;

		TargetAsc->ExecuteGameplayCue(
			this->InflictDamageCueTag,
			CueParams
		);
	}
}
