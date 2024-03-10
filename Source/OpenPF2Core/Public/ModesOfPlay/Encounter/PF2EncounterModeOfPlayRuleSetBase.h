// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/ScriptInterface.h>

#include "Commands/PF2CommandExecuteImmediatelyResult.h"

#include "ModesOfPlay/PF2ModeOfPlayRuleSetBase.h"
#include "ModesOfPlay/Encounter/PF2CharacterInitiativeQueueInterface.h"
#include "ModesOfPlay/Encounter/PF2EncounterModeOfPlayRuleSetInterface.h"

#include "PF2EncounterModeOfPlayRuleSetBase.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterCommandInterface;
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Default base class for OpenPF2 Mode of Play Rule Sets (MoPRS) that need encounter logic.
 *
 * Encounters often involve each character "rolling" for their initiative order, and then performing attacks in order
 * from highest to lowest initiative score. Encounters also usually involve characters needing to "queue-up"
 * abilities/commands that they want to perform when it is their turn. To make it easy for game designers, this base
 * class composes together several actor components together to expose this functionality under the
 * IPF2EncounterModeOfPlayRuleSetInterface.
 *
 * Since OpenPF2 is intended to support action RPGs just as well as turn-based RPGs, you have the freedom to extend this
 * base class with a Blueprint sub-class that directs how this functionality gets applied, or you can ignore this base
 * class and implement the IPF2EncounterModeOfPlayRuleSetInterface interface in your own C++ base class and compose
 * functionality however you like. For example, in your game, you might want characters to act one-by-one; or, you may
 * prefer to cycle through characters at a rapid clip to keep combat flowing despite the turn-based nature of standard
 * OpenPF2 rules.
 *
 * @see IPF2EncounterModeOfPlayRuleSetInterface
 */
UCLASS(Abstract, Blueprintable)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2EncounterModeOfPlayRuleSetBase :
	public APF2ModeOfPlayRuleSetBase, public IPF2EncounterModeOfPlayRuleSetInterface
{
	GENERATED_BODY()

protected:
	/**
	 * The component of the MoPRS that maintains the list of characters and their initiatives.
	 */
	TScriptInterface<IPF2CharacterInitiativeQueueInterface> CharacterInitiativeQueue;

	/**
	 * The character whose turn it is in the encounter.
	 *
	 * Can be null if in between turns or no character has started a turn.
	 */
	TScriptInterface<IPF2CharacterInterface> ActiveCharacter;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for APF2EncounterModeOfPlayRuleSetBase.
	 */
	explicit APF2EncounterModeOfPlayRuleSetBase();

	// =================================================================================================================
	// Public Methods - IPF2ModeOfPlayRuleSetInterface Overrides
	// =================================================================================================================
	virtual void OnModeOfPlayEnd(const EPF2ModeOfPlayType ModeOfPlay) override;

	// =================================================================================================================
	// Public Methods - IPF2EncounterModeOfPlayRuleSetInterface Implementation
	// =================================================================================================================
	virtual bool HavePlayableCharacters() const override;

	virtual void SetCharacterInitiative(const TScriptInterface<IPF2CharacterInterface>& Character,
	                                    const int32                                     Initiative) override;

	virtual bool IsInitiativeSetForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) const override;

	virtual void ClearInitiativeForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) override;

	virtual void ClearInitiativeForAllCharacters() override;

	virtual TScriptInterface<IPF2CharacterInterface> GetNextCharacterByInitiative() override;

	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetAllCharactersInInitiativeOrder() const override;

	virtual TScriptInterface<IPF2CharacterInterface> GetActiveCharacter() const override;

	virtual void MoveInitiativeHigherThanCharacter(
		const TScriptInterface<IPF2CharacterInterface>& AffectedCharacter,
		const TScriptInterface<IPF2CharacterInterface>& OtherCharacter
	) const override;

	virtual void MoveInitiativeLowerThanCharacter(
		const TScriptInterface<IPF2CharacterInterface>& AffectedCharacter,
		const TScriptInterface<IPF2CharacterInterface>& OtherCharacter
	) const override;

	virtual void StartTurnForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) override;

	virtual void EndTurnForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) override;

	virtual bool DoesCharacterHaveNextCommandQueued(const TScriptInterface<IPF2CharacterInterface>& Character) const override;

	virtual void QueueCommandForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&        Character,
		const TScriptInterface<IPF2CharacterCommandInterface>& Command) override;

	virtual void CancelQueuedCommandsForAllCharacters() override;

	virtual EPF2CommandExecuteImmediatelyResult ExecuteNextQueuedCommandForCharacter(
		const TScriptInterface<IPF2CharacterInterface>& Character) override;

	virtual void PeekNextQueuedCommandForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&  Character,
		TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) const override;

	virtual void PopNextCommandQueuedForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&  Character,
		TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) override;

protected:
	// =================================================================================================================
	// Blueprint Implementable Events
	// =================================================================================================================
	/**
	 * Callback to notify this rule set that a character's turn is starting.
	 *
	 * @param Character
	 *	The character whose turn is starting.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets|Encounters",
		meta=(DisplayName="On Chararacter Turn Start")
	)
	void BP_OnCharacterTurnStart(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character's turn is ending.
	 *
	 * @param Character
	 *	The character whose turn is ending.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Mode of Play Rule Sets|Encounters",
		meta=(DisplayName="On Chararacter Turn End")
	)
	void BP_OnCharacterTurnEnd(const TScriptInterface<IPF2CharacterInterface>& Character);

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the component of the MoPRS that maintains the list of characters and their initiatives.
	 *
	 * @return
	 *	The character initiative queue.
	 */
	FORCEINLINE TScriptInterface<IPF2CharacterInitiativeQueueInterface> GetCharacterInitiativeQueue() const
	{
		return this->CharacterInitiativeQueue;
	}

	/**
	 * Sets the character whose turn it is.
	 *
	 * @param NewActiveCharacter
	 *	The character to make the active character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Encounters")
	void SetActiveCharacter(const TScriptInterface<IPF2CharacterInterface>& NewActiveCharacter);

	/**
	 * Removes all characters from the current encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Encounters")
	void RemoveAllCharactersFromEncounter();
};
