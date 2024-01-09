// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Delegates/IDelegateInstance.h>

#include "PF2GameModeInterface.h"
#include "PF2ModeOfPlayRuleSetInterface.h"
#include "PF2PlayerControllerInterface.h"

#include "GameModes/PF2ModeOfPlayType.h"

#include "PF2ModeOfPlayRuleSetBase.generated.h"

/**
 * Default base class for OpenPF2 Mode of Play Rule Sets (MoPRS).
 *
 * @see IPF2GameModeInterface
 */
UCLASS(Abstract, Blueprintable)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2ModeOfPlayRuleSetBase : public AActor, public IPF2ModeOfPlayRuleSetInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Constants
	// =================================================================================================================
	/**
	 * The name of the root of tags that indicate that a character is in the process of dying.
	 */
	inline static const FName DyingConditionTagName = FName("Trait.Condition.Dying");

	/**
	 * The name of the tag that indicates a character is dead. This is equivalent to Trait.Condition.Dying.4.
	 */
	inline static const FName DeadConditionTagName = FName("Trait.Condition.Dead");

	/**
	 * The name of the tag that indicates a character is no longer conscious.
	 */
	inline static const FName UnconsciousConditionTagName = FName("Trait.Condition.Unconscious");

	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The root of tags that signify a character is dying.
	 *
	 * An MoPRS listens for a tag of this type to be added to or removed from a character in order to fire the
	 * Native_OnCharacterDying() and Native_OnCharacterRecoveredFromDying(), respectively.
	 */
	FGameplayTag DyingConditionTag;

	/**
	 * The tag that signifies a character is dead.
	 *
	 * An MoPRS listens for a tag of this type to be added to a character in order to fire OnCharacterDead().
	 */
	FGameplayTag DeadConditionTag;

	/**
	 * The tag that signifies a character is unconscious.
	 *
	 * An MoPRS listens for a tag of this type to be added to or removed from a character in order to fire the
	 * OnCharacterUnconscious() and OnCharacterConscious(), respectively.
	 */
	FGameplayTag UnconsciousConditionTag;

	/**
	 * Map of handles for callback delegates on condition tags.
	 */
	TMap<const TWeakObjectPtr<AActor>, TMap<FGameplayTag, FDelegateHandle>> ConditionCallbackHandles;

