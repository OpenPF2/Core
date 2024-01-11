// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//   - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//   - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//   - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
//
// Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
// as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
// file other than the material designated as Open Game Content may be reproduced in any form without written
// permission.

#pragma once

#include <AbilitySystemInterface.h>

#include <UObject/ScriptInterface.h>

#include "Abilities/PF2AbilityBoostBase.h"

#include "PF2CharacterInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterCommandInterface;
class IPF2CommandQueueInterface;
class IPF2OwnerTrackingInterface;
class IPF2PlayerControllerInterface;

// =====================================================================================================================
// Normal Declarations - Delegates
// =====================================================================================================================
/**
 * Delegate for Blueprints to react to when a character's turn has started or ended.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FPF2CharacterTurnDelegate,
	const TScriptInterface<IPF2CharacterInterface>&, Character
);

// =====================================================================================================================
// Normal Declarations - Types
// =====================================================================================================================
/**
 * The "Events" object for PF2CharacterInterface.
 *
 * This is a concrete UObject that contains only the dynamic multicast delegates that instances of the interface expose
 * to consumers for binding.
 *
 * @see IPF2EventEmitterInterface
 */
UCLASS()
class OPENPF2CORE_API UPF2CharacterInterfaceEvents : public UObject
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Fields - Multicast Delegates
	// =================================================================================================================
	/**
	 * Event fired when character's encounter turn has started.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Characters")
	FPF2CharacterTurnDelegate OnEncounterTurnStarted;

	/**
	 * Event fired when character's encounter turn has ended.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Characters")
	FPF2CharacterTurnDelegate OnEncounterTurnEnded;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2CharacterInterface : public UAbilitySystemInterface
{
    GENERATED_BODY()
};

/**
 * An interface for OpenPF2 characters.
 */
