// OpenPF2 for UE Game Logic, Copyright 2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "PF2AbilityAsync_WaitConditionBase.h"
#include "PF2AbilityAsync_WaitCharacterConditionAdded.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * An async. task for non-ability blueprints (e.g., UMG widgets) to react to addition of a condition on characters.
 */
UCLASS()
class OPENPF2CORE_API UPF2AbilityAsync_WaitConditionAdded : public UPF2AbilityAsync_WaitCharacterConditionBase
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Declarations
	// =================================================================================================================
	/**
	 * Delegate for the graph pin fired when a condition is added to a character.
	 *
	 * @param ConditionTag
	 *	The tag for the specific condition (including level) that was added to the character.
	 * @param ConditionLevel
	 *	The parsed condition level.
	 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
		FPF2AsyncWaitConditionAddedDelegate,
		const FGameplayTag&, ConditionTag,
		uint8,               ConditionLevel
	);

	// =================================================================================================================
	// Public Static Methods
	// =================================================================================================================
	/**
	 * Waits until the specified condition gameplay tag is added to the ASC of the target character.
	 *
	 * The task will broadcast the "On Condition Added" event the first time that the specified condition tag is added
	 * or changes levels, and it will broadcast again on future additions unless "Only Trigger Once" is "false".
	 *
	 * If "Fire Immediately If Already Satisfied" is "true" and the target character already has the specified
	 * condition, the "On Condition Added" event will be broadcast immediately.
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
		DisplayName="Wait for Condition Added to Character",
		Category="OpenPF2|Ability|Tasks"
	)
	static UPF2AbilityAsync_WaitConditionAdded* WaitConditionAddedToCharacter(
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
	FPF2AsyncWaitConditionAddedDelegate OnConditionAdded;

	// =================================================================================================================
	// Protected Methods - UPF2AbilityAsync_WaitCharacterConditionBase Overrides
	// =================================================================================================================
	virtual void OnTagCriterionSatisfied(const FGameplayTag& ConditionTag) override;
};
