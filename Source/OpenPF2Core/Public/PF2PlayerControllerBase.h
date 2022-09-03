﻿// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#pragma once

#include <GameFramework/PlayerController.h>

#include "PF2PlayerControllerInterface.h"

#include "PF2PlayerControllerBase.generated.h"

// =====================================================================================================================
// Forward Declarations (to break recursive dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2CharacterCommandInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Default base class for PF2 Player Controllers.
 *
 * @see IPF2PlayerControllerInterface
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2PlayerControllerBase : public APlayerController, public IPF2PlayerControllerInterface
{
	GENERATED_BODY()

	// =================================================================================================================
	// Private Properties
	// =================================================================================================================
	/**
	 * The character(s) that can be controlled by this player.
	 *
	 * The characters in this list must be affiliated with the same party as this player.
	 *
	 * This is an array of actors (instead of interfaces) for replication. UE will not replicate actors if they are
	 * declared/referenced through an interface property.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_ControllableCharacters)
	TArray<AActor*> ControllableCharacters;

public:
	// =================================================================================================================
	// Public Methods - AController Overrides
	// =================================================================================================================
	virtual void InitPlayerState() override;

	virtual void OnRep_PlayerState() override;

	// =================================================================================================================
	// Public Methods - APlayerController Overrides
	// =================================================================================================================
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetPawn(APawn* NewPawn) override;

	// =================================================================================================================
	// Public Methods - IPF2PlayerControllerInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual TScriptInterface<IPF2PlayerStateInterface> GetPlayerState() const override;

	UFUNCTION(BlueprintCallable)
	virtual APlayerController* ToPlayerController() override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetControllableCharacters() const override;

	virtual void Native_OnModeOfPlayChanged(EPF2ModeOfPlayType NewMode) override;

	UFUNCTION(BlueprintCallable)
	virtual void GiveCharacter(const TScriptInterface<IPF2CharacterInterface>& GivenCharacter) override;

	UFUNCTION(BlueprintCallable)
	virtual void ReleaseCharacter(const TScriptInterface<IPF2CharacterInterface>& ReleasedCharacter) override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_OnEncounterTurnStarted() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_OnEncounterTurnEnded() override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Overrides
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual FString GetIdForLogs() const override;

protected:
	// =================================================================================================================
	// Protected Replication Callbacks
	// =================================================================================================================
	/**
	 * Notifies this component that the list of character(s) that can be controlled by the player has been replicated.
	 *
	 * @param OldCharacters
	 *	The previous array of characters that the player could control.
	 */
	UFUNCTION()
	void OnRep_ControllableCharacters(const TArray<AActor*> OldCharacters);

	// =================================================================================================================
	// Protected Native Event Callbacks
	// =================================================================================================================
	/*
	 * Callback invoked in C++ code when the player state is available for use.
	 *
	 * - On the server: This happens just after the player state has been initialized.
	 * - On clients: This happens just after the player state has been replicated.
	 *
	 * @param NewPlayerState
	 *	The player state that was just made available.
	 */
	virtual void Native_OnPlayerStateAvailable(TScriptInterface<IPF2PlayerStateInterface> NewPlayerState);

	/**
	 * Callback invoked in C++ code when the player has taken ownership of a character.
	 *
	 * @param GivenCharacter
	 *	The character that is now controllable by the player.
	 */
	virtual void Native_OnCharacterGiven(const TScriptInterface<IPF2CharacterInterface>& GivenCharacter);

	/**
	 * Callback invoked in C++ code when the player has been released of ownership of a character.
	 *
	 * @param ReleasedCharacter
	 *	The character that is no longer controllable by the player.
	 */
	virtual void Native_OnCharacterReleased(const TScriptInterface<IPF2CharacterInterface>& ReleasedCharacter);

	// =================================================================================================================
	// Blueprint Implementable Events
	// =================================================================================================================
	/**
	 * Blueprint event callback invoked in Blueprint when the player state is available for use.
	 *
	 * This is invoked on both the owning client and the server:
	 * - On the server: This happens just after the player state has been initialized.
	 * - On clients: This happens just after the player state has been replicated.
	 *
	 * @param NewPlayerState
	 *	The player state that was just made available.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Player State Available")
	)
	void BP_OnPlayerStateAvailable(const TScriptInterface<IPF2PlayerStateInterface>& NewPlayerState);

	/**
	 * Blueprint event callback invoked when the player has taken ownership of a character.
	 *
	 * This is invoked on both the owning client and the server.
	 *
	 * @param GivenCharacter
	 *	The character that is now controllable by the player.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Character Given")
	)
	void BP_OnCharacterGiven(const TScriptInterface<IPF2CharacterInterface>& GivenCharacter);

	/**
	 * Blueprint event invoke callback when the player has been released from ownership of a character.
	 *
	 * This is invoked on both the owning client and the server.
	 *
	 * @param ReleasedCharacter
	 *	The character that is no longer controllable by the player.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Character Released")
	)
	void BP_OnCharacterReleased(const TScriptInterface<IPF2CharacterInterface>& ReleasedCharacter);

	/**
	 * BP event invoked when the mode of play has changed.
	 *
	 * This is invoked on both the owning client and server.
	 *
	 * @param NewMode
	 *	The new mode of play.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Mode of Play Changed")
	)
	void BP_OnModeOfPlayChanged(EPF2ModeOfPlayType NewMode);

	/**
	 * BP event invoked when the pawn's turn during an encounter has started.
	 *
	 * This is invoked on both the owning client and server.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Encounter Turn Started")
	)
	void BP_OnEncounterTurnStarted();

	/**
	 * BP event invoked when the pawn's turn during an encounter has ended.
	 *
	 * This is invoked on both the owning client and server.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Encounter Turn Ended")
	)
	void BP_OnEncounterTurnEnded();
};