class OPENPF2CORE_API IPF2CharacterInterface : public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods - IAbilitySystemInterface Implementation
	// =================================================================================================================
	/**
	 * Gets the UObject version of the Ability System Component (ASC) for this character.
	 *
	 * @return
	 *	The ASC for this character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override = 0;

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Returns a unique identifier for this object, for logging and debugging purposes.
	 *
	 * BUGBUG: This duplicates IPF2LogIdentifiableInterface::GetIdForLogs() because it seems that UE does not generate
	 * code properly for interfaces that extend *multiple* other interfaces.
	 *
	 * @return
	 *	A unique identifier for object in debug logs.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual FString GetIdForLogs() const = 0;

	/**
	 * Gets the events object used for binding Blueprint callbacks to events from this component.
	 *
	 * @return
	 *	The events object for this interface.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual UPF2CharacterInterfaceEvents* GetEvents() const = 0;

	/**
	 * Returns the name of this character, as set by the game designer.
	 *
	 * If no name has been set, then the UObject name of the character is returned instead.
	 *
	 * @return
	 *	The name of this character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual FText GetCharacterName() const = 0;

	/**
	 * Gets a visual portrait of this character, to represent them in the UI to players/users.
	 *
	 * @return
	 *	A graphical representation of this character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual UTexture2D* GetCharacterPortrait() const = 0;

	/**
	 * Gets the current level of this character.
	 *
	 * The character level impacts the character's stats and how many boosts and feats the character can have.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 31, "Leveling Up":
	 * "Each level grants greater skill, increased resiliency, and new capabilities, allowing your character to face
	 * even greater challenges and go on to earn even more impressive rewards."
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual int32 GetCharacterLevel() const = 0;

	/**
	 * Gets an OpenPF2-specific version of the Ability System Component (ASC) for this character.
	 *
	 * @return
	 *	The ASC, as an implementation of the interface for character ASCs.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual TScriptInterface<IPF2CharacterAbilitySystemInterface> GetCharacterAbilitySystemComponent() const = 0;

	/**
	 * Gets the sub-component of this character that is used to track commands queued during encounters.
	 *
	 * @return
	 *	The command queue component, if one is available. Otherwise, a script interface that wraps a null pointer.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual TScriptInterface<IPF2CommandQueueInterface> GetCommandQueueComponent() const = 0;

	/**
	 * Gets the sub-component of this character that is used to track which player owns this character.
	 *
	 * @return
	 *	The owner tracking component, if one is available. Otherwise, a script interface that wraps a null pointer.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual TScriptInterface<IPF2OwnerTrackingInterface> GetOwnerTrackingComponent() const = 0;

	/**
	 * Gets the player controller for this character, if this character is controllable by a player.
	 *
	 * This method behaves differently depending upon if it is being invoked on a client or on the server:
	 * - On clients: For characters controllable by the local player, this will return the local player controller.
	 *   Otherwise, this will return null.
	 * - On the server:
	 *     - For a character that is possessed by a player controller, this will return the player controller that is
	 *       possessing the character.
	 *     - Otherwise, if the character has an owner tracking component, this will return the player controller that
	 *       "owns" the character.
	 *
	 * @return
	 *	Either the player controller for this character, or a null player controller reference if this character is not
	 *	currently in the list of controllable characters for any player that is locally accessible.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual TScriptInterface<IPF2PlayerControllerInterface> GetPlayerController() const = 0;

	/**
	 * Gets all of the ability boosts that have been granted to this character that require choices from the player.
	 *
	 * @return
	 *	The ability boost GAs that are still pending for this character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual TArray<TScriptInterface<IPF2AbilityBoostInterface>> GetPendingAbilityBoosts() const = 0;

	/**
	 * Initializes or refreshes the ability system component and all other related ability logic for this character.
	 *
	 * Characters typically invoke this method automatically on both the client and server side whenever they are being
	 * possessed by a player controller or AI controller.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual void InitializeOrRefreshAbilities() = 0;

	/**
	 * Gets the actor that is implementing this interface.
	 *
	 * @return
	 *	This character, as an actor.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual AActor* ToActor() = 0;

	/**
	 * Gets the pawn that is implementing this interface.
	 *
	 * @return
	 *	This character, as a pawn.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual APawn* ToPawn() = 0;

	/**
	 * Determines if this character is living (i.e., has hit points > 0).
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual bool IsAlive() = 0;

	/**
	 * Applies a single ability boost selection to this character.
	 *
	 * The ability score selections must be compatible with the rule options of the Boost GA.
	 *
	 * The ability boost does not take effect immediately. Invoke ApplyAbilityBoostSelections() to apply all of the
	 * selections that have been added.
	 *
	 * @param BoostGameplayAbility
	 *	The "Boost GA" -- the Gameplay Ability for which ability score boost selections are being applied.
	 * @param SelectedAbilities
	 *	The ability scores that the player selected, out of the options offered by the Boost GA.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual void AddAbilityBoostSelection(
	    const TSubclassOf<UPF2AbilityBoostBase>   BoostGameplayAbility,
	    const TSet<EPF2CharacterAbilityScoreType> SelectedAbilities) = 0;

	/**
	 * Attempts to find and activate a pending ability boost Gameplay Ability for each Ability Boost selection on this
	 * character.
	 *
	 * Pending ability boosts are registered on this character via calls to AddAbilityBoostSelection() before a call to
	 * this method.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual void ApplyAbilityBoostSelections() = 0;

	/**
	 * Activates Gameplay Effects that are always passively applied to the character.
	 *
	 * This is the preferred method for toggling passive GEs on for a character that supports ability boosts instead of
	 * toggling passive GEs on the character's ASC, as this method automatically skips out of offering boost selections
	 * for which a player or game designer has already made choices.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual void ActivatePassiveGameplayEffects() = 0;

	/**
	 * Removes all passive Gameplay Effects that were previously activated for this character.
	 *
	 * This is the preferred method for toggling passive GEs off for a character that supports ability boosts (e.g.,
	 * during a character level-up) instead of toggling passive GEs off at the character's ASC, as it gives the code for
	 * the character a chance to react to the change before involving the ASC. For example, if the state of any GAs
	 * needs to be recorded so that they are re-applied correctly after passive GEs are re-activated.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual void DeactivatePassiveGameplayEffects() = 0;

	/**
	 * Adds a Gameplay Ability (GA) to this character and then immediately activates it.
	 *
	 * This method is intended for use by one-shot GAs that react to events in the environment or game story. As such,
	 * the GA is NOT added to the character's "AdditionalGameplayAbilities" property, as that property is intended to
	 * control the GAs that are added to the character at creation time.
	 *
	 * @param Ability
	 *	The type of ability to add and activate on the character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual void AddAndActivateGameplayAbility(const TSubclassOf<UGameplayAbility> Ability) = 0;

	// =================================================================================================================
	// Public Event Notifications from Attribute Set
	// =================================================================================================================
	/**
	 * Notifies this character that it has received damage.
	 *
	 * This should only be invoked by the character's attribute set. This does NOT actually modify the character's
	 * stats. Once invoked, this method fires off appropriate callbacks into the character's Blueprint event graph.
	 *
	 * @param Damage
	 *	The amount of the damage.
	 * @param InstigatorCharacter
	 *	The character that is ultimately responsible for the damage. This can be null if the damage is caused by the
	 *	environment.
	 * @param DamageSource
	 *	The actor that directly inflicted the damage, such as a weapon or projectile.
	 * @param SourceTags
	 *	Tags passed along with the damage Gameplay Event that were captured from the source ability and source
	 *	character.
	 * @param HitInfo
	 *	Hit result information, including who was hit and where the damage was inflicted.
	 */
	virtual void Native_OnDamageReceived(const float                  Damage,
	                                     IPF2CharacterInterface*      InstigatorCharacter,
	                                     AActor*                      DamageSource,
	                                     const FGameplayTagContainer* SourceTags,
	                                     const FHitResult             HitInfo) = 0;

	/**
	 * Notifies this character that its hit points (i.e., health) have changed.
	 *
	 * This should only be invoked by the character's attribute set. This does NOT actually modify the character's
	 * stats. Once invoked, this method fires off appropriate callbacks into the character's Blueprint event graph.
	 *
	 * @param Delta
	 *	The amount that the character's hit points have changed.
	 * @param NewValue
	*	The new amount of hit points after the change.
	 * @param SourceTags
	 *	Tags passed along with the Gameplay Event that were captured from the source ability and source character as
	 *	metadata about the cause of the change to hit points.
	 */
	virtual void Native_OnHitPointsChanged(const float                  Delta,
	                                       const float                  NewValue,
	                                       const FGameplayTagContainer* SourceTags) = 0;

	/**
	 * Notifies this character that its speed (i.e., how fast it can move during a stride) has changed.
	 *
	 * This should only be invoked by the character's attribute set. This does NOT actually modify the character's
	 * stats. Once invoked, this method fires off appropriate callbacks into the character's Blueprint event graph.
	 *
	 * @param Delta
	 *	The amount that the character's speed has changed.
	 * @param NewValue
	*	The new amount of speed after the change.
	 * @param SourceTags
	 *	Tags passed along with the Gameplay Event that were captured from the source ability and source character as
	 *	metadata about the cause of the change to speed.
	 */
	virtual void Native_OnSpeedChanged(const float                  Delta,
	                                   const float                  NewValue,
	                                   const FGameplayTagContainer* SourceTags) = 0;

	// =================================================================================================================
	// Public Event Notifications from Mode of Play Rule Sets (MoPRS)
	// =================================================================================================================
	/**
	 * Notifies this character that their turn during an encounter has started.
	 *
	 * (This should normally be invoked only by the MoPRS).
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_OnEncounterTurnStarted() = 0;

	/**
	 * Notifies this character that their turn during an encounter has ended.
	 *
	 * (This should normally be invoked only by the MoPRS).
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_OnEncounterTurnEnded() = 0;
};
