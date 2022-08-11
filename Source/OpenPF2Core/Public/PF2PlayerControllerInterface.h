﻿// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/Interface.h>

#include "GameModes/PF2ModeOfPlayType.h"

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2PlayerControllerInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to break recursive dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2PlayerStateInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2PlayerControllerInterface : public UPF2LogIdentifiableInterface
{
	GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible Player Controllers.
 */
class OPENPF2CORE_API IPF2PlayerControllerInterface : public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets the player state of this player controller.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Player Controllers")
	virtual TScriptInterface<IPF2PlayerStateInterface> GetPlayerState() const = 0;

	/**
	 * Gets the player controller that is implementing this interface.
	 *
	 * @return
	 *	This player controller, as a player controller actor.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Player Controllers")
	virtual APlayerController* ToPlayerController() = 0;

	// =================================================================================================================
	// Public Event Notifications from the Game State
	// =================================================================================================================
	/**
	 * Notifies this player controller that the mode of play has changed.
	 *
	 * (This should normally be invoked only by the game state).
	 *
	 * @see EPF2ModeOfPlay
	 *
	 * @param NewMode
	 *	The new mode of play.
	 */
	UFUNCTION()
	virtual void HandleModeOfPlayChanged(EPF2ModeOfPlayType NewMode) = 0;

	// =================================================================================================================
	// Public Event Notifications from Mode of Play Rule Sets (MoPRS)
	// =================================================================================================================
	/**
	 * Notifies this player controller that the pawn's turn during an encounter has started.
	 *
	 * (This should normally be invoked only by the MoPRS).
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleEncounterTurnStarted() = 0;

	/**
	 *  Notifies this player controller that the pawn's turn during an encounter has ended.
	 *
	 * (This should normally be invoked only by the MoPRS).
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleEncounterTurnEnded() = 0;
};
