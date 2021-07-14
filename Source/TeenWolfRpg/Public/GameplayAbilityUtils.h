﻿// Teen Wolf RPG game logic, Copyright 2021, Teen Wolf RPG. All Rights Reserved.

//=============================================================================
// GameplayAbilityUtils.h: Utility logic for working with Gameplay Abilities.
//=============================================================================

#pragma once

#include <CoreMinimal.h>

#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

namespace GameplayAbilityUtils
{
	/**
	 * Creates an attribute capture definition for the specified Gameplay Attribute.
	 *
	 * @param Attribute
	 *   The Gameplay Attribute that should be captured.
	 *
	 * @return
	 *   The capture definition for the specified attribute.
	 */
	FGameplayEffectAttributeCaptureDefinition BuildSourceCaptureFor(const FGameplayAttribute Attribute);

	/**
	 * Checks if a tag with the given name or prefix is present.
	 *
	 * @param Tags
	 *   The list of tags in which to search.
	 * @param TagNameOrPrefix
	 *   The name of the tag or the prefix.
	 *
	 * @return
	 *   - TRUE if given a tag name, and a tag with the specified name is present in the tag list.
	 *   - TRUE if given a tag prefix, and there is a tag present in the tag list that starts with that prefix.
	 *   - FALSE, otherwise.
	 */
	FORCEINLINE static bool HasTag(const FGameplayTagContainer *Tags, const FString TagNameOrPrefix)
	{
		return Tags->HasTag(FGameplayTag::RequestGameplayTag(FName(TagNameOrPrefix)));
	}
}