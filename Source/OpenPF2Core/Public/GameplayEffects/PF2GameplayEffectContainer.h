// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// Adapted from content that is Copyright Epic Games, Inc. (Action RPG Sample).
// Licensed only for use with Unreal Engine.

#pragma once

#include <GameplayEffect.h>

#include "PF2GameplayEffectContainer.generated.h"

/**
 * A container for applying one or more gameplay effects to the source or target of an attack.
 */
USTRUCT(BlueprintType)
struct FPF2GameplayEffectContainer
{
	GENERATED_BODY()

	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor.
	 */
	FPF2GameplayEffectContainer()
	{
	}

	// =================================================================================================================
	// Public Fields
	// =================================================================================================================
	/**
	 * All the types of gameplay effects to apply when this container gets applied to a target.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="OpenPF2|Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectsToApply;
};
