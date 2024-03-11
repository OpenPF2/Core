// Copyright 2021-2023 Guy Elsmore-Paddock. All Rights Reserved.
// Adapted from content that is Copyright Epic Games, Inc. (Action RPG Sample).
// Licensed only for use with Unreal Engine.

#pragma once

#include <Abilities/Tasks/AbilityTask.h>

#include <Animation/AnimMontage.h>

#include "PF2AbilityTaskBase.h"

#include "PF2AbilityTask_PlayMontageAndWaitForEvent.generated.h"

/**
 * Event delegate/callback used for all pins of this type of event.
 *
 * The "EventTag" and "Payload" parameters of this event will be empty if they were triggered by standard GA system
 * montage callbacks.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPF2PlayMontageAndWaitForEventDelegate,
	FGameplayTag,       EventTag,
	FGameplayEventData, EventData
);

/**
 * A task that combines both the PlayMontageAndWait and WaitForEvent tasks into one.
 *
 * This allows the same montage to trigger one of any number of tag-based events (for example, as part of a complex
 * combat or action sequence where multiple types of damage are possible).
 */
UCLASS()
class OPENPF2CORE_API UPF2AbilityTask_PlayMontageAndWaitForEvent : public UPF2AbilityTaskBase
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Static Methods
	// =================================================================================================================
	/**
	 * Play a montage and wait for it to end.
	 *
	 * Execution pins work as follows:
	 *	- OnEventReceived will fire if a gameplay event happens and either: the event matches EventTags or EventTags is
	 *    empty. The delegate will be supplied with the tag and event data.
	 *	- OnBlendOut will fire when the montage is blending out.
	 *	- OnCompleted will fire when the montage is completely done playing.
	 *	- OnInterrupted will fire if another montage overwrites this.
	 *	- OnCancelled is called if the ability or task is cancelled.
	 *
	 * @param OwningAbility
	 *	A reference to the ability instance that is triggering this task. (This is automatically populated by Blueprint
	 *	from the ability Blueprint.)
	 * @param TaskInstanceName
	 *	A custom name for this task, in case it needs to be queried later while it is running.
	 * @param MontageToPlay
	 *	The animation montage to play on the character.
	 * @param EventTags
	 *	A list of one or more tags that, if present in an event from the montage, will activate the EventReceived
	 *	callback. If empty, any event the ability receives during the montage will trigger the callback.
	 * @param Rate
	 *	The speed at which to play the montage, in case the montage should play faster or slower. This is a speed
	 *	multiplier, so 1.0f is normal speed.
	 * @param StartSection
	 *	Optionally, the name of the montage section from which to start playback.
	 * @param bStopWhenAbilityEnds
	 *	If true, the montage will be aborted if the ability ends normally. It is always stopped when the ability is
	 *	explicitly cancelled.
	 * @param AnimRootMotionTranslationScale
	 *	Amount to scale up root motion during montage playback. Set to 0 to block root motion entirely.
	 */
	UFUNCTION(
		BlueprintCallable,
		Category="Ability|Tasks|OpenPF2",
		meta=(
			HidePin="OwningAbility",
			DefaultToSelf="OwningAbility",
			BlueprintInternalUseOnly="TRUE",
			DisplayName="PlayMontageAndWaitForEvent"
		)
	)
	static UPF2AbilityTask_PlayMontageAndWaitForEvent* CreatePlayMontageAndWaitForEvent(
		UGameplayAbility*           OwningAbility,
		const FName                 TaskInstanceName,
		UAnimMontage*               MontageToPlay,
		const FGameplayTagContainer EventTags,
		float                       Rate                           = 1.0f,
		const FName                 StartSection                   = NAME_None,
		const bool                  bStopWhenAbilityEnds           = true,
		const float                 AnimRootMotionTranslationScale = 1.0f);

