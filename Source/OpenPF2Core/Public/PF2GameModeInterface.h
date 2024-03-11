// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/Interface.h>
#include <UObject/ScriptInterface.h>

#include "Commands/PF2CommandExecuteOrQueueResult.h"

#include "ModesOfPlay/PF2ModeOfPlayType.h"

#include "PF2GameModeInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterCommandInterface;
class IPF2CharacterInterface;
class IPF2ModeOfPlayRuleSetInterface;
class IPF2PartyInterface;
class IPF2PlayerControllerInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2GameModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible Player Modes.
 *
 * Instances of this interface exist only on the SERVER, as is the case with a game mode in any UE-powered game.
 */
class OPENPF2CORE_API IPF2GameModeInterface
{
	GENERATED_BODY()

public:
	/**
	 * Creates the appropriate rule set for the given mode of play.
	 *
	 * @return
	 *	The rule set for the current mode of play.
	 */
	virtual TScriptInterface<IPF2ModeOfPlayRuleSetInterface> CreateModeOfPlayRuleSet(const EPF2ModeOfPlayType ModeOfPlay) = 0;

	/**
	 * Transfers ownership of the specified character from one player to another, as identified by player controller.
	 *
	 * @param Character
	 *	The character that is being transferred.
	 * @param ControllerOfNewOwner
	 *	The player controller that is being made the new owner of the character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual void TransferCharacterOwnership(
		const TScriptInterface<IPF2CharacterInterface>        Character,
		const TScriptInterface<IPF2PlayerControllerInterface> ControllerOfNewOwner
	) = 0;

	/**
	 * Changes the party affiliation of a player.
	 *
	 * This will automatically notify the affected parties of the change. Thus, if the player already belongs to a
	 * party, that party is notified to remove the player. If the new party is not null, it is notified to add the
	 * player.
	 *
	 * If the player has controllable characters, all of them will be released during this transition. If this behavior
	 * is undesirable, use SwitchPartyOfPlayerAndOwnedCharacters() instead.
	 *
	 * @param PlayerController
	 *	The player controller that corresponds to the player who is switching parties.
	 * @param NewParty
	 *	The new party affiliation for the player. Can be null to indicate that the player should not belong to any
	 *	party.
	 *
	 * @see SwitchPartyOfPlayerAndOwnedCharacters
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual void SwitchPartyOfPlayer(
		const TScriptInterface<IPF2PlayerControllerInterface> PlayerController,
		const TScriptInterface<IPF2PartyInterface>            NewParty
	) = 0;

	/**
	 * Changes the party affiliation of a player and each of its controllable characters.
	 *
	 * This will automatically notify the affected parties of the change. Thus, if the player already belongs to a
	 * party, that party is notified to remove the player. If the new party is not null, it is notified to add the
	 * player.
	 *
	 * If the player has controllable characters, each of them will be switched to be affiliated with the new party
	 * during this transition. If this behavior is undesirable, use SwitchPartyOfPlayer() instead.
	 *
	 * @param PlayerController
	 *	The player controller that corresponds to the player who is switching parties.
	 * @param NewParty
	 *	The new party affiliation for the player and its characters. Can be null to indicate that the player should not
	 *	belong to any party.
	 *
	 * @see SwitchPartyOfPlayer
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual void SwitchPartyOfPlayerAndOwnedCharacters(
		const TScriptInterface<IPF2PlayerControllerInterface> PlayerController,
		const TScriptInterface<IPF2PartyInterface>            NewParty
	) = 0;

	/**
	 * Requests a switch of the play mode to encounter mode.
	 *
	 * The request is subject to any business rules of the game mode. If the switch is not allowed by current game
	 * rules, the game mode will not be changed.
	 *
	 * @see EPF2ModeOfPlay
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual void RequestEncounterMode() = 0;

	/**
	 * Requests a switch of the play mode to exploration mode.
	 *
	 * The request is subject to any business rules of the game mode. If the switch is not allowed by current game
	 * rules, the game mode will not be changed.
	 *
	 * @see EPF2ModeOfPlay
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual void RequestExplorationMode() = 0;

	/**
	 * Requests a switch of the play mode to downtime mode.
	 *
	 * The request is subject to any business rules of the game mode. If the switch is not allowed by current game
	 * rules, the game mode will not be changed.
	 *
	 * @see EPF2ModeOfPlay
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual void RequestDowntimeMode() = 0;

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
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual void AddCharacterToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Removes a character from the current encounter, if an encounter is active.
	 *
	 * This call will have no effect under the following circumstances:
	 *	- If there is no active encounter according to game mode rules or the current Mode of Play Rule Set (MoPRS).
	 *	- If there is an active encounter but the given character is not already part of the encounter.
	 *
	 * @param Character
	 *	The character being added to the encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual void RemoveCharacterFromEncounter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Notifies game rules and/or the Mode of Play Rule Set (MoPRS) that a character wishes to perform a command.
	 *
	 * If the current mode is structured (e.g., Encounter mode), then the command should be placed into a queue of
	 * commands for the character -- preserving the order that the commands were queued -- and the command should be
	 * executed when it is the character's turn (e.g., according to initiative order). On the other hand, if the current
	 * Mode of Play allows characters to perform commands immediately, the command may not be queued and might instead
	 * be given the opportunity to run before this call returns. Alternatively, the command may be dropped without being
	 * executed if the character's queue has a size limit and the queue is currently full.
	 *
	 * @param Command
	 *	The command being queued.
	 *
	 * @return
	 *	A result that indicates whether the command was queued, executed immediately, or refused.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual EPF2CommandExecuteOrQueueResult AttemptToExecuteOrQueueCommand(
		const TScriptInterface<IPF2CharacterCommandInterface>& Command) = 0;

	/**
	 * Notifies game rules and/or the Mode of Play Rule Set (MoPRS) that a character wishes to queue a command.
	 *
	 * If the current mode is structured (e.g., Encounter mode), then the command should be placed into a queue of
	 * commands for the character -- preserving the order that the commands were queued -- and the command should be
	 * executed when it is the character's turn (e.g., according to initiative order). On the other hand, if the current
	 * Mode of Play allows characters to perform commands immediately, the command should be ignored and might not be
	 * queued. Alternatively, the command may be dropped if the character's queue has a size limit and the queue is
	 * currently full.
	 *
	 * @param Command
	 *	The command being queued.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual bool AttemptToQueueCommand(const TScriptInterface<IPF2CharacterCommandInterface>& Command) = 0;

	/**
	 * Notifies game rules and/or the Mode of Play Rule Set (MoPRS) that a character wishes to cancel a command.
	 *
	 * If the current mode is structured (e.g., Encounter mode), then the command will be removed from the queue of
	 * commands for the character. On the other hand, if the current Mode of Play allows characters to perform commands
	 * immediately, canceling the command will have no effect because it is likely already being executed or has
	 * finished executing.
	 *
	 * @param Command
	 *	The command to cancel.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	virtual void AttemptToCancelCommand(const TScriptInterface<IPF2CharacterCommandInterface>& Command) = 0;
};
