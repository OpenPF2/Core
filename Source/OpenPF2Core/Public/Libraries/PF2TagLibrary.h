// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

// ReSharper disable CppUEBlueprintCallableFunctionUnused
#pragma once

#include <GameplayTagContainer.h>

#include <Kismet/BlueprintFunctionLibrary.h>

#include "PF2TagLibrary.generated.h"

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Function library for working with gameplay abilities in OpenPF2.
 */
UCLASS()
class OPENPF2CORE_API UPF2TagLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Locates the tag within the specified tag container that is a child of another tag.
	 *
	 * If there are multiple tags in the container that are children or grandchildren of the given tag, only the first
	 * one gets returned.
	 *
	 * @param [in] AllTags
	 *	The tags to search.
	 * @param [in] ParentTag
	 *	The parent (or grandparent) tag of the desired tag.
	 * @param [out] bMatchFound
	 *	Whether a tag was found that is a child of the specified tag.
	 *
	 * @return
	 *	Either the matching tag; or a gameplay tag that is not valid if there is no tag that matches.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(AutoCreateRefTerm="ParentTag"), Category="OpenPF2|Gameplay Tags")
	static FGameplayTag FindChildTag(const FGameplayTagContainer& AllTags,
	                                 const FGameplayTag&          ParentTag,
	                                 bool&                        bMatchFound);

	/**
	 * Finds the condition trait tag having the specified parent tag and parses the condition level into an integer.
	 *
	 * For example, given a container with the following tags and a parent tag of "Condition.Dying", this would parse
	 * and return a value of "3":
	 * - CreatureSize.Medium
	 * - CreatureAlignment.Neutral.Good
	 * - KeyAbility.Intelligence
	 * - Trait.Condition.Dying.3
	 * - Trait.Condition.Wounded.1
	 *
	 * If there are multiple tags in the container that are children or grandchildren of the given tag, only the first
	 * one gets parsed.
	 *
	 * @param AllTags
	 *	The container of tags within which to locate the condition trait tag.
	 * @param ParentTag
	 *	The tag immediately above the tags that contain the integer condition level.
	 *
	 * @return
	 *	Either the value parsed out of the suffix; or, 0 if either a condition trait tag could not be found or the tag
	 *	that was found did not have a suffix tag that could be parsed.
	 */
	UFUNCTION(
		BlueprintCallable,
		BlueprintPure,
		meta=(AutoCreateRefTerm="ParentTag"),
		Category="OpenPF2|Gameplay Tags"
	)
	static uint8 FindAndParseConditionLevel(const FGameplayTagContainer& AllTags,
	                                        const FGameplayTag&          ParentTag);

	/**
	 * Parses the condition level suffix from a condition trait tag into an integer.
	 *
	 * For example, this parses tags like "Trait.Condition.Dying.4" and "Trait.Condition.Wounded.3" into the values
	 * "4" and "3", respectively.
	 *
	 * @param Tag
	 *	The tag from which to extract and parse the condition level.
	 * @param ParentTag
	 *	The tag immediately above the tags that contain the integer condition level.
	 *
	 * @return
	 *	Either the value parsed out of the suffix; or, 0 if the tag did not have a condition level that could be parsed.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(AutoCreateRefTerm="ParentTag"), Category="OpenPF2|Gameplay Tags")
	static uint8 ParseConditionLevel(const FGameplayTag& Tag,
	                                 const FGameplayTag& ParentTag);
};
