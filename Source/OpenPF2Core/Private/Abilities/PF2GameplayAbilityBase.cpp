// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2GameplayAbilityBase.h"

#include <AbilitySystemGlobals.h>

#include "PF2CharacterInterface.h"
#include "PF2GameplayEffectContainer.h"
#include "PF2GameplayEffectContainerSpec.h"

#include "Items/Weapons/PF2WeaponInterface.h"

#include "Libraries/PF2AbilitySystemLibrary.h"

#include "Utilities/PF2InterfaceUtilities.h"

FString UPF2GameplayAbilityBase::GetIdForLogs() const
{
	return this->GetName();
}

UGameplayAbility* UPF2GameplayAbilityBase::ToGameplayAbility()
{
	return this;
}

FGameplayAbilitySpec UPF2GameplayAbilityBase::ToGameplayAbilitySpecForCharacter(
	const TScriptInterface<IPF2CharacterInterface> Character,
	bool& bOutSpecFound
) const
{
	FGameplayAbilitySpec AbilitySpec;

	if (Character == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Given null for character when attempting to find ability spec for ability ('%s')."),
			*(this->GetIdForLogs())
		);
	}
	else
	{
		UAbilitySystemComponent* AbilitySystemComponent = Character->GetAbilitySystemComponent();
		bool                     bFoundAbilitySpec      = false;

		check(AbilitySystemComponent != nullptr);

		for (const FGameplayAbilitySpec& CurrentAbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (CurrentAbilitySpec.Ability.Get() == this)
			{
				AbilitySpec       = CurrentAbilitySpec;
				bFoundAbilitySpec = true;
				break;
			}
		}

		if (!bFoundAbilitySpec)
		{
			UE_LOG(
				LogPf2CoreAbilities,
				Error,
				TEXT("Failed to find ability spec in ASC for ability ('%s') granted to character ('%s')."),
				*(this->GetIdForLogs()),
				*(Character->GetIdForLogs())
			);
		}

		bOutSpecFound = bFoundAbilitySpec;
	}

	return AbilitySpec;
}

FGameplayAbilitySpecHandle UPF2GameplayAbilityBase::ToGameplayAbilitySpecHandleForCharacter(
	const TScriptInterface<IPF2CharacterInterface> Character,
	bool&                                          bOutSpecHandleFound
) const
{
	return this->ToGameplayAbilitySpecForCharacter(Character, bOutSpecHandleFound).Handle;
}

FGameplayAbilitySpecHandle UPF2GameplayAbilityBase::GetSpecHandleOfCurrentActivation() const
{
	return this->GetCurrentAbilitySpecHandle();
}

TScriptInterface<IPF2CharacterInterface> UPF2GameplayAbilityBase::GetOwningCharacterFromActorInfo() const
{
	TScriptInterface<IPF2CharacterInterface> Result;
	AActor*                                  OwningActor     = this->GetOwningActorFromActorInfo();
	IPF2CharacterInterface*                  OwningCharacter = Cast<IPF2CharacterInterface>(OwningActor);

	if (OwningActor == nullptr)
	{
		Result = TScriptInterface<IPF2CharacterInterface>(nullptr);
	}
	else
	{
		Result = PF2InterfaceUtilities::ToScriptInterface(OwningCharacter);
	}

	return Result;
}

FPF2GameplayEffectContainerSpec UPF2GameplayAbilityBase::MakeEffectContainerSpecFromContainer(
	const FPF2GameplayEffectContainer& Container) const
{
	FPF2GameplayEffectContainerSpec Result;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.GameplayEffectsToApply)
	{
		Result.AddGameplayEffectSpec(this->MakeOutgoingGameplayEffectSpec(EffectClass));
	}

	return Result;
}

void UPF2GameplayAbilityBase::MakeEffectContainerSpecsFromWeapon(
	const TScriptInterface<IPF2WeaponInterface> Weapon,
	FPF2GameplayEffectContainerSpec&            SourceEffectsSpec,
	FPF2GameplayEffectContainerSpec&            TargetEffectsSpec,
	const float                                 Level) const
{
	const TScriptInterface<IPF2CharacterInterface> Character = this->GetOwningCharacterFromActorInfo();

	if (Character == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("The owner of this gameplay ability ('%s') is not an OpenPF2-compatible character."),
			*(this->GetIdForLogs())
		);
	}
	else
	{
		const FPF2GameplayEffectContainer& SourceEffectsContainer = Weapon->GetSourceGameplayEffects();
		const FPF2GameplayEffectContainer& TargetEffectsContainer = Weapon->GetTargetGameplayEffects();

		check(this->CurrentSpecHandle.IsValid());
		check(this->CurrentActorInfo != nullptr);

		for (const TSubclassOf<UGameplayEffect>& EffectClass : SourceEffectsContainer.GameplayEffectsToApply)
		{
			SourceEffectsSpec.AddGameplayEffectSpec(
				this->MakeOutgoingGameplayEffectSpecForWeapon(EffectClass, Weapon, Level)
			);
		}

		Weapon->OnSourceGameplayEffectsContainerSpecGenerated(
			Character->GetCharacterAbilitySystemComponent(),
			this->CurrentSpecHandle,
			*(this->CurrentActorInfo),
			SourceEffectsSpec
		);

		for (const TSubclassOf<UGameplayEffect>& EffectClass : TargetEffectsContainer.GameplayEffectsToApply)
		{
			TargetEffectsSpec.AddGameplayEffectSpec(
				this->MakeOutgoingGameplayEffectSpecForWeapon(EffectClass, Weapon, Level)
			);
		}

		Weapon->OnTargetGameplayEffectsContainerSpecGenerated(
			Character->GetCharacterAbilitySystemComponent(),
			this->CurrentSpecHandle,
			*(this->CurrentActorInfo),
			TargetEffectsSpec
		);
	}
}

