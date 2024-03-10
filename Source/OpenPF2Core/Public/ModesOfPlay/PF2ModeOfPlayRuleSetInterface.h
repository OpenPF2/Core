// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/Interface.h>

#include "PF2CharacterInterface.h"
#include "PF2GameStateInterface.h"

#include "Commands/PF2CommandExecuteOrQueueResult.h"

#include "ModesOfPlay/PF2ModeOfPlayType.h"

#include "PF2ModeOfPlayRuleSetInterface.generated.h"

UINTERFACE(BlueprintType)
class OPENPF2CORE_API UPF2ModeOfPlayRuleSetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible Mode of Play Rule Sets (MoPRS), which provide strategies for how the game should
 * behave while in a particular mode of OpenPF2 play.
 *
 * MoPRS not only provide logic that control how different gameplay events are handled, but also can act as an extension
 * to game state by storing and maintaining variables that are relevant for the current mode of play. For example,
 * encounter modes maintain initiative order, number of enemies left standing, etc. which are not relevant in other game
 * modes like exploration mode. A new MoPRS instance is created each time that the mode of play changes, so this state
 * is only maintained while it is relevant.
 *
 * @see EPF2ModeOfPlayType
 */
class OPENPF2CORE_API IPF2ModeOfPlayRuleSetInterface : public IInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods
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
	virtual void OnModeOfPlayStart(const EPF2ModeOfPlayType ModeOfPlay) = 0;

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
	virtual void OnPlayableCharacterStarting(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Callback to notify this rule set that a character should be added to the current encounter.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter). This callback should also be ignored if the given character is already part of the encounter.
	 *
	 * @param Character
	 *	The character being added to the encounter.
	 */
	virtual void OnCharacterAddedToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Callback to notify this rule set that a character should be removed from the current encounter.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter). This callback should also be ignored if the given character is not part of the encounter.
	 *
	 * @param Character
	 *	The character being removed from the encounter.
	 */
	virtual void OnCharacterRemovedFromEncounter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Callback to notify this rule set to wrap-up prior to a change in mode of play.
	 *
	 * The rule set should use this as an opportunity to apply any long-lasting effects of the mode (e.g., calculate
	 * experience and hero points, end encounter-only gameplay effects or abilities, etc.).
	 *
	 * @param ModeOfPlay
	 *	The mode of play that is ending.
	 */
	virtual void OnModeOfPlayEnd(const EPF2ModeOfPlayType ModeOfPlay) = 0;

	/**
	 * Notifies this rule set that a character wishes to perform a command (e.g., use an ability).
	 *
	 * This gives the rule set control over when the command should be performed (e.g., to enforce initiative order).
	 * The command may not get executed if the encounter ends before it has been activated. In such a situation, the
	 * command will be cancelled instead.
	 *
	 * If the current mode is structured (e.g., Encounter mode), then the command should be placed into a queue of
	 * commands for the character -- preserving the order that the commands were queued -- and the command should be
	 * executed when it is the character's turn (e.g., according to initiative order). On the other hand, if the current
	 * Mode of Play allows characters to perform commands immediately, the command may not be queued and might instead
	 * be given the opportunity to run before this call returns. Alternatively, the command may be dropped without being
	 * executed if the character's queue has a size limit and the queue is currently full.
	 *
	 * @param Command
	 *	The command that is being queued.
	 *
	 * @return
	 *	A result that indicates whether the command was queued, executed immediately, or refused.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="OpenPF2|Mode of Play Rule Sets")
	EPF2CommandExecuteOrQueueResult AttemptToExecuteOrQueueCommand(
		const TScriptInterface<IPF2CharacterCommandInterface>& Command);

	/**
	 * Notifies this rule set that a character wishes to queue a command without trying to execute it first.
	 *
	 * This gives the rule set control over when the command should be performed (e.g., to enforce initiative order).
	 * The command may not get executed if the encounter ends before it has been activated. In such a situation, the
	 * command will be cancelled instead.
	 *
	 * If the current mode is structured (e.g., Encounter mode), then the command should be placed into a queue of
	 * commands for the character -- preserving the order that the commands were queued -- and the command should be
	 * executed when it is the character's turn (e.g., according to initiative order). On the other hand, if the current
	 * Mode of Play allows characters to perform commands immediately, the command should be ignored and might not be
	 * queued. Alternatively, the command may be dropped if the character's queue has a size limit and the queue is
	 * currently full.
	 *
	 * @param Command
	 *	The command that is being queued.
	 *
	 * @return
	 *	- true if the command was able to be queued.
	 *	- false if the command could not be queued.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="OpenPF2|Mode of Play Rule Sets")
	bool AttemptToQueueCommand(const TScriptInterface<IPF2CharacterCommandInterface>& Command);

	/**
	 * Notifies this rule set that a character wishes to cancel a command.
	 *
	 * This gives the rule set control over if/when a command should be cancelled. The command may not get cancelled if
	 * it is already being executed or has finished being executed. In such a situation, the command will still proceed.
	 *
	 * @param Command
	 *	The command that is being cancelled.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="OpenPF2|Mode of Play Rule Sets")
	void AttemptToCancelCommand(const TScriptInterface<IPF2CharacterCommandInterface>& Command);

	/**
	 * Determines whether this rule set allows transitioning to the specified mode of play with the given game state.
	 *
	 * Some modes freely allow transitions to other game modes, while others place restrictions on transitions so that
	 * they depend on certain conditions being met. For example, it is common that encounters prevent players from
	 * returning to exploration until they have either: 1) vanquished all enemies within range; 2) retreated
	 * successfully; or 3) forced remaining enemies to retreat. Meanwhile, it is common for players to be able to leave
	 * exploration and downtime modes of play without any conditions.
	 *
	 * @param GameState
	 *	The current game state.
	 * @param TargetMode
	 *	The mode of play to which the game is attempting to transition.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Mode of Play Rule Sets")
	bool CanTransitionTo(const TScriptInterface<IPF2GameStateInterface>& GameState,
	                     const EPF2ModeOfPlayType                        TargetMode) const;
};
