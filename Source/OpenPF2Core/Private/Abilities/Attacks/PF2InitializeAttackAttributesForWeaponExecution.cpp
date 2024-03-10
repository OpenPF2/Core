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

#include "Abilities/Attacks/PF2InitializeAttackAttributesForWeaponExecution.h"

#include <AbilitySystemComponent.h>

#include "Abilities/Attacks/PF2AttackAttributeStatics.h"

#include "Items/Weapons/PF2WeaponInterface.h"

#include "Libraries/PF2AttackStatLibrary.h"
#include "Libraries/PF2DiceLibrary.h"

void UPF2InitializeAttackAttributesForWeaponExecution::InitializeAttackStats(
	const int32                           WeaponRollCount,
	const int32                           WeaponDieSize,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput)
{
	const FPF2AttackAttributeStatics AttackCaptures = FPF2AttackAttributeStatics::GetInstance();

	// Start out with a 1d20 for attack rolls (TmpAttackRollCount = 1, TmpAttackRollSize = 20).
	//
	// From the Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Attack Rolls":
	// "When making an attack roll, determine the result by rolling 1d20 [...]"
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			AttackCaptures.TmpAttackRollCountProperty,
			EGameplayModOp::Override,
			1.0f
		)
	);

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			AttackCaptures.TmpAttackRollSizeProperty,
			EGameplayModOp::Override,
			20.0f
		)
	);

	// Start with no degree of success value.
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			AttackCaptures.TmpAttackDegreeOfSuccessProperty,
			EGameplayModOp::Override,
			UPF2AttackStatLibrary::DegreeOfSuccessStatFromEnum(EPF2DegreeOfSuccess::None)
		)
	);

	// Initialize damage rolls from the weapon statistics.
	//
	// From the Pathfinder 2E Core Rulebook, Chapter 6, page 450, "Attack Rolls":
	// "Damage is sometimes given as a fixed amount, but more often than not you’ll make a damage roll to
	// determine how much damage you deal. A damage roll typically uses a number and type of dice determined by
	// the weapon or unarmed attack used or the spell cast, and it is often enhanced by various modifiers,
	// bonuses, and penalties."
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			AttackCaptures.TmpDmgRollCountProperty,
			EGameplayModOp::Override,
			WeaponRollCount
		)
	);

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			AttackCaptures.TmpDmgRollSizeProperty,
			EGameplayModOp::Override,
			WeaponDieSize
		)
	);

	// Zero out all other damage attributes.
	for (const FGameplayEffectAttributeCaptureDefinition* Capture : AttackCaptures.GetAllDamageCaptures())
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				Capture->AttributeToCapture.GetUProperty(),
				EGameplayModOp::Override,
				0.0f
			)
		);
	}
}

void UPF2InitializeAttackAttributesForWeaponExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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
			TEXT("Cannot load weapon statistics because no weapon was provided in the effect context. If this is an unarmed attack, the source of the attack must still implement IPF2WeaponInterface.")
		);
	}
	else
	{
		const FName DamageDie       = Weapon->GetDamageDie();
		int32       WeaponRollCount,
		            WeaponDieSize;

		if (UPF2DiceLibrary::ParseRollExpression(DamageDie, WeaponRollCount, WeaponDieSize))
		{
			UE_LOG(
				LogPf2CoreAbilities,
				Verbose,
				TEXT("Initializing attack with weapon ('%s')."),
				*(Weapon->GetIdForLogs())
			);

			InitializeAttackStats(WeaponRollCount, WeaponDieSize, OutExecutionOutput);
		}
		else
		{
			UE_LOG(
				LogPf2CoreAbilities,
				Error,
				TEXT("Cannot load weapon statistics because damage die expression ('%s') cannot be parsed."),
				*(DamageDie.ToString())
			);
		}
	}
}
