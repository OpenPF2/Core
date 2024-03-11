// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#pragma once

#include <GameFramework/GameStateBase.h>

#include <UObject/ScriptInterface.h>

#include "PF2GameStateInterface.h"

#include "ModesOfPlay/PF2ModeOfPlayRuleSetInterface.h"
#include "ModesOfPlay/PF2ModeOfPlayType.h"

#include "PF2GameStateBase.generated.h"

/**
 * Default base class for OpenPF2 Game States.
 *
 * @see IPF2GameStateInterface
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2GameStateBase : public AGameStateBase, public IPF2GameStateInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The next index to assign to a player who joins the game.
	 */
	int32 NextPlayerIndex;

	/**
	 * The next index to assign to a new party in the game.
	 */
	int32 NextPartyIndex;

	// =================================================================================================================
	// Protected Fields - Blueprint Accessible
	// =================================================================================================================
	/**
	 * The current play mode for all characters in the loaded level.
	 *
	 * @see IPF2GameStateInterface::GetModeOfPlay
	 */
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category=GameState, ReplicatedUsing=OnRep_ModeOfPlay)
	EPF2ModeOfPlayType ModeOfPlay;

	/**
	 * The set of rules that govern how the game behaves in the current play mode.
	 *
	 * @see IPF2ModeOfPlayRuleSet
	 */
	UPROPERTY(Transient, BlueprintReadOnly, Replicated, Category=GameState)
	TScriptInterface<IPF2ModeOfPlayRuleSetInterface> ModeOfPlayRuleSet;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for APF2GameStateBase.
	 */
	explicit APF2GameStateBase();

	// =================================================================================================================
	// Public Methods - AGameStateBase Implementation
	// =================================================================================================================
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods - IPF2GameStateInterface Implementation
	// =================================================================================================================
	virtual int32 GeneratePlayerIndex() override
	{
		return this->NextPlayerIndex++;
	}

	virtual int32 GeneratePartyIndex() override
	{
		return this->NextPartyIndex++;
	}

	virtual EPF2ModeOfPlayType GetModeOfPlay() const override
	{
		return this->ModeOfPlay;
	}

	virtual TScriptInterface<IPF2ModeOfPlayRuleSetInterface> GetModeOfPlayRuleSet() const override
	{
		return this->ModeOfPlayRuleSet;
	}

	virtual void SetModeOfPlay(const EPF2ModeOfPlayType                         NewMode,
	                           TScriptInterface<IPF2ModeOfPlayRuleSetInterface> NewRuleSet) override;

protected:
	// =================================================================================================================
	// Protected Replication Callbacks
	// =================================================================================================================
	/**
	 * Replication callback for the "Mode of Play" property.
	 *
	 * This notifies all player controllers that the play mode has changed (e.g., to change HUD display or background
	 * music to suit the new play mode).
	 */
	UFUNCTION()
	virtual void OnRep_ModeOfPlay();

	// =================================================================================================================
	// Protected Native Event Callbacks
	// =================================================================================================================
	/**
	 * Notifies this copy of the game state that the mode of play has been updated by the server.
	 *
	 * - On listen servers: this is invoked AFTER this change has been replicated to clients.
	 * - On clients: this is invoked AFTER the change has been replicated from the server.
	 *
	 * It has no effect on dedicated servers.
	 */
	virtual void Native_OnModeOfPlayAvailable();
};
