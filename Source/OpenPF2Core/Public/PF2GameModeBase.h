// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#pragma once

#include <GameFramework/GameModeBase.h>
#include <GameFramework/GameStateBase.h>

#include <UObject/ScriptInterface.h>

#include "PF2GameModeInterface.h"

#include "ModesOfPlay/PF2ModeOfPlayRuleSetBase.h"

#include "PF2GameModeBase.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class APF2Party;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Default base class for OpenPF2 Game Modes.
 *
 * A single instance of this class exists only on the SERVER, as is the case with a game mode in any UE-powered game.
 *
 * @see IPF2GameModeInterface
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2GameModeBase : public AGameModeBase, public IPF2GameModeInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * Map from Modes of Play to the Rule Set to use for each mode.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="OpenPF2 - Game Modes")
	TMap<EPF2ModeOfPlayType, TSubclassOf<APF2ModeOfPlayRuleSetBase>> ModeRuleSets;

public:
	// =================================================================================================================
	// Public Methods - IPF2GameModeInterface Implementation
	// =================================================================================================================
	virtual TScriptInterface<IPF2ModeOfPlayRuleSetInterface> CreateModeOfPlayRuleSet(
		const EPF2ModeOfPlayType ModeOfPlay) override;

	virtual void TransferCharacterOwnership(
		const TScriptInterface<IPF2CharacterInterface>        Character,
		const TScriptInterface<IPF2PlayerControllerInterface> NewController
	) override;

	virtual void SwitchPartyOfPlayer(
		const TScriptInterface<IPF2PlayerControllerInterface> PlayerController,
		const TScriptInterface<IPF2PartyInterface>            NewParty
	) override;

	virtual void SwitchPartyOfPlayerAndOwnedCharacters(
		const TScriptInterface<IPF2PlayerControllerInterface> PlayerController,
		const TScriptInterface<IPF2PartyInterface>            NewParty
	) override;

	virtual void RequestEncounterMode() override;

	virtual void RequestExplorationMode() override;

	virtual void RequestDowntimeMode() override;

	virtual void AddCharacterToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character) override;

	virtual void RemoveCharacterFromEncounter(const TScriptInterface<IPF2CharacterInterface>& Character) override;

	virtual EPF2CommandExecuteOrQueueResult AttemptToExecuteOrQueueCommand(
		const TScriptInterface<IPF2CharacterCommandInterface>& Command) override;

	virtual bool AttemptToQueueCommand(const TScriptInterface<IPF2CharacterCommandInterface>& Command) override;

	virtual void AttemptToCancelCommand(const TScriptInterface<IPF2CharacterCommandInterface>& Command) override;

protected:
	// =================================================================================================================
	// Protected Methods - AActor Overrides
	// =================================================================================================================
	virtual void BeginPlay() override;

	// =================================================================================================================
	// Protected Methods - AGameModeBase Overrides
	// =================================================================================================================
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the game state as an OpenPF2-compatible game state interface.
	 *
	 * @return
	 *	The game state.
	 */
	FORCEINLINE IPF2GameStateInterface* GetGameStateIntf() const
	{
		IPF2GameStateInterface* GameStateIntf = this->GetGameState<IPF2GameStateInterface>();

		check(GameStateIntf != nullptr);

		return GameStateIntf;
	}

	/**
	 * Gets the active Mode of Play Rule Set (MoPRS) from the game state.
	 *
	 * @return
	 *	The active MoPRS, wrapped in a script interface (for Blueprint). If there is not a compatible game state loaded,
	 *	or there is no active MoPRS, the script interface wraps nullptr.
	 */
	virtual TScriptInterface<IPF2ModeOfPlayRuleSetInterface> GetModeOfPlayRuleSet();

	/**
	 * Generates a new, unassigned index for a player.
	 *
	 * @return
	 *	The next player index.
	 */
	FORCEINLINE int32 GeneratePlayerIndex() const
	{
		return this->GetGameStateIntf()->GeneratePlayerIndex();
	}

	/**
	 * Generates a new, unassigned index for a party.
	 *
	 * @return
	 *	The next party index.
	 */
	FORCEINLINE int32 GeneratePartyIndex() const
	{
		return this->GetGameStateIntf()->GeneratePartyIndex();
	}

	/**
	 * Assigns the specified player controller a new, unused player index.
	 *
	 * @param PlayerController
	 *	The player controller that will be assigned a distinct player index.
	 */
	void AssignPlayerIndex(const APlayerController* PlayerController) const;

	/**
	 * Spawns a new party instance and assigns it a party index.
	 *
	 * @param PartyType
	 *	The specific type of party to generate.
	 *
	 * @return
	 *	The new party instance.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Mode")
	TScriptInterface<IPF2PartyInterface> SpawnParty(
		UPARAM(meta=(MustImplement="/Script/OpenPF2Core.PF2PartyInterface"))
		const TSubclassOf<AActor> PartyType
	);

	/**
	 * Attempts to change the current play mode for all characters in the loaded level.
	 *
	 * The mode of play is only changed if the rule set for the current mode of play allows the transition. If the mode
	 * of play is changed, all player controllers are notified of the change in mode via game state replication. If the
	 * mode of play is not changed, nothing happens.
	 *
	 * @see EPF2ModeOfPlay
	 *
	 * @param NewModeOfPlay
	 *	The new play mode.
	 */
	void AttemptModeOfPlaySwitch(const EPF2ModeOfPlayType NewModeOfPlay);

	/**
	 * Immediately switches the current play mode for all characters in the loaded level.
	 *
	 * All player controllers are notified of the change in mode via game state replication.
	 *
	 * @param NewModeOfPlay
	 *	The mode of play to switch to.
	 */
	void ForceSwitchModeOfPlay(const EPF2ModeOfPlayType NewModeOfPlay);
};
