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

#include <Components/ActorComponent.h>

#include "Actors/Components/PF2ActorComponentBase.h"

#include "ModesOfPlay/Encounter/PF2CharacterInitiativeQueueInterface.h"

#include "PF2CharacterInitiativeQueueComponent.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2PlayerControllerInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UCLASS(ClassGroup="OpenPF2-ModeOfPlayRuleSets", meta=(BlueprintSpawnableComponent))
class OPENPF2CORE_API UPF2CharacterInitiativeQueueComponent final :
	public UPF2ActorComponentBase,
	public IPF2CharacterInitiativeQueueInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * A map of initiative to characters.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 13, "Initiative":
	 * "At the start of an encounter, all creatures involved roll for initiative to determine the order in which they
	 * act. The higher the result of its roll, the earlier a creature gets to act."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 468, "Step 1: Roll Initiative":
	 * "If your result is tied with a foe’s result, the adversary goes first. If your result is tied with another PC’s,
	 * you can decide between yourselves who goes first when you reach that place in the initiative order. After that,
	 * your places in the initiative order usually don’t change during the encounter."
	 */
	TMultiMap<int, IPF2CharacterInterface*> CharactersByInitiatives;

	/**
	 * All of the characters in the queue, ordered from highest to lowest initiative.
	 *
	 * This array is rebuilt whenever characters are added or removed from the queue.
	 */
	TArray<IPF2CharacterInterface*> CurrentCharacterSequence;

	/**
	 * The last character that was returned by GetNextCharacterByInitiative().
	 */
	IPF2CharacterInterface* PreviousCharacter;

	/**
	 * The initiative of the last character that was returned by GetNextCharacterByInitiative().
	 */
	int32 PreviousCharacterIndex;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2CharacterInitiativeQueueComponent.
	 */
	explicit UPF2CharacterInitiativeQueueComponent() :
		PreviousCharacter(nullptr),
		PreviousCharacterIndex(-1)
	{
	}

	// =================================================================================================================
	// Public Methods - IPF2CharacterInitiativeQueueInterface Implementation
	// =================================================================================================================
	virtual bool IsEmpty() override;

	virtual int32 GetCharacterInitiative(const TScriptInterface<IPF2CharacterInterface>& Character) const override;

	virtual void SetCharacterInitiative(const TScriptInterface<IPF2CharacterInterface>& Character,
	                                    const int32                                     Initiative) override;

	virtual bool IsInitiativeSetForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) const override;

	virtual void InsertCharacterAtOrAboveInitiative(
		const TScriptInterface<IPF2CharacterInterface>& Character,
		const int32                                     TargetInitiative) override;

	virtual void InsertCharacterAtOrBelowInitiative(
		const TScriptInterface<IPF2CharacterInterface>& Character,
		const int32                                     TargetInitiative) override;

	virtual void ClearInitiativeForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) override;

	virtual void ClearInitiativeForAllCharacters() override;

	virtual TScriptInterface<IPF2CharacterInterface> GetNextCharacterByInitiative() override;

	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetCharactersInInitiativeOrder() const override;

	// =================================================================================================================
	// Public Methods - IPF2ActorComponentInterface Implementation
	// =================================================================================================================
	virtual UActorComponent* ToActorComponent() override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	virtual FString GetIdForLogs() const override
	{
		return Super::GetIdForLogs();
	}

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets controllers for all the characters in the world.
	 *
	 * @return
	 *	OpenPF2-specific controllers for all players in the world.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
	TArray<TScriptInterface<IPF2PlayerControllerInterface>> GetPlayerControllers() const;

	/**
	 * Gets all of the player-controlled OpenPF2 characters in the world.
	 *
	 * @return
	 *	OpenPF2-specific controllers for all players in the world.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Mode of Play Rule Sets|Character Initiative Queues")
	TArray<TScriptInterface<IPF2CharacterInterface>> GetPlayerControlledCharacters() const;

	/**
	 * Rebuilds the sequence of characters according to initiative order.
	 *
	 * All characters are sorted from highest to lowest initiative order in the rebuilt list. If two characters have the
	 * same initiative, their order will be adjusted so that one goes before the other. Per OpenPF2 rules (see below),
	 * Playable Characters (PCs) with the same initiative as Non-Playable Characters (NPCs) are sorted after NPCs so
	 * that NPCs take turns first. Unlike with standard OpenPF2 rules, though, if multiple characters of the same
	 * type -- either two PCs or two NPCs -- we randomize their order rather than give each character a choice of
	 * preferred order. This helps to keep combat fluid by avoiding having to prompt players for input at the start of
	 * encounters.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 13, "Initiative":
	 * "At the start of an encounter, all creatures involved roll for initiative to determine the order in which they
	 * act. The higher the result of its roll, the earlier a creature gets to act."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 468, "Step 1: Roll Initiative":
	 * "If your result is tied with a foe’s result, the adversary goes first. If your result is tied with another PC’s,
	 * you can decide between yourselves who goes first when you reach that place in the initiative order. After that,
	 * your places in the initiative order usually don’t change during the encounter."
	 *
	 * This method performs a lot of sorting and is expensive. It should only be called when the list of characters has
	 * changed.
	 */
	void RebuildCharacterSequence();

	/**
	 * Attempts to locate the specified character in the initiative map and then remove them.
	 *
	 * This method does not rebuild the character sequence. In most cases, you will want to use
	 * ClearInitiativeForCharacter() instead, as that method calls this method and then rebuilds the character sequence.
	 *
	 * @param Character
	 *	The character being removed from the map.
	 */
	void RemoveCharacterFromInitiativeMap(const IPF2CharacterInterface* Character);

	/**
	 * Adjusts a character's initiative to occupy the specified initiative score or an offset above or below it.
	 *
	 * The adjustment proceeds as follows:
	 * 1. If the target character already has the specified initiative score, no changes to initiative are made.
	 * 2. If no character in the queue has the target initiative score, the initiative of the target character is set to
	 *    the specified initiative score.
	 * 3. If at least one character in the queue has the target initiative score:
	 *    a. The target initiative score will be incremented by the offset (the offset can be positive or negative).
	 *    b. If there is at least one character in the queue that has an initiative equal to the new initiative score
	 *       OR the target initiative score is now 0 (i.e., the original, passed-in initiative value was 1, the offset
	 *       was -1, and there was already a character with an initiative of 1):
	 *        I. All initiative scores are scaled up by 10, to ensure gaps between the existing initiative scores.
	 *       II. The target initiative score is set equal to: <Original passed-in value> * 10 + Offset. So, if this
	 *           method were invoked with an initiative score of 21 and offset of -1, the new target initiative score
	 *           would be set to 209 (21 * 10 - 1).
	 *
	 * The initiative score must be greater than zero. If an initiative of zero is provided, an error is logged and no
	 * changes to initiative score are made.
	 *
	 * If the character already has an initiative set, the character's initiative is changed to the new initiative
	 * value. If the character already has an initiative equal to the given value, no changes to initiative score are
	 * made.
	 *
	 * @param Character
	 *	The "target character" -- the character for which initiative is being set.
	 * @param TargetInitiative
	 *	The desired initiative value for the character. Must be greater than 0.
	 * @param Offset
	 *	The amount to increment or decrement the initiative if the target initiative is occupied.
	 */
	void InsertCharacterAtOrRelativeToInitiative(
		const TScriptInterface<IPF2CharacterInterface>& Character,
		const int32                                     TargetInitiative,
		const int32                                     Offset);

};