FPF2GameplayEffectContainerSpec UPF2GameplayAbilityBase::MakeEffectContainerSpecFromContainerAndCauser(
	const FPF2GameplayEffectContainer& Container,
	AActor*                            EffectCauser,
	const float                        Level) const
{
	FPF2GameplayEffectContainerSpec Result;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.GameplayEffectsToApply)
	{
		Result.AddGameplayEffectSpec(
			this->MakeOutgoingGameplayEffectSpecForCauser(EffectClass, EffectCauser, Level)
		);
	}

	return Result;
}

TArray<FActiveGameplayEffectHandle> UPF2GameplayAbilityBase::ApplyEffectContainerSpecToOwner(
	const FPF2GameplayEffectContainerSpec& ContainerSpec) const
{
	TArray<FActiveGameplayEffectHandle> AppliedEffects;
	TArray<FGameplayEffectSpecHandle>   SpecsToApply   = ContainerSpec.GameplayEffectSpecsToApply;

	check(this->CurrentSpecHandle.IsValid());
	check(this->CurrentActorInfo != nullptr);

	AppliedEffects.Reserve(SpecsToApply.Num());

	for (const FGameplayEffectSpecHandle& SpecHandle : SpecsToApply)
	{
		AppliedEffects.Add(
			ApplyGameplayEffectSpecToOwner(
				this->CurrentSpecHandle,
				this->CurrentActorInfo,
				this->CurrentActivationInfo,
				SpecHandle
			)
		);
	}

	return AppliedEffects;
}

TArray<FActiveGameplayEffectHandle> UPF2GameplayAbilityBase::ApplyEffectContainerSpecToTargets(
	const FPF2GameplayEffectContainerSpec& ContainerSpec) const
{
	TArray<FActiveGameplayEffectHandle> AppliedEffects;
	TArray<FGameplayEffectSpecHandle>   SpecsToApply   = ContainerSpec.GameplayEffectSpecsToApply;

	check(this->CurrentSpecHandle.IsValid());
	check(this->CurrentActorInfo != nullptr);

	AppliedEffects.Reserve(SpecsToApply.Num());

	for (const FGameplayEffectSpecHandle& SpecHandle : SpecsToApply)
	{
		AppliedEffects.Append(
			ApplyGameplayEffectSpecToTarget(
				this->CurrentSpecHandle,
				this->CurrentActorInfo,
				this->CurrentActivationInfo,
				SpecHandle,
				ContainerSpec.TargetData
			)
		);
	}

	return AppliedEffects;
}

FGameplayEffectSpecHandle UPF2GameplayAbilityBase::MakeOutgoingGameplayEffectSpecForWeapon(
	const TSubclassOf<UGameplayEffect>          GameplayEffectClass,
	const TScriptInterface<IPF2WeaponInterface> Weapon,
	const float                                 Level) const
{
	check(this->CurrentActorInfo != nullptr);
	check(this->CurrentActorInfo->AbilitySystemComponent.IsValid());

	return UPF2AbilitySystemLibrary::MakeGameplayEffectSpecForWeapon(
		this->CurrentSpecHandle,
		*(this->CurrentActorInfo),
		GameplayEffectClass,
		Weapon,
		Level
	);
}

FGameplayEffectSpecHandle UPF2GameplayAbilityBase::MakeOutgoingGameplayEffectSpecForCauser(
	const TSubclassOf<UGameplayEffect> GameplayEffectClass,
	AActor*                            EffectCauser,
	const float                        Level) const
{
	check(this->CurrentActorInfo != nullptr);
	check(this->CurrentActorInfo->AbilitySystemComponent.IsValid());

	return UPF2AbilitySystemLibrary::MakeGameplayEffectSpecForCauser(
		this->CurrentSpecHandle,
		*(this->CurrentActorInfo),
		GameplayEffectClass,
		EffectCauser,
		Level
	);
}

FGameplayEffectSpecHandle UPF2GameplayAbilityBase::MakeOutgoingGameplayEffectSpecForInstigatorAndCauser(
	const TSubclassOf<UGameplayEffect> GameplayEffectClass,
	AActor*                            Instigator,
	AActor*                            EffectCauser,
	const float                        Level) const
{
	check(this->CurrentActorInfo != nullptr);
	check(this->CurrentActorInfo->AbilitySystemComponent.IsValid());

	return UPF2AbilitySystemLibrary::MakeGameplayEffectSpecForInstigatorAndCauser(
		this->CurrentSpecHandle,
		*(this->CurrentActorInfo),
		GameplayEffectClass,
		Instigator,
		EffectCauser,
		Level
	);
}

FActiveGameplayEffectHandle UPF2GameplayAbilityBase::ApplyGameplayEffectToSelfWithForwardedGameplayEventContext(
	const TSubclassOf<UGameplayEffect> GameplayEffectClass,
	const FGameplayEventData&          EventData) const
{
	const FGameplayEffectSpecHandle EffectSpecHandle =
		UPF2AbilitySystemLibrary::MakeGameplayEffectSpecFromGameplayEventContext(GameplayEffectClass, EventData);

	if (EffectSpecHandle.IsValid())
	{
		return this->ApplyGameplayEffectSpecToOwner(
			this->CurrentSpecHandle,
			this->CurrentActorInfo,
			this->CurrentActivationInfo,
			EffectSpecHandle
		);
	}
	else
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Failed to make GE spec from Gameplay Event context received by ability ('%s'); there was insufficient context available in the event payload."),
			*(this->GetIdForLogs())
		);

		return FActiveGameplayEffectHandle();
	}
}
