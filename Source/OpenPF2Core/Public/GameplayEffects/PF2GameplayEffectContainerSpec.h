// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// Adapted from content that is Copyright Epic Games, Inc. (Action RPG Sample).
// Licensed only for use with Unreal Engine.

#pragma once

#include <GameplayEffect.h>

#include "CharacterStats/AbilityBoosts/PF2GameplayAbilityTargetData_BoostAbility.h"

#include "PF2GameplayEffectContainerSpec.generated.h"

/**
 * The "processed" version of an PF2GameplayEffectContainer in which all effects are ready to apply to targets.
 *
 * When an instance of this struct is created, CDOs for all gameplay effect types in the source GE container are loaded
 * and converted into gameplay effect specifications on the server. The resulting specs are then added to the instance
 * of this struct, enabling it to be passed around freely from client to server, from gameplay abilities into gameplay
 * effect executions.
 */
USTRUCT(BlueprintType)
struct FPF2GameplayEffectContainerSpec
{
	GENERATED_BODY()

	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor.
	 */
	FPF2GameplayEffectContainerSpec()
	{
	}

	// =================================================================================================================
	// Public Fields
	// =================================================================================================================
	/**
	 * The target(s) of the ability being executed.
	 *
	 * At the time that an instance of this container spec is created, this can be populated by target(s) provided at
	 * the time the GA was queued or executed by the player. For example, when a player is controlling a character and
	 * presses a button on the controller while the player has chosen another RPG character as a target, that target can
	 * be used to initialize this spec.
	 *
	 * Then, once this spec has been created but before it has been applied, additional targets can be added freely to
	 * capture additional targets that might be affected. For example, at the time that it is queued, an area-of-effect
	 * ability might only have a target location within the world but no target characters; then, at execution time, all
	 * targets within a certain radius of the target location would then be added as targets before this spec gets
	 * applied to its targets, applying the area of effect ability to all the matching targets.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="OpenPF2|Gameplay Effects")
	FGameplayAbilityTargetDataHandle TargetData;

	/**
	 * The gameplay effect specifications (specs) to apply to targets when this container gets applied.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="OpenPF2|Gameplay Effects")
	TArray<FGameplayEffectSpecHandle> GameplayEffectSpecsToApply;

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets whether this effect container currently has any gameplay effects to apply.
	 *
	 * @return
	 *	- true if this container has at least one GE spec.
	 *	- false if this container has no GE specs.
	 */
	FORCEINLINE bool HasEffects() const
	{
		return !this->GameplayEffectSpecsToApply.IsEmpty();
	}

	/**
	 * Gets whether this effect container currently has any targets (either hits or actors).
	 *
	 * @return
	 *	- true if this container has at least one target.
	 *	- false if this container has no targets.
	 */
	FORCEINLINE bool HasTargets() const
	{
		return !this->GameplayEffectSpecsToApply.IsEmpty();
	}

	/**
	 * Adds the given gameplay effect specification handle to this container.
	 *
	 * @param GameplayEffectSpec
	 *	The specification handle to add.
	 */
	FORCEINLINE void AddGameplayEffectSpec(const FGameplayEffectSpecHandle& GameplayEffectSpec)
	{
		this->GameplayEffectSpecsToApply.Add(GameplayEffectSpec);
	}

	/**
	 * Adds one or more hit-result-based targets to which GEs in this container will be applied.
	 *
	 * @param HitResults
	 *	The list of hit results to add as targets.
	 */
	void AddHitTargets(const TArray<FHitResult>& HitResults);

	/**
	 * Adds one or more actor targets to which GEs in this container will be applied.
	 *
	 * @param TargetActors
	 *	The list of actors to add as targets.
	 */
	void AddActorTargets(const TArray<AActor*>& TargetActors);

	/**
	 * Adds existing generic target data to this container, such as from context passed into GA activation.
	 *
	 * @param ExistingTargetData
	 *	An existing target to which GEs in this container will be applied.
	 */
	void AddTargetData(FGameplayAbilityTargetData* ExistingTargetData);

	/**
	 * Adds existing generic targets to this container, such as from context passed into GA activation.
	 *
	 * @param ExistingTargetDataHandle
	 *	A handle to existing targets to which GEs in this container will be applied.
	 */
	void AppendTargets(const FGameplayAbilityTargetDataHandle& ExistingTargetDataHandle);
};
