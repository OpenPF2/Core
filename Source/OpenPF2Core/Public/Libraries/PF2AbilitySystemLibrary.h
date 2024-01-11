// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

// ReSharper disable CppUEBlueprintCallableFunctionUnused
#pragma once

#include <GameplayEffectExecutionCalculation.h>

#include <Kismet/BlueprintFunctionLibrary.h>

#include "PF2GameplayEffectContainerSpec.h"
#include "PF2PlayerControllerInterface.h"
#include "PF2TargetSelectionType.h"

#include "Utilities/PF2InterfaceUtilities.h"

#include "PF2AbilitySystemLibrary.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
struct FGameplayEffectCustomExecutionParameters;
struct FPF2GameplayEffectContainerSpec;

class IPF2WeaponInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Function library for working with gameplay abilities in OpenPF2.
 */
UCLASS()
class OPENPF2CORE_API UPF2AbilitySystemLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Obtains the gameplay tags from the source that were captured when a Gameplay Effect specification was created.
	 *
	 * @param GameplayEffectSpec
	 *	The GE spec from which the tags will be obtained.
	 * @param [out] SourceTags
	 *	A reference to the container to receive the captured source tags.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	static void GetCapturedSourceTags(const FGameplayEffectSpec& GameplayEffectSpec,
	                                  FGameplayTagContainer&     SourceTags)
	{
		SourceTags.AppendTags(*GameplayEffectSpec.CapturedSourceTags.GetAggregatedTags());
	}

	/**
	 * Extracts the effect causer that was supplied as the first optional object in a Gameplay Event payload.
	 *
	 * @param EventData
	 *	The payload provided along with the Gameplay Event.
	 *
	 * @return
	 *	Either the character as a Blueprint-friendly script interface, or a script interface that wraps nullptr if the
	 *	the first optional object is a nullptr or not a OpenPF2-compatible character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	static TScriptInterface<IPF2CharacterInterface> GetCausingCharacterFromGameplayEventPayload(
		const FGameplayEventData& EventData);

	/**
	 * Locates the instance of the Gameplay Ability (GA) that activated the effect that emitted an event.
	 *
	 * This only works if all of the following are true:
	 * 1. The Gameplay Event was triggered by a Gameplay Effect (GE); and
	 * 2. The Gameplay Effect was triggered/applied by a GA; and
	 * 3. The GA is instanced per execution; and
	 * 4. The Gameplay Effect context has been provided in the ContextHandle field of the payload.
	 *
	 * @param EventData
	 *	The payload provided along with the Gameplay Event.
	 *
	 * @return
	 *	Either a pointer to the instigating GA, or a nullptr if the preconditions indicated above did not hold.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	static const UGameplayAbility* GetAbilityInstanceFromGameplayEventPayload(
		const FGameplayEventData& EventData);

	/**
	 * Determines the instigator and damage source from the given Gameplay Effect (GE) activation specification.
	 *
	 * @param [in] EffectSpec
	 *	A specification that provides context about the GE execution.
	 * @param [out] Instigator
	 *	The character who is ultimately responsible for the damage. This could be null if the damage is caused by the
	 *	world.
	 * @param [out] DamageSource
	 *	The actor that directly inflicted the damage, such as a weapon or projectile.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	static void DetermineDamageInstigatorAndSource(const FGameplayEffectSpec&                EffectSpec,
	                                               TScriptInterface<IPF2CharacterInterface>& Instigator,
	                                               AActor*&                                  DamageSource);

	/**
	 * Creates a Gameplay Effect (GE) specification from context in a Gameplay Event.
	 *
	 * This enables a new GE to be applied to the target as if it was initiated by a prior ability action on the source.
	 * We are forwarding details about the original ability activation into a new GE activation on the target, as if it
	 * originated from the source.
	 *
	 * This only works if all of the following are true:
	 * 1. The Gameplay Event was triggered by a Gameplay Effect (GE); and
	 * 2. The Gameplay Effect was triggered/applied by a GA; and
	 * 3. The GA is instanced per execution; and
	 * 4. The Gameplay Effect context has been provided in the ContextHandle field of the payload.
	 *
	 * @param GameplayEffectClass
	 *	The type of gameplay effect for which a spec is desired.
	 * @param EventData
	 *	The payload provided along with the Gameplay Event.
	 *
	 * @return
	 *	Either a valid handle for the specified Gameplay Effect, or an invalid handle if the preconditions indicated
	 *	above did not hold.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	static FGameplayEffectSpecHandle MakeGameplayEffectSpecFromGameplayEventContext(
		const TSubclassOf<UGameplayEffect> GameplayEffectClass,
		const FGameplayEventData&          EventData);

	/**
	 * Creates a gameplay effect (GE) spec for damage from a weapon.
	 *
	 * This method differs from methods like MakeEffectContextForCauser() and MakeEffectContextForInstigatorAndCauser()
	 * in that it returns the server handle for a GE *specification* rather than for a GE *context*. The former is a
	 * higher-level struct that *includes* the GE context along with other information about which effect is about to be
	 * applied and its state; meanwhile, the latter only tracks the context of the effect activation (including the
	 * instigator, effect causer, ability, and level). In short, this method is intended to assemble all the required
	 * information to apply a GE to a target.
	 *
	 * @param AbilityHandle
	 *	The handle for the gameplay ability activation that is generating the gameplay effect spec.
	 * @param AbilityOwnerInfo
	 *	Information about the actor who activated this gameplay ability.
	 * @param GameplayEffectClass
	 *	The type of gameplay effect for which a spec is desired.
	 * @param Weapon
	 *	The weapon that actually caused this effect (e.g., did damage), such as a sword or projectile.
	 * @param Level
	 *	The level of the attack (for weapons that increase in damage as they are upgraded, etc.)
	 *
	 * @return
	 *	The new gameplay effect specification handle.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	static FGameplayEffectSpecHandle MakeGameplayEffectSpecForWeapon(
		const FGameplayAbilitySpecHandle&            AbilityHandle,
		const FGameplayAbilityActorInfo&             AbilityOwnerInfo,
		const TSubclassOf<UGameplayEffect>           GameplayEffectClass,
		const TScriptInterface<IPF2WeaponInterface>& Weapon,
		const float                                  Level = 1.0f);

	/**
	 * Creates a gameplay effect (GE) spec that has a custom effect causer.
	 *
	 * This is similar to MakeOutgoingGameplayEffectSpec() in GAS except that the effect causer can be set rather than
	 * it being set equal to the "avatar actor" which, in many games, is identical to the "owner actor" that is used as
	 * the instigator, the actor/character who owns the Ability System Component (ASC).
	 *
	 * This method differs from methods like MakeEffectContextForCauser() and MakeEffectContextForInstigatorAndCauser()
	 * in that it returns the server handle for a GE *specification* rather than for a GE *context*. The former is a
	 * higher-level struct that *includes* the GE context along with other information about which effect is about to be
	 * applied and its state; meanwhile, the latter only tracks the context of the effect activation (including the
	 * instigator, effect causer, ability, and level). In short, this method is intended to assemble all the required
	 * information to apply a GE to a target.
	 *
	 * @param AbilityHandle
	 *	The handle for the gameplay ability activation that is generating the gameplay effect spec.
	 * @param AbilityOwnerInfo
	 *	Information about the actor who activated this gameplay ability.
	 * @param GameplayEffectClass
	 *	The type of gameplay effect for which a spec is desired.
	 * @param EffectCauser
	 *	The physical actor that actually caused this effect (e.g., did damage), such as a weapon or projectile. If
	 *	the effect/damage is being done by bare fists or physical contact rather than a weapon, this could be the same
	 *	actor as the instigator.
	 * @param Level
	 *	The level of the ability (for abilities that increase in damage/effect as they are upgraded, etc.)
	 *
	 * @return
	 *	The new gameplay effect specification handle.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	static FORCEINLINE FGameplayEffectSpecHandle MakeGameplayEffectSpecForCauser(
		const FGameplayAbilitySpecHandle&  AbilityHandle,
		const FGameplayAbilityActorInfo&   AbilityOwnerInfo,
		const TSubclassOf<UGameplayEffect> GameplayEffectClass,
		AActor*                            EffectCauser,
		const float                        Level = 1.0f)
	{
		return MakeGameplayEffectSpecForInstigatorAndCauser(
			AbilityHandle,
			AbilityOwnerInfo,
			GameplayEffectClass,
			AbilityOwnerInfo.OwnerActor.Get(),
			EffectCauser,
			Level
		);
	}

	/**
	 * Creates a gameplay effect (GE) spec that has a custom instigator and effect causer.
	 *
	 * This is similar to MakeOutgoingGameplayEffectSpec() in GAS except that the instigator and effect causer can both
	 * be set rather than them being based on the "avatar actor" which, in many games, is identical to the
	 * "owner actor", the actor/character who owns the Ability System Component (ASC).
	 *
	 * This method differs from methods like MakeEffectContextForCauser() and MakeEffectContextForInstigatorAndCauser()
	 * in that it returns the server handle for a GE *specification* rather than for a GE *context*. The former is a
	 * higher-level struct that *includes* the GE context along with other information about which effect is about to be
	 * applied and its state; meanwhile, the latter only tracks the context of the effect activation (including the
	 * instigator, effect causer, ability, and level). In short, this method is intended to assemble all the required
	 * information to apply a GE to a target.
	 *
	 * @param AbilityHandle
	 *	The handle for the gameplay ability activation that is generating the gameplay effect spec.
	 * @param AbilityOwnerInfo
	 *	Information about the actor who activated this gameplay ability.
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
	 *	The new gameplay effect specification handle.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	static FGameplayEffectSpecHandle MakeGameplayEffectSpecForInstigatorAndCauser(
		const FGameplayAbilitySpecHandle&  AbilityHandle,
		const FGameplayAbilityActorInfo&   AbilityOwnerInfo,
		const TSubclassOf<UGameplayEffect> GameplayEffectClass,
		AActor*                            Instigator,
		AActor*                            EffectCauser,
		const float                        Level = 1.0f);

	/**
	 * Builds context for a gameplay effect activation triggered by the specified ability, owner, and effect causer.
	 *
	 * This method differs from methods like MakeGameplayEffectSpecForWeapon(), MakeGameplayEffectSpecForCauser(), and
	 * MakeGameplayEffectSpecForInstigatorAndCauser() in that it returns the server handle for a gameplay effect (GE)
	 * *context* rather than for a GE *specification*. The former is a lower-level struct that tracks the context of the
	 * effect activation (including the instigator, effect causer, ability, and level); meanwhile, the latter is a
	 * higher-level struct that *includes* the GE context along with other information about which effect is about to be
	 * applied and its state. In short, this method only assembles the information about the ability and actor that is
	 * invoking a GE, without enough information to know which GE to invoke or how to invoke it.
	 *
	 * @param AbilityHandle
	 *	The handle for the gameplay ability activation that is generating the effect context.
	 * @param AbilityOwnerInfo
	 *	Information about the actor who activated this gameplay ability.
	 * @param EffectCauser
	 *	The physical actor that actually caused this effect (e.g., did damage), such as a weapon or projectile. If
	 *	the effect/damage is being done by bare fists or physical contact rather than a weapon, this could be the same
	 *	actor as the instigator.
	 *
	 * @return
	 *	The new gameplay effect context handle.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	static FORCEINLINE FGameplayEffectContextHandle MakeEffectContextForCauser(
		const FGameplayAbilitySpecHandle& AbilityHandle,
		const FGameplayAbilityActorInfo&  AbilityOwnerInfo,
		AActor*                           EffectCauser)
	{
		// Use the character who activated the ability as the instigator, while using a caller-supplied effect causer.
		return MakeEffectContextForInstigatorAndCauser(
			AbilityHandle,
			AbilityOwnerInfo,
			AbilityOwnerInfo.OwnerActor.Get(),
			EffectCauser
		);
	}

	/**
	 * Builds context for a gameplay effect activation triggered by the specified ability, instigator, and causer.
	 *
	 * This method differs from methods like MakeGameplayEffectSpecForWeapon(), MakeGameplayEffectSpecForCauser(), and
	 * MakeGameplayEffectSpecForInstigatorAndCauser() in that it returns the server handle for a gameplay effect (GE)
	 * *context* rather than for a GE *specification*. The former is a lower-level struct that tracks the context of the
	 * effect activation (including the instigator, effect causer, ability, and level); meanwhile, the latter is a
	 * higher-level struct that *includes* the GE context along with other information about which effect is about to be
	 * applied and its state. In short, this method only assembles the information about the ability and actor that is
	 * invoking a GE, without enough information to know which GE to invoke or how to invoke it.
	 *
	 * @param AbilityHandle
	 *	The handle for the gameplay ability activation that is generating the effect context.
	 * @param AbilityOwnerInfo
	 *	Information about the actor who activated this gameplay ability.
	 * @param Instigator
	 *	The actor who originally initiated this effect (e.g., the actor who performed the attack).
	 * @param EffectCauser
	 *	The physical actor that actually inflicted this effect (e.g., did damage), such as a weapon or projectile. If
	 *	the effect/damage is being done by bare fists or physical contact rather than a weapon, this could be the same
	 *	actor as the instigator.
	 *
	 * @return
	 *	The new gameplay effect context handle.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	static FGameplayEffectContextHandle MakeEffectContextForInstigatorAndCauser(
		const FGameplayAbilitySpecHandle& AbilityHandle,
		const FGameplayAbilityActorInfo&  AbilityOwnerInfo,
		AActor*                           Instigator,
		AActor*                           EffectCauser);

	/**
	 * Creates target data from whichever location or character the player has selected through the UI.
	 *
	 * This is usually selected via a movement grid.
	 *
	 * @return
	 *	The handle for the new gameplay ability target data.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Gameplay Abilities|Target Data")
	static FGameplayAbilityTargetDataHandle CreateAbilityTargetDataFromPlayerControllerTargetSelection(
		const TScriptInterface<IPF2PlayerControllerInterface> PlayerController);

	/**
	 * Gets the type of selection a player has made for a target selection.
	 *
	 * If a target does not exist for the specified handle under the given index, or the target did not originate from
	 * a target selection through an OpenPF2-compatible method, the result is EPF2TargetSelectionType::None.
	 *
	 * @param TargetDataHandle
	 *	The handle to the target data for which a target type is desired.
	 * @param Index
	 *	The index of the data for which a target type is desired.
	 *
	 * @return
	 *	The type of the target selection.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Gameplay Abilities|Target Data")
	static EPF2TargetSelectionType GetTargetSelectionType(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                                      const int32                             Index);

	/**
	 * Gets whether the specified Gameplay Effect (GE) container currently has any gameplay effects to apply.
	 *
	 * @param ContainerSpec
	 *	The gameplay effect container specification to check.
	 *
	 * @return
	 *	- true if the container has at least one GE spec.
	 *	- false if the container has no GE specs.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Gameplay Effects")
	static FORCEINLINE bool DoesEffectContainerSpecHaveEffects(const FPF2GameplayEffectContainerSpec& ContainerSpec)
	{
		return ContainerSpec.HasEffects();
	}

	/**
	 * Gets whether the specified Gameplay Effect (GE) container currently has any targets (either hits or actors).
	 *
	 * @param ContainerSpec
	 *	The gameplay effect container specification to check.
	 *
	 * @return
	 *	- true if the container has at least one target.
	 *	- false if the container has no targets.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Gameplay Effects")
	static FORCEINLINE bool DoesEffectContainerSpecHaveTargets(const FPF2GameplayEffectContainerSpec& ContainerSpec)
	{
		return ContainerSpec.HasTargets();
	}

	/**
	 * Returns a copy of the specified Gameplay Effect (GE) container spec with the given gameplay effect added.
	 *
	 * @param ContainerSpec
	 *	The original gameplay effect container specification from which to start from.
	 * @param GameplayEffectSpec
	 *	The specification handle to add.
	 *
	 * @return
	 *	The modified GE container spec.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Effects")
	static FPF2GameplayEffectContainerSpec AddGameplayEffectSpecToEffectContainerSpec(
		const FPF2GameplayEffectContainerSpec& ContainerSpec,
		const FGameplayEffectSpecHandle&       GameplayEffectSpec);

	/**
	 * Returns a copy of the specified Gameplay Effect (GE) container spec with the given hits added as targets.
	 *
	 * @param ContainerSpec
	 *	The original gameplay effect container specification from which to start from.
	 * @param HitResults
	 *	The list of hit results to add as targets of this container.
	 *
	 * @return
	 *	The modified GE container spec.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Effects")
	static FPF2GameplayEffectContainerSpec AddHitTargetsToEffectContainerSpec(
		const FPF2GameplayEffectContainerSpec& ContainerSpec,
		const TArray<FHitResult>&              HitResults);

	/**
	 * Returns a copy of the specified Gameplay Effect (GE) container spec with the given actors added as targets.
	 *
	 * @param ContainerSpec
	 *	The original gameplay effect container specification from which to start from.
	 * @param TargetActors
	 *	The list of actors to add as targets of the container.
	 *
	 * @return
	 *	The modified GE container spec.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Effects")
	static FPF2GameplayEffectContainerSpec AddActorTargetsToEffectContainerSpec(
		const FPF2GameplayEffectContainerSpec& ContainerSpec,
		const TArray<AActor*>&                 TargetActors);

	/**
	 * Returns a copy of the specified Gameplay Effect (GE) container spec with the given generic targets added.
	 *
	 * This is useful for adding existing generic targets to a container, such as from context passed into activation of
	 * a gameplay ability.
	 *
	 * @param ContainerSpec
	 *	The original gameplay effect container specification from which to start from.
	 * @param ExistingTargetDataHandle
	 *	A handle to existing targets to which GEs in the container will be applied.
	 *
	 * @return
	 *	The modified GE container spec.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Effects")
	static FPF2GameplayEffectContainerSpec AppendTargetsToEffectContainerSpec(
		const FPF2GameplayEffectContainerSpec&  ContainerSpec,
		const FGameplayAbilityTargetDataHandle& ExistingTargetDataHandle);

	/**
	 * Builds a container of aggregator evaluation parameters, including source and target tags.
	 *
	 * Aggregator evaluation parameters influence what values are returned for captured attributes. They provide the
	 * context necessary for the Gameplay Ability System to know which GEs should or should not apply to values.
	 *
	 * @param ExecutionParams
	 *	Parameters passed-in to the execution of the Execution.
	 *
	 * @return
	 *	The aggregator evaluation parameters.
	 */
	static FORCEINLINE FAggregatorEvaluateParameters BuildEvaluationParameters(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams)
	{
		return BuildEvaluationParameters(ExecutionParams.GetOwningSpec());
	}

	/**
	 * Builds a container of aggregator evaluation parameters, including source and target tags.
	 *
	 * Aggregator evaluation parameters influence what values are returned for captured attributes. They provide the
	 * context necessary for the Gameplay Ability System to know which GEs should or should not apply to values.
	 *
	 * @param ActivatingSpec
	 *	The specification for the gameplay effect instance that is activating the calculation that needs
	 *	evaluation parameters.
	 *
	 * @return
	 *	The aggregator evaluation parameters.
	 */
	static FAggregatorEvaluateParameters BuildEvaluationParameters(const FGameplayEffectSpec& ActivatingSpec);
};