protected:
	// =================================================================================================================
	// Protected Delegates/Execution Pins
	// =================================================================================================================
	/**
	 * Execution pin fired if a gameplay event with one of the tags of interest has been received.
	 */
	UPROPERTY(BlueprintAssignable)
	FPF2PlayMontageAndWaitForEventDelegate OnEventReceived;

	/**
	 * Execution pin fired when the montage that was triggered has completely finished playing.
	 */
	UPROPERTY(BlueprintAssignable)
	FPF2PlayMontageAndWaitForEventDelegate OnCompleted;

	/**
	 * Execution pin fired when the montage that was triggered starts to blend out.
	 *
	 * Not all montages are configured to blend out; this is specifically for montages with a "blend out time" that
	 * represents a duration of time at the end of montage playback during which the character will blend back to its
	 * original pose.
	 */
	UPROPERTY(BlueprintAssignable)
	FPF2PlayMontageAndWaitForEventDelegate OnBlendOut;

	/**
	 * Execution pin fired if the montage gets interrupted without completing.
	 */
	UPROPERTY(BlueprintAssignable)
	FPF2PlayMontageAndWaitForEventDelegate OnInterrupted;

	/**
	 * Execution pin fired if the ability task has been cancelled by another ability.
	 */
	UPROPERTY(BlueprintAssignable)
	FPF2PlayMontageAndWaitForEventDelegate OnCancelled;

private:
	// =================================================================================================================
	// Private Fields
	// =================================================================================================================
	/**
	 * Delegate invoked when the animation system signals that the montage is starting to blend out.
	 */
	FOnMontageBlendingOutStarted BlendingOutDelegate;

	/**
	 * Delegate invoked when the animation system signals that a montage has finished playing.
	 */
	FOnMontageEnded MontageEndedDelegate;

	/**
	 * Handle to the multi-cast cancellation delegate, so we can clean it up when the task is destroyed.
	 */
	FDelegateHandle CancelledHandle;

	/**
	 * Handle to the multi-cast tag event delegate, so we can clean it up when the task is destroyed.
	 */
	FDelegateHandle EventHandle;

	/**
	 * The name of the montage to play.
	 */
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	/**
	 * The list of one or more tags that, if present in an event from the montage, will activate the EventReceived
	 * callback.
	 *
	 * If empty, any event the ability receives during the montage will trigger the callback.
	 */
	FGameplayTagContainer EventTags;

	/**
	 * The speed at which to play the montage, in case the montage should play faster or slower. This is a speed
	 * multiplier, so 1.0f is normal speed.
	 */
	float Rate;

	/**
	 * The name of the montage section from which to start playback.
	 */
	FName StartSection;

	/**
	 * Amount to scale up root motion during montage playback. Set to 0 to block root motion entirely.
	 */
	float AnimRootMotionTranslationScale;

	/**
	 * If true, the montage will be aborted if the ability ends normally.
	 */
	bool bStopWhenAbilityEnds;

	// =================================================================================================================
	// Constructors
	// =================================================================================================================
	explicit UPF2AbilityTask_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);

	// =================================================================================================================
	// Public Methods - UAbilityTask Overrides
	// =================================================================================================================
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;

protected:
	// =================================================================================================================
	// Protected Methods - UAbilityTask Overrides
	// =================================================================================================================
	virtual void Activate() override;
	virtual void OnDestroy(bool bAbilityEnded) override;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the Ability System Component that owns the ability this task is running under.
	 *
	 * @return
	 *	The ASC for the owning ability.
	 */
	FORCEINLINE UAbilitySystemComponent* GetTargetAsc() const
	{
		check(this->AbilitySystemComponent.IsValid());

		return this->AbilitySystemComponent.Get();
	}

	/**
	 * Stops playing the montage, if it is playing.
	 *
	 * @return
	 *	Either true if a montage was playing and has been stopped; false if no montage was stopped.
	 */
	bool StopPlayingMontage() const;

	/**
	 * Callback fired when an event matching one of our tags has been received by the ASC.
	 *
	 * This turns into a broadcast/call to the appropriate execution pin in the Blueprint graph.
	 *
	 * @param EventTag
	 *	The tag that was received.
	 * @param Payload
	 *	Information about the ability activation.
	 */
	void Native_OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) const;

	/**
	 * Callback fired when a montage has ended (either normally or because it was interrupted).
	 *
	 * @param Montage
	 *	The montage that has ended.
	 * @param bInterrupted
	 *	true if the montage was cancelled/interrupted/ended abruptly; false if the montage played to completion.
	 */
	void Native_OnMontageEnded(UAnimMontage* Montage, const bool bInterrupted);

	/**
	 * Callback fired when a montage is starting to blend out.
	 *
	 * @param Montage
	 *	The montage that is blending out.
	 * @param bInterrupted
	 *	true if -- prior to blending out -- the montage was cancelled/interrupted/ended abruptly; false if the montage
	 *	played to completion.
	 */
	void Native_OnMontageBlendingOut(UAnimMontage* Montage, const bool bInterrupted) const;

	/**
	 * Callback fired if the GA running this task has been cancelled.
	 */
	void Native_OnAbilityCancelled() const;
};