public:
	// =================================================================================================================
	// Public Constructor
	// =================================================================================================================
	/**
	 * Default constructor for APF2ModeOfPlayRuleSetBase.
	 */
	explicit APF2ModeOfPlayRuleSetBase();

	// =================================================================================================================
	// Public Methods - IPF2ModeOfPlayRuleSetInterface Implementation
	// =================================================================================================================
	virtual void OnModeOfPlayStart(const EPF2ModeOfPlayType ModeOfPlay) override;

	virtual void OnPlayableCharacterStarting(const TScriptInterface<IPF2CharacterInterface>& Character) override;

	virtual void OnCharacterAddedToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character) override;

	virtual void OnCharacterRemovedFromEncounter(const TScriptInterface<IPF2CharacterInterface>& Character) override;

	virtual void OnModeOfPlayEnd(const EPF2ModeOfPlayType ModeOfPlay) override;

	virtual EPF2CommandExecuteOrQueueResult AttemptToExecuteOrQueueCommand_Implementation(
		const TScriptInterface<IPF2CharacterCommandInterface>& Command) override;

	virtual bool AttemptToQueueCommand_Implementation(
		const TScriptInterface<IPF2CharacterCommandInterface>& Command) override;

	virtual void AttemptToCancelCommand_Implementation(
		const TScriptInterface<IPF2CharacterCommandInterface>& Command) override;

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Registers callbacks to invoke when a tag is added or removed from the specified character.
	 *
	 * If the character has been garbage collected, this method has no effect.
	 *
	 * @param CharacterPtr
	 *	A weak pointer to the character for which a callback will be registered.
	 * @param Tag
	 *	The tag to react to.
	 * @param OnTagAdded
	 *	The callback to invoke when the tag has been added.
	 * @param OnTagRemoved
	 *	The callback to invoke when the tag has been removed.
	 */
	void RegisterTagCallback(
		const TWeakObjectPtr<AActor>      CharacterPtr,
		const FGameplayTag&               Tag,
		void (APF2ModeOfPlayRuleSetBase::*OnTagAdded)(const TScriptInterface<IPF2CharacterInterface>&),
		void (APF2ModeOfPlayRuleSetBase::*OnTagRemoved)(const TScriptInterface<IPF2CharacterInterface>&));

	/**
	 * Unregisters all tag callbacks for the specified character.
	 *
	 * If the character has been garbage collected, this method has no effect.
	 *
	 * @param CharacterPtr
	 *	A weak pointer to the character for which callbacks are being unregistered.
	 */
	void UnregisterAllTagCallbacksForCharacter(const TWeakObjectPtr<AActor> CharacterPtr);

	/**
	 * Unregisters callbacks from being invoked when a tag is added or removed from the specified character.
	 *
	 * If the character has been garbage collected, this method has no effect.
	 *
	 * @param CharacterPtr
	 *	A weak pointer to the character for which a callback will be unregistered.
	 * @param Tag
	 *	The tag to no longer react to.
	 * @param DelegateHandle
	 *	The handle to unregister.
	 */
	void UnregisterTagCallback(
		const TWeakObjectPtr<AActor> CharacterPtr,
		const FGameplayTag&          Tag,
		const FDelegateHandle&       DelegateHandle);

	// =================================================================================================================
	// Native Events
	// =================================================================================================================
	/**
	 * Notifies this MoPRS that a character it is tracking is now unconscious.
	 *
	 * Subclasses can use this callback to react accordingly to a character that is no longer suitable for combat and
	 * interaction. For example, an encounter MoPRS will use this as an opportunity to adjust the character's
	 * initiative.
	 *
	 * Characters become unconscious for a variety of reasons, including being knocked out at 0 HP by a non-lethal
	 * attack, dying at 0 HP by a lethal attack, or merely just resting (e.g., during "Downtime" mode).
	 *
	 * @param Character
	 *	The character who has just become unconscious.
	 */
	virtual void Native_OnCharacterUnconscious(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Notifies this MoPRS that a character it is tracking is no longer unconscious.
	 *
	 * Subclasses can use this callback to react accordingly to a character that is once again suitable for combat and
	 * interaction.
	 *
	 * @param Character
	 *	The character who has just awakened after being unconscious.
	 */
	virtual void Native_OnCharacterConscious(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Notifies this MoPRS that a character it is tracking has acquired the dying condition.
	 *
	 * Subclasses can use this callback to react accordingly to a character that is at risk of death but has not yet
	 * died.
	 *
	 * @param Character
	 *	The character who has just started started dying.
	 */
	virtual void Native_OnCharacterDying(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Notifies this MoPRS that a character it is tracking no longer has the dying condition.
	 *
	 * Subclasses can use this callback to react accordingly to a character that is once again suitable for combat and
	 * interaction with other characters.
	 *
	 * @param Character
	 *	The character who has just recovered from dying.
	 */
	virtual void Native_OnCharacterRecoveredFromDying(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Notifies this MoPRS that a character it is tracking has acquired the dead condition.
	 *
	 * Subclasses can use this callback to react accordingly to a character that is no longer suitable for combat and
	 * interaction. For example, this can be used to remove the character from the encounter.
	 *
	 * @param Character
	 *	The character who has just died.
	 */
	virtual void Native_OnCharacterDead(const TScriptInterface<IPF2CharacterInterface>& Character);

	// =================================================================================================================
	// Blueprint Implementable Events
	// =================================================================================================================
	/**
	 * Callback to notify this rule set that the mode of play that invoked it is now active.
	 *
	 * The rule set should use this as an opportunity to initialize its state (e.g., roll initiative, assemble a list of
	 * enemies, etc.).
	 *
	 * @param ModeOfPlay
	 *	The mode of play that is just starting.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets",
		meta=(DisplayName="On Mode of Play Start")
	)
	void BP_OnModeOfPlayStart(EPF2ModeOfPlayType ModeOfPlay);

	/**
	 * Callback to notify this rule set that a playable character has just joined the game.
	 *
	 * This only fires if a player joins the game while this rule set is already active. The rule set should use this as
	 * an opportunity to update character-tracking logic (e.g., add the player to an existing, on-going encounter,
	 * etc.).
	 *
	 * @param Character
	 *	The character of the player who just connected.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets",
		meta=(DisplayName="On Playable Character Starting")
	)
	void BP_OnPlayableCharacterStarting(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character should be added to the current encounter.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter). This callback should also be ignored if the given character is already part of the encounter.
	 *
	 * @param Character
	 *	The character being added to the encounter.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets",
		meta=(DisplayName="On Character Added to Encounter")
	)
	void BP_OnCharacterAddedToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character it is tracking is now unconscious.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter).
	 *
	 * @param Character
	 *	The character who has just become unconscious.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets",
		meta=(DisplayName="On Character Unconscious")
	)
	void BP_OnCharacterUnconscious(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character it is tracking is no longer unconscious.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter).
	 *
	 * @param Character
	 *	The character who has just awakened after being unconscious.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets",
		meta=(DisplayName="On Character Conscious")
	)
	void BP_OnCharacterConscious(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character it is tracking has acquired the dying condition.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter).
	 *
	 * @param Character
	 *	The character who has just been knocked out.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets",
		meta=(DisplayName="On Character Dying")
	)
	void BP_OnCharacterDying(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character it is tracking no longer has the dying condition.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter).
	 *
	 * @param Character
	 *	The character who has just recovered.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets",
		meta=(DisplayName="On Character Recovered from Dying")
	)
	void BP_OnCharacterRecoveredFromDying(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character it is tracking has died.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter).
	 *
	 * @param Character
	 *	The character who has just died.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets",
		meta=(DisplayName="On Character Dead")
	)
	void BP_OnCharacterDead(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character should be removed from the current encounter.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter). This callback should also be ignored if the given character is not part of the encounter.
	 *
	 * @param Character
	 *	The character being removed from the encounter.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets",
		meta=(DisplayName="On Character Removed from Encounter")
	)
	void BP_OnCharacterRemovedFromEncounter(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set to wrap-up prior to a change in mode of play.
	 *
	 * The rule set should use this as an opportunity to apply any long-lasting effects of the mode (e.g., calculate
	 * experience and hero points, end encounter-only gameplay effects or abilities, etc.).
	 *
	 * @param ModeOfPlay
	 *	The mode of play that is ending.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets",
		meta=(DisplayName="On Mode of Play End")
	)
	void BP_OnModeOfPlayEnd(EPF2ModeOfPlayType ModeOfPlay);

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the game mode.
	 *
	 * @return
	 *	OpenPF2-specific game mode.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets")
	TScriptInterface<IPF2GameModeInterface> GetGameMode() const;

	/**
	 * Gets controllers for all the characters in the world.
	 *
	 * @return
	 *	OpenPF2-specific controllers for all players in the world.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets")
	TArray<TScriptInterface<IPF2PlayerControllerInterface>> GetPlayerControllers() const;

	/**
	 * Gets all of the player-controlled OpenPF2 characters in the world.
	 *
	 * @return
	 *	OpenPF2-specific controllers for all players in the world.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets")
	TArray<TScriptInterface<IPF2CharacterInterface>> GetPlayerControlledCharacters() const;

	/**
	 * Adds a character to the current encounter, if an encounter is active.
	 *
	 * This call will have no effect under the following circumstances:
	 *	- If there is no active encounter according to game mode rules or the current Mode of Play Rule Set (MoPRS).
	 *	- If there is an active encounter and the given character is already part of the encounter.
	 *
	 * @param Character
	 *	The character being added to the encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets")
	void AddCharacterToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Adds all characters controlled by players to the current encounter, if an encounter is active.
	 *
	 * This call will have no effect under the following circumstances:
	 *	- If there is no active encounter according to game mode rules or the current Mode of Play Rule Set (MoPRS).
	 *	- If there is an active encounter and all player-controlled characters are already part of the encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets")
	void AddAllPlayerControlledCharactersToEncounter();

	/**
	 * Removes a character from the current encounter, if an encounter is active.
	 *
	 * This call will have no effect under the following circumstances:
	 *	- If there is no active encounter according to game mode rules or the current Mode of Play Rule Set (MoPRS).
	 *	- If there is an active encounter and no characters are part of the encounter.
	 *
	 * @param Character
	 *	The character being removed from the encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets")
	void RemoveCharacterFromEncounter(const TScriptInterface<IPF2CharacterInterface> Character);
};
