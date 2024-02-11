// OpenPF2 for UE Game Logic, Copyright 2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Abilities/Async/AbilityAsync_WaitGameplayTag.h>

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2AbilityAsync_WaitConditionBase.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================

/**
 * Abstract base class for async. tasks in non-ability blueprints to react to character condition tags.
 */
UCLASS(Abstract)
class OPENPF2CORE_API UPF2AbilityAsync_WaitCharacterConditionBase : public UAbilityAsync, public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

protected:
	enum EWaitCriterion
	{
		None,
		TagAdded,
		TagRemoved,
	};

	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The tag immediately above the tag that contains the integer condition level (e.g., "Trait.Condition.Dying").
	 */
	FGameplayTag ConditionParentTag;

	/**
	 * What criterion needs to be satisfied for OnTagCriterionSatisfied() to be triggered.
	 */
	EWaitCriterion CriterionToSatisfy;

	/**
	 * Whether to evaluate the tag criterion upon starting this task and then fire the callback if has been satisfied.
	 */
	bool bFireImmediatelyIfAlreadySatisfied;

	/**
	 * Whether to end this async task after the first time it fires.
	 */
	bool bOnlyTriggerOnce;

	/**
	 * Whether the condition supports levels (e.g., Trait.Condition.Dying.1) or is just on/off (Trait.Condition.Dead).
	 */
	bool bConditionSupportsLevels;

	/**
	 * The handles of the callback registered with the ASC.
	 */
	TMap<FGameplayTag, FDelegateHandle> CallbackHandles;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	explicit UPF2AbilityAsync_WaitCharacterConditionBase() :
		CriterionToSatisfy(None),
		bFireImmediatelyIfAlreadySatisfied(true),
		bOnlyTriggerOnce(false),
		bConditionSupportsLevels(true)
	{
	}

protected:
	// =================================================================================================================
	// Protected Methods - UBlueprintAsyncActionBase Overrides
	// =================================================================================================================
	virtual void Activate() override;

	// =================================================================================================================
	// Protected Methods - UAbilityAsync Overrides
	// =================================================================================================================
	virtual void EndAction() override;

	// =================================================================================================================
	// Protected Methods - IPF2LogIdentifiableInterface Overrides
	// =================================================================================================================
	virtual FString GetIdForLogs() const override;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets whether the condition supports levels.
	 *
	 * @return
	 *	- true if the condition supports levels (e.g., Trait.Condition.Dying.1).
	 *	- false if the condition is either on or off, but does not support levels (e.g., Trait.Condition.Dead).
	 */
	FORCEINLINE bool DoesConditionSupportLevels() const
	{
		return this->bConditionSupportsLevels;
	}

	/**
	 * Registers a callback on the ASC for the specified condition tag.
	 *
	 * @param ConditionTag
	 *	The tag for which a callback should be registered.
	 */
	void SetupCallbackForConditionTag(const FGameplayTag& ConditionTag);

	/**
	 * Callback invoked by the character ASC when the count on a condition tag of interest has changed.
	 *
	 * @param ConditionTag
	 *	The condition tag that has been added or removed from the ASC.
	 * @param NewCount
	 *	The new stack count for the condition tag.
	 */
	void OnConditionTagCountChanged(const FGameplayTag ConditionTag, int32 NewCount);

	/**
	 * Invokes the callback on this task if the tag criterion has been satisfied, and then optionally ends this task.
	 *
	 * This invokes OnTagCriterionSatisfied() under the following circumstances:
	 *   - The criterion to satisfy is EWaitCriterion::TagAdded and the ASC of the character has any level of the
	 *     condition tag of interest currently applied.
	 *   - The criterion to satisfy is EWaitCriterion::TagRemoved and the ASC of the character does not have any level
	 *     of the condition tag of interest currently applied.
	 *
	 * @param ConditionTag
	 *	The condition tag (with level) that has been added or removed from the ASC.
	 */
	void NotifyIfCriterionSatisfied(const FGameplayTag& ConditionTag);

	/**
	 * Invokes the callback on this task if the tag criterion has been satisfied, and then optionally ends this task.
	 *
	 * This overload expects the stack count of the tag to be passed in.
	 *
	 * This invokes OnTagCriterionSatisfied() under the following circumstances:
	 *   - The criterion to satisfy is EWaitCriterion::TagAdded and the ASC of the character has any level of the
	 *     condition tag of interest currently applied (NewCount > 0).
	 *   - The criterion to satisfy is EWaitCriterion::TagRemoved and the ASC of the character does not have any level
	 *     of the condition tag of interest currently applied (NewCount == 0).
	 *
	 * @param ConditionTag
	 *	The condition tag (with level) that has been added or removed from the ASC.
	 * @param NewCount
	 *	The new count for a condition tag of interest.
	 */
	void NotifyIfCriterionSatisfied(const FGameplayTag& ConditionTag, int32 NewCount);

	/**
	 * Method invoked by this class when the desired count on a condition tag of interest has been satisfied.
	 *
	 * Subclasses must override this method to provide the behavior that should be invoked for the change to condition
	 * tag.
	 *
	 * @param ConditionTag
	 *	The tag for which the criterion was satisfied.
	 */
	virtual void OnTagCriterionSatisfied(const FGameplayTag& ConditionTag)
	PURE_VIRTUAL(UPF2AbilityAsync_WaitCharacterConditionBase::OnTagCriterionSatisfied);
};
