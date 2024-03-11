// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
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

#include <UObject/ScriptInterface.h>

#include "Commands/PF2CommandExecuteImmediatelyResult.h"

#include "ModesOfPlay/PF2ModeOfPlayRuleSetInterface.h"

#include "PF2EncounterModeOfPlayRuleSetInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterCommandInterface;
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2EncounterModeOfPlayRuleSetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An encounter-specific sibling interface to the OpenPF2 Mode of Play Rule Set (MoPRS) interface.
 *
 * Encounters tend to maintain initiative order, keep track of the number of enemies left standing, and have to keep
 * track of which character is taking a turn. These aspects are not relevant to other game modes like exploration mode
 * and downtime mode, so they are not part of the standard MoPRS interface for OpenPF2.
 *
 * This interface does not extend the IPF2ModeOfPlayRuleSetInterface because the UE type system does not allow
 * interfaces to contain both BlueprintImplementableEvents and BlueprintCallable functions.
 */
class OPENPF2CORE_API IPF2EncounterModeOfPlayRuleSetInterface : public IInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets whether there are playable characters with initiative set in this encounter.
	 *
	 * @return
	 *	- true if there is at least one playable character who is not dead and who has initiative set.
	 *	- false if there are no playable, living characters with initiative set.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Characters")
	virtual bool HavePlayableCharacters() const = 0;

	/**
	 * Sets the initiative of the specified character.
	 *
	 * If the character already has an initiative set, the character's initiative is changed to the specified value.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 13, "Initiative":
	 * "At the start of an encounter, all creatures involved roll for initiative to determine the order in which they
	 * act. The higher the result of its roll, the earlier a creature gets to act."
	 *
	 * @param Character
	 *	The character for which initiative is being set.
	 * @param Initiative
	 *	The initiative value to use for the specified character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Initiative")
	virtual void SetCharacterInitiative(const TScriptInterface<IPF2CharacterInterface>& Character,
	                                    const int32                                     Initiative) = 0;

	/**
	 * Determines if the specified character has an initiative set.
	 *
	 * @param Character
	 *	The character for which initiative is being checked.
	 *
	 * @return
	 *	- true if the character has an initiative set.
	 *	- false if the character does not have any initiative set.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Initiative")
	virtual bool IsInitiativeSetForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) const = 0;

	/**
	 * Clears any initiative value set for the specified character.
	 *
	 * If the character does not have any initiative set, this has no effect.
	 *
	 * @param Character
	 *	The character for which initiative is being cleared.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Initiative")
	virtual void ClearInitiativeForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Clears all initiative values set for all characters.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Initiative")
	virtual void ClearInitiativeForAllCharacters() = 0;

	/**
	 * Gets the next character in initiative order, looping around to the first character when at the end of the list.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 468, "Step 2: Play a Round":
	 * "A round begins when the participant with the highest initiative roll result starts their turn, and it ends when
	 * the one with the lowest initiative ends their turn."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 468, "Step 3: Begin the Next Round":
	 * "Once everyone in the encounter has taken a turn, the round is over and the next one begins. Don’t roll
	 * initiative again; the new round proceeds in the same order as the previous one, repeating the cycle until the
	 * encounter ends."
	 *
	 * This method maintains an internal iterator in this rule set. If initiative is set, changed, or cleared for
	 * characters in the encounter after iteration has begun, the iterator can make only the following two guarantees
	 * about the next character to be returned relative to the character that was returned the last time this method was
	 * called:
	 * 1. The next character will be a different character than the last character, unless there is only one character
	 *    in the encounter.
	 * 2. The next character will have either a lower initiative or an equal initiative to the last character, unless
	 *    the last character returned was the character in the encounter with the lowest initiative.
	 *
	 * Only characters that have an initiative set are returned; all others are ignored.
	 *
	 * @return
	 *	The next character in the encounter who has an initiative equal to or lower than the character that was last
	 *	returned; or, if at the end of the list of characters, the character with the highest initiative in the
	 *	encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Initiative")
	virtual TScriptInterface<IPF2CharacterInterface> GetNextCharacterByInitiative() = 0;

	/**
	 * Gets all characters in the order of their initiative.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 13, "Initiative":
	 * "At the start of an encounter, all creatures involved roll for initiative to determine the order in which they
	 * act. The higher the result of its roll, the earlier a creature gets to act."
	 *
	 * Only characters that have an initiative set are returned; all others are excluded.
	 *
	 * @return
	 *	The list of characters, sorted in order of highest to lowest initiative.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Initiative")
	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetAllCharactersInInitiativeOrder() const = 0;

	/**
	 * Gets the character whose turn it is.
	 *
	 * @return
	 *	The active character. Can be nullptr if in between turns or no character has started a turn in the current
	 *	encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Turns")
	virtual TScriptInterface<IPF2CharacterInterface> GetActiveCharacter() const = 0;

	/**
	 * Adjusts the initiative order of the specified character so they have a higher initiative score than another.
	 *
	 * A higher initiative score will enable the affected character to act before the specified character.
	 *
	 * The AffectedCharacter does not need to have an initiative set before this method is called. If the character
	 * already had an initiative set, it will be modified to the new initiative. If the OtherCharacter does not have an
	 * initiative set, an error is logged and no initiative change occurs.
	 *
	 * @param AffectedCharacter
	 *	The character whose initiative order will be adjusted.
	 * @param OtherCharacter
	 *	The character before whom the character will be slotted in, based on initiative.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="OpenPF2|Encounter Mode of Play Rule Sets|Turns")
	virtual void MoveInitiativeHigherThanCharacter(
		const TScriptInterface<IPF2CharacterInterface>& AffectedCharacter,
		const TScriptInterface<IPF2CharacterInterface>& OtherCharacter
	) const = 0;

	/**
	 * Adjusts the initiative order of the specified character so they have a lower initiative score than another.
	 *
	 * A lower initiative score will enable the affected character to act after the specified character.
	 *
	 * The AffectedCharacter does not need to have an initiative set before this method is called. If the character
	 * already had an initiative set, it will be modified to the new initiative. If the OtherCharacter does not have an
	 * initiative set, an error is logged and no initiative change occurs.
	 *
	 * @param AffectedCharacter
	 *	The character whose initiative order will be adjusted.
	 * @param OtherCharacter
	 *	The character after whom the character will be slotted in, based on initiative.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="OpenPF2|Encounter Mode of Play Rule Sets|Turns")
	virtual void MoveInitiativeLowerThanCharacter(
		const TScriptInterface<IPF2CharacterInterface>& AffectedCharacter,
		const TScriptInterface<IPF2CharacterInterface>& OtherCharacter
	) const = 0;

	/**
	 * Signals the start of the specified character's turn.
	 *
	 * @param Character
	 *	The character for whom a turn is starting.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Turns")
	virtual void StartTurnForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Signals the end of the specified character's turn.
	 *
	 * @param Character
	 *	The character for whom a turn is ending.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Turns")
	virtual void EndTurnForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Returns whether there is another command queued for the specified character.
	 *
	 * @param Character
	 *	The character for which commands will be inspected.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Command Queue")
	virtual bool DoesCharacterHaveNextCommandQueued(
		const TScriptInterface<IPF2CharacterInterface>& Character) const = 0;

	/**
	 * Adds the specified command to the queue of commands being maintained for the specified OpenPF2 character.
	 *
	 * @param Character
	 *	The character that is queuing the command up.
	 * @param Command
	 *	The command that is being queued.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Command Queue")
	virtual void QueueCommandForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&        Character,
		const TScriptInterface<IPF2CharacterCommandInterface>& Command) = 0;

	/**
	 * Cancels and clears all commands queued for all characters.
	 *
	 * This is typically fired right before a transition out of an encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Command Queue")
	virtual void CancelQueuedCommandsForAllCharacters() = 0;

	/**
	 * Performs the next command in the specified character's queue of commands (if there is one).
	 *
	 * The command is automatically removed from the character's queue of commands.
	 *
	 * @param Character
	 *	The character whose turn it is to act.
	 *
	 * @return
	 *	- EPF2CommandExecuteImmediatelyResult::None if the character is out of commands to execute.
	 *	- EPF2CommandExecuteImmediatelyResult::Activated if the character had an command queued up that was executed.
	 *	- EPF2CommandExecuteImmediatelyResult::Blocked if the character has an command to execute but it cannot be
	 *	  executed yet, typically because it is blocked by another ability that is active on the character.
	 *	- EPF2CommandExecuteImmediatelyResult::Cancelled if the rule set has opted to cancel the command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Command Queue")
	virtual UPARAM(DisplayName = "Activation Result") EPF2CommandExecuteImmediatelyResult
		ExecuteNextQueuedCommandForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Returns the next command in the specified character's queue of commands (if there is one).
	 *
	 * The command is not removed from the queue.
	 *
	 * @param Character
	 *	The character for which an command is desired.
	 * @param NextCommand
	 *	The output -- either the next command for the specified character; or an invalid reference if the character does
	 *	not have any more queued commands.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Command Queue")
	virtual void PeekNextQueuedCommandForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&  Character,
		TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) const = 0;

	/**
	 * Removes and returns the next command in the specified character's queue of commands (if there is one).
	 *
	 * The command is removed from the queue.
	 *
	 * @param Character
	 *	The character for which an command is desired.
	 * @param NextCommand
	 *	The output -- either the next command for the specified character; or an invalid reference if the character does
	 *	not have any more queued commands.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Encounter Mode of Play Rule Sets|Command Queue")
	virtual void PopNextCommandQueuedForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&  Character,
		TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) = 0;
};
