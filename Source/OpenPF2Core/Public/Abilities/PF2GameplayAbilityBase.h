// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Abilities/GameplayAbility.h>

#include "PF2InteractableAbilityInterface.h"

#include "PF2GameplayAbilityBase.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
struct FPF2GameplayEffectContainer;
struct FPF2GameplayEffectContainerSpec;

class IPF2CharacterInterface;
class IPF2WeaponInterface;
class UInputAction;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Abstract base class for OpenPF2-enabled gameplay abilities, including both passive and interactable abilities.
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class UPF2GameplayAbilityBase :
	public UGameplayAbility,
	public IPF2GameplayAbilityInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	virtual FString GetIdForLogs() const override;

	// =================================================================================================================
	// Public Methods - IPF2GameplayAbilityInterface Implementation
	// =================================================================================================================
	virtual UGameplayAbility* ToGameplayAbility() override;

	virtual FGameplayAbilitySpec ToGameplayAbilitySpecForCharacter(
		const TScriptInterface<IPF2CharacterInterface> Character,
		bool& bOutSpecFound
	) const override;

	virtual FGameplayAbilitySpecHandle ToGameplayAbilitySpecHandleForCharacter(
		const TScriptInterface<IPF2CharacterInterface> Character,
		bool& bOutSpecHandleFound
	) const override;

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the handle of the ability specification for the current activation.
	 *
	 * @return
	 *	The handle of the active ability.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	FGameplayAbilitySpecHandle GetSpecHandleOfCurrentActivation() const;

	/**
	 * Gets the OpenPF2-compatible character to which this ability has been granted.
	 *
	 * May be null if this ability has been instantiated but not yet been granted to a character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	TScriptInterface<IPF2CharacterInterface> GetOwningCharacterFromActorInfo() const;

	/**
	 * Processes a gameplay effect (GE) container into a gameplay effect container spec.
	 *
	 * @param Container
	 *	The GE container to process.
	 *
	 * @return
	 *	The GE container spec created from the GE container.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	FPF2GameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(
		const FPF2GameplayEffectContainer& Container) const;

	/**
	 * Processes source and target gameplay effect (GE) containers from a weapon into a gameplay effect container spec.
	 *
	 * This automatically invokes callbacks on the weapon to give it a chance to add any additional GEs that require
	 * values to be set on "Transient Aggregators" (a.k.a., "Temporary Variables" in the Editor UI).
	 *
	 * @param Weapon
	 *	The weapon for which gameplay effect containers will be processed into a ready-to-execute GE spec.
	 * @param Level
	 *	The character level to pass into each gameplay effect during processing.
	 * @param [out] SourceEffectsSpec
	 *	The GE container spec created from the source GE container of the given weapon. This spec is ready to apply to
	 *	the source ASC of the attack.
	 * @param [out] TargetEffectsSpec
	*	The GE container spec created from the target GE containers of the given weapon. This spec is ready to apply to
	 *	the target ASC of the attack.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities", BlueprintPure=false)
	void MakeEffectContainerSpecsFromWeapon(
		const TScriptInterface<IPF2WeaponInterface> Weapon,
		FPF2GameplayEffectContainerSpec&            SourceEffectsSpec,
		FPF2GameplayEffectContainerSpec&            TargetEffectsSpec,
		const float                                 Level = 1.0f) const;

	/**
	 * Processes a gameplay effect (GE) container into a gameplay effect container spec with the given effect causer.
	 *
	 * @param Container
	 *	The GE container to process.
	 * @param EffectCauser
	 *	The physical actor that is the source of all the effects (e.g., the source of damage), such as a sword or
	 *	projectile. If the effect/damage is being done by bare fists or physical contact rather than a weapon, this
	 *	could be the same actor as the instigator.
	 * @param Level
	 *	The level of the ability (for abilities that increase in damage/effect as they are upgraded, etc.)
	 *
	 * @return
	 *	The GE container spec created from the GE container.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	FPF2GameplayEffectContainerSpec MakeEffectContainerSpecFromContainerAndCauser(
		const FPF2GameplayEffectContainer& Container,
		AActor*                            EffectCauser,
		const float                        Level = 1.0f) const;

	/**
	 * Applies a gameplay effect (GE) container spec to the source/owning ASC of this ability.
	 *
	 * @param ContainerSpec
	 *	The GE container specification.
	 *
	 * @return
	 *	The list of handles for the GEs created and now active on the source.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="OpenPF2|Gameplay Abilities")
	TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpecToOwner(
		const FPF2GameplayEffectContainerSpec& ContainerSpec) const;

	/**
	 * Applies a gameplay effect (GE) container spec to all target ASCs of this ability.
	 *
	 * @param ContainerSpec
	 *	The GE container specification.
	 *
	 * @return
	 *	The list of handles for the GEs created and now active on all targets.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="OpenPF2|Gameplay Abilities")
	TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpecToTargets(
		const FPF2GameplayEffectContainerSpec& ContainerSpec) const;

	/**
	 * Creates a gameplay effect spec outgoing from the current ability for damage from a weapon.
	 *
	 * @param GameplayEffectClass
	 *	The type of gameplay effect for which a spec is desired.
	 * @param Weapon
	 *	The weapon that actually caused this effect (e.g., did damage), such as a sword or projectile.
	 * @param Level
	 *	The level of the attack (for weapons that increase in damage as they are upgraded, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	FGameplayEffectSpecHandle MakeOutgoingGameplayEffectSpecForWeapon(
		const TSubclassOf<UGameplayEffect>          GameplayEffectClass,
		const TScriptInterface<IPF2WeaponInterface> Weapon,
		const float                                 Level = 1.0f) const;

	/**
	 * Creates a gameplay effect spec outgoing from the current ability that has a custom effect causer.
	 *
	 * This is similar to MakeOutgoingGameplayEffectSpec() except that the effect causer can be set rather than it being
	 * set to the "avatar actor" which, in many games, is identical to the "owner actor" that is used as the instigator,
	 * which is the actor/character who owns the Ability System Component (ASC).
	 *
	 * @param GameplayEffectClass
	 *	The type of gameplay effect for which a spec is desired.
	 * @param EffectCauser
	 *	The physical actor that actually caused this effect (e.g., did damage), such as a weapon or projectile. If
	 *	the effect/damage is being done by bare fists or physical contact rather than a weapon, this could be the same
	 *	actor as the instigator.
	 * @param Level
	 *	The level of the ability (for abilities that increase in damage/effect as they are upgraded, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	FGameplayEffectSpecHandle MakeOutgoingGameplayEffectSpecForCauser(
		const TSubclassOf<UGameplayEffect> GameplayEffectClass,
		AActor*                            EffectCauser,
		const float                        Level = 1.0f) const;

	/**
	 * Creates a gameplay effect spec outgoing from the current ability that has a custom instigator and effect causer.
	 *
	 * This is similar to MakeOutgoingGameplayEffectSpec() except that the instigator and effect causer can both be set
	 * rather than them being based on the "avatar actor" which, in many games, is identical to the "owner actor", the
	 * actor/character who owns the Ability System Component (ASC).
	 *
	 * @param GameplayEffectClass
	 *	The type of gameplay effect for which a spec is desired.
	 * @param Instigator
	 *	The actor who originally initiated this effect (e.g., the actor who performed the attack).
	 * @param EffectCauser
	 *	The physical actor that actually caused this effect (e.g., did damage), such as a weapon or projectile. If
	 *	the effect/damage is being done by bare fists or physical contact rather than a weapon, this could be the same
	 *	actor as the instigator.
	 * @param Level
	 *	The level of the ability (for abilities that increase in damage/effect as they are upgraded, etc.)
	 *
	 * @return
	 *	The new gameplay effect specification.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	FGameplayEffectSpecHandle MakeOutgoingGameplayEffectSpecForInstigatorAndCauser(
		const TSubclassOf<UGameplayEffect> GameplayEffectClass,
		AActor*                            Instigator,
		AActor*                            EffectCauser,
		const float                        Level = 1.0f) const;

	/**
	 * Activates a Gameplay Effect (GE) on the owning ASC using source, instigator, causer, and tags from an event.
	 *
	 * This only works if all of the following are true:
	 * 1. The Gameplay Event was triggered by a Gameplay Effect (GE); and
	 * 2. The Gameplay Effect was triggered/applied by a GA (likely a *different* GA than this one); and
	 * 3. The triggering GA is instanced per execution; and
	 * 4. The Gameplay Effect context has been provided in the ContextHandle field of the payload.
	 *
	 * @param GameplayEffectClass
	 *	The type of gameplay effect to activate with the forwarded context.
	 * @param EventData
	 *	The payload provided along with the Gameplay Event.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="OpenPF2|Gameplay Abilities")
	FActiveGameplayEffectHandle ApplyGameplayEffectToSelfWithForwardedGameplayEventContext(
		const TSubclassOf<UGameplayEffect> GameplayEffectClass,
		const FGameplayEventData&          EventData) const;
};
