// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Items/Weapons/PF2Weapon.h"

#include <GameplayEffectExecutionCalculation.h>

#include "Abilities/PF2EffectCauseWrapper.h"

#include "CharacterStats/PF2CharacterAbilitySystemInterface.h"

#include "GameplayEffects/PF2GameplayEffectContainerSpec.h"

APF2EffectCauseWrapper* UPF2Weapon::ToEffectCauser(AActor* OwningActor)
{
	return APF2EffectCauseWrapper::Create(OwningActor, this);
}

void UPF2Weapon::OnSourceGameplayEffectsContainerSpecGenerated(
	const TScriptInterface<IPF2CharacterAbilitySystemInterface>& SourceAbilitySystemComponent,
	const UGameplayAbility*                                      ActivatedAbility,
	FPF2GameplayEffectContainerSpec&                             ContainerSpec)
{
	this->BP_OnSourceGameplayEffectsContainerSpecGenerated(
		SourceAbilitySystemComponent,
		ActivatedAbility,
		ContainerSpec,
		ContainerSpec
	);
}

void UPF2Weapon::OnTargetGameplayEffectsContainerSpecGenerated(
	const TScriptInterface<IPF2CharacterAbilitySystemInterface>& SourceAbilitySystemComponent,
	const UGameplayAbility*                                      ActivatedAbility,
	FPF2GameplayEffectContainerSpec&                             ContainerSpec)
{
	this->BP_OnTargetGameplayEffectsContainerSpecGenerated(
		SourceAbilitySystemComponent,
		ActivatedAbility,
		ContainerSpec,
		ContainerSpec
	);
}

FGameplayTagContainer UPF2Weapon::GetProficiencyTagPrefixes() const
{
	return this->ProficiencyTagPrefixes;
}

EPF2CharacterAbilityScoreType UPF2Weapon::GetAttackAbilityModifierType() const
{
	return this->AttackAbilityModifierType;
}

EPF2CharacterAbilityScoreType UPF2Weapon::GetDamageAbilityModifierType() const
{
	return this->DamageAbilityModifierType;
}

FName UPF2Weapon::GetDamageDie() const
{
	return this->DamageDie;
}

FGameplayTag UPF2Weapon::GetDamageType() const
{
	return this->DamageType;
}

FPF2GameplayEffectContainer UPF2Weapon::GetSourceGameplayEffects() const
{
	return this->SourceGameplayEffects;
}

FPF2GameplayEffectContainer UPF2Weapon::GetTargetGameplayEffects() const
{
	return this->TargetGameplayEffects;
}

FPrimaryAssetId UPF2Weapon::GetPrimaryAssetId()
{
	return Super::GetPrimaryAssetId();
}

UDataAsset* UPF2Weapon::ToDataAsset()
{
	return Super::ToDataAsset();
}

bool UPF2Weapon::ShouldBeEquippedInAllLinkedSlots()
{
	return Super::ShouldBeEquippedInAllLinkedSlots();
}

FString UPF2Weapon::GetIdForLogs() const
{
	return Super::GetIdForLogs();
}

void UPF2Weapon::BP_OnSourceGameplayEffectsContainerSpecGenerated_Implementation(
	const TScriptInterface<IPF2CharacterAbilitySystemInterface>& SourceAbilitySystemComponent,
	const UGameplayAbility*                                      ActivatedAbility,
	const FPF2GameplayEffectContainerSpec&                       ContainerSpec,
	FPF2GameplayEffectContainerSpec&                             ModifiedContainerSpec)
{
	// Default implementation: just return what was passed in, as is.
	ModifiedContainerSpec = ContainerSpec;
}

void UPF2Weapon::BP_OnTargetGameplayEffectsContainerSpecGenerated_Implementation(
	const TScriptInterface<IPF2CharacterAbilitySystemInterface>& SourceAbilitySystemComponent,
	const UGameplayAbility*                                      ActivatedAbility,
	const FPF2GameplayEffectContainerSpec&                       ContainerSpec,
	FPF2GameplayEffectContainerSpec&                             ModifiedContainerSpec)
{
	// Default implementation: just return what was passed in, as is.
	ModifiedContainerSpec = ContainerSpec;
}
