// OpenPF2 for UE Game Logic, Copyright 2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "PF2AbilityAsync_WaitConditionBase.h"
#include "PF2AbilityAsync_WaitCharacterConditionRemoved.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * An async. task for non-ability blueprints (e.g., UMG widgets) to react to removal of a condition on characters.
 */
UCLASS()
class OPENPF2CORE_API UPF2AbilityAsync_WaitCharacterConditionRemoved : public UPF2AbilityAsync_WaitCharacterConditionBase
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Declarations
	// =================================================================================================================
	/**
	 * Delegate for the graph pin fired when a condition is removed from a character.
	 *
	 * @param ConditionTag
	 *	The tag for the specific condition (including level) that was removed from the character.
	 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
		FPF2AsyncWaitConditionLevelRemovedDelegate,
		const FGameplayTag&, ConditionTag
	);

	// =================================================================================================================
	// Public Static Methods
	// =================================================================================================================
	/**
	 * Waits until the specified condition gameplay tag is removed from the ASC of the target character.
	 *
	 * The task will broadcast the "On Condition Removed" event the first time that the specified condition tag is
	 * removed, and it will broadcast again on future removals unless "Only trigger once" is "false". If the character
	 * does not have the specified condition when this task is started, the event will be broadcast immediately.
	 *
	 * The task does not broadcast on changes to the level of a condition (e.g., going from "Trait.Condition.Dying.1" to
	 * "Trait.Condition.Dying.2"), only on a removal of the condition entirely.
	 *
	 * If "Fire Immediately If Already Satisfied" is "true" and the target character does not have the specified
	 * condition, the "On Condition Removed" event will be broadcast immediately.
	 *
	 * @param Character
	 *	The character for which condition levels are being monitored.
	 * @param ConditionParentTag
	 *	The tag immediately above the tag that contains the integer condition level (e.g., "Trait.Condition.Dying").
	 * @param bFireImmediatelyIfAlreadySatisfied
	 *	Whether to evaluate the tag criterion upon starting this task and then fire the callback if has been satisfied.
	 * @param bOnlyTriggerOnce
	 *	Only broadcast the event once and then automatically cancel the task.
	 */
	UFUNCTION(
		BlueprintCallable,
		meta=(DefaultToSelf="Character", BlueprintInternalUseOnly="TRUE"),
		DisplayName="Wait for Condition Removed from Character",
		Category="OpenPF2|Ability|Tasks"
	)
	static UPF2AbilityAsync_WaitCharacterConditionRemoved* WaitConditionRemovedFromCharacter(
		const TScriptInterface<IPF2CharacterInterface>& Character,
		const FGameplayTag                              ConditionParentTag,
		const bool                                      bFireImmediatelyIfAlreadySatisfied = true,
		const bool                                      bOnlyTriggerOnce = false
	);

protected:
	// =================================================================================================================
	// Protected Delegates/Execution Pins
	// =================================================================================================================
	UPROPERTY(BlueprintAssignable)
	FPF2AsyncWaitConditionLevelRemovedDelegate OnConditionRemoved;

	// =================================================================================================================
	// Protected Methods - UPF2AbilityAsync_WaitCharacterConditionBase Overrides
	// =================================================================================================================
	virtual void OnTagCriterionSatisfied(const FGameplayTag& ConditionTag) override;
};
