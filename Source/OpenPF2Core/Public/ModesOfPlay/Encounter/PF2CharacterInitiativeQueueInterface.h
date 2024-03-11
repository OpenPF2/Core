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

#include "Actors/Components/PF2ActorComponentInterface.h"

#include "PF2CharacterInitiativeQueueInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2CharacterInitiativeQueueInterface : public UPF2ActorComponentInterface
{
	GENERATED_BODY()
};

/**
 * An interface for objects that maintain queues of commands/actions.
 */
class OPENPF2CORE_API IPF2CharacterInitiativeQueueInterface : public IPF2ActorComponentInterface
{
	GENERATED_BODY()

public:
	/**
	 * Determines if there are any characters in this queue.
	 *
	 * @return
	 *	- true if the queue has no characters.
	 *	- false if the queue has at least one character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
	virtual bool IsEmpty() = 0;

	/**
	 * Gets the initiative of the specified character.
	 *
	 * @param Character
	 *	The character for which initiative is being fetched.
	 *
	 * @return
	 *	The initiative of the specified character. If no initiative has been set for the given character, INDEX_NONE is
	 *	returned.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
	virtual int32 GetCharacterInitiative(const TScriptInterface<IPF2CharacterInterface>& Character) const = 0;

	/**
	 * Sets the initiative of the specified character.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 13, "Initiative":
	 * "At the start of an encounter, all creatures involved roll for initiative to determine the order in which they
	 * act. The higher the result of its roll, the earlier a creature gets to act."
	 *
	 * The initiative score must be greater than zero. If an initiative of zero is provided, an error is logged and no
	 * changes to initiative score are made.
	 *
	 * If the character already has an initiative set, the character's initiative is changed to the specified value. If
	 * the character already has an initiative equal to the given value, no changes to initiative score are made.
	 *
	 * @param Character
	 *	The character for which initiative is being set.
	 * @param Initiative
	 *	The initiative value to use for the specified character. Must be greater than 0.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
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
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
	virtual bool IsInitiativeSetForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) const = 0;

	/**
	 * Adjusts a character's initiative to occupy the specified initiative score or immediately above it.
	 *
	 * The adjustment proceeds as follows:
	 * 1. If the target character already has the specified initiative score, no changes to initiative are made.
	 * 2. If no character in the queue has the target initiative score, the initiative of the target character is set to
	 *    the specified initiative score.
	 * 3. If at least one character in the queue has the target initiative score:
	 *    a. The target initiative score will be incremented by 1.
	 *    b. If there is at least one character in the queue that has an initiative equal to the new initiative score:
	 *        I. All initiative scores are scaled up by 10, to ensure gaps between the existing initiative scores.
	 *       II. The target initiative score is set equal to: <Original passed-in value> * 10 + 1. So, if this method
	 *           were invoked with an initiative score of 21, the new target initiative score would be set to 211 (21 *
	 *           10 + 1).
	 *
	 * This ensures that the target character has an initiative score one point higher than the occupied initiative but
	 * lower than any other characters who had a higher initiative score than the character occupying the target
	 * initiative score.
	 *
	 * The initiative score must be greater than zero. If an initiative of zero is provided, an error is logged and no
	 * changes to initiative score are made.
	 *
	 * If the character already has an initiative set, the character's initiative is changed to the specified value. If
	 * the character already has an initiative equal to the given value, no changes to initiative score are made.
	 *
	 * @param Character
	 *	The "target character" -- the character for which initiative is being set.
	 * @param TargetInitiative
	 *	The desired initiative value for the character. Must be greater than 0.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
	virtual void InsertCharacterAtOrAboveInitiative(
		const TScriptInterface<IPF2CharacterInterface>& Character,
		const int32                                     TargetInitiative) = 0;

	/**
	 * Adjusts a character's initiative to occupy the specified initiative score or immediately below it.
	 *
	 * The adjustment proceeds as follows:
	 * 1. If the target character already has the specified initiative score, no changes to initiative are made.
	 * 2. If no character in the queue has the target initiative score, the initiative of the target character is set to
	 *    the specified initiative score.
	 * 3. If at least one character in the queue has the target initiative score:
	 *    a. The target initiative score will be decremented by 1.
	 *    b. If there is at least one character in the queue that has an initiative equal to the new initiative score
	 *       OR the target initiative score is now 0 (i.e., the original, passed-in initiative value was 1 and there was
	 *       already a character with an initiative of 1):
	 *        I. All initiative scores are scaled up by 10, to ensure gaps between the existing initiative scores.
	 *       II. The target initiative score is set equal to: <Original passed-in value> * 10 - 1. So, if this method
	 *           were invoked with an initiative score of 21, the new target initiative score would be set to
	 *           209 (21 * 10 - 1).
	 *
	 * This ensures that the target character has an initiative score one point lower than the occupied initiative but
	 * higher than any other characters who had a lower initiative score than the character occupying the target
	 * initiative score.
	 *
	 * The initiative score must be greater than zero. If an initiative of zero is provided, an error is logged and no
	 * changes to initiative score are made.
	 *
	 * If the character already has an initiative set, the character's initiative is changed to the specified value. If
	 * the character already has an initiative equal to the given value, no changes to initiative score are made.
	 *
	 * @param Character
	 *	The "target character" -- the character for which initiative is being set.
	 * @param TargetInitiative
	 *	The desired initiative value for the character. Must be greater than 0.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
	virtual void InsertCharacterAtOrBelowInitiative(
		const TScriptInterface<IPF2CharacterInterface>& Character,
		const int32                                     TargetInitiative) = 0;

	/**
	 * Clears any initiative value set for the specified character.
	 *
	 * If the character does not have any initiative set, this has no effect.
	 *
	 * @param Character
	 *	The character for which initiative is being cleared.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
	virtual void ClearInitiativeForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Clears all initiative values set for all characters.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
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
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
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
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetCharactersInInitiativeOrder() const = 0;
};
