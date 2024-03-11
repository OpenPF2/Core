// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#pragma once

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2PartyInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2PlayerControllerInterface;
class IPF2PlayerStateInterface;

// =====================================================================================================================
// Normal Declarations - Delegates
// =====================================================================================================================
/**
 * Delegate for Blueprints to react to a change in player membership.
 *
 * @param Party
 *	The party broadcasting this event.
 * @param PlayerState
 *	The player state of the player who was added to or removed from the party.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPF2PartyMemberAddedOrRemovedDelegate,
	const TScriptInterface<IPF2PartyInterface>&,       Party,
	const TScriptInterface<IPF2PlayerStateInterface>&, PlayerState
);

/**
 * Delegate for Blueprints to react to queue membership changing (players added or removed, or party cleared).
 *
 * @param Party
 *	The party broadcasting this event.
 * @param PlayerStates
 *	An array containing the player state for each player who is currently a member of the party.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPF2PartyMembersChangedDelegate,
	const TScriptInterface<IPF2PartyInterface>&,               Party,
	const TArray<TScriptInterface<IPF2PlayerStateInterface>>&, PlayerStates
);

// =====================================================================================================================
// Normal Declarations - Types
// =====================================================================================================================
/**
 * The "Events" object for PF2PartyInterface.
 *
 * This is a concrete UObject that contains only the dynamic multicast delegates that instances of the interface expose
 * to consumers for binding.
 *
 * @see IPF2EventEmitterInterface
 */
UCLASS()
class OPENPF2CORE_API UPF2PartyInterfaceEvents : public UObject
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Fields - Multicast Delegates
	// =================================================================================================================
	/**
	 * Event fired when the commands in the queue have changed (commands added, commands removed, or queue cleared).
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Parties")
	FPF2PartyMembersChangedDelegate OnPartyMembersChanged;

	/**
	 * Event fired when a player is added to this party.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Parties")
	FPF2PartyMemberAddedOrRemovedDelegate OnPlayerAdded;

	/**
	 * Event fired when a player is removed from this party.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Parties")
	FPF2PartyMemberAddedOrRemovedDelegate OnPlayerRemoved;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2PartyInterface : public UPF2LogIdentifiableInterface
{
	GENERATED_BODY()
};

/**
 * An interface for groups or "parties" that one or more players and characters can belong to in OpenPF2.
 *
 * Each player of the game is affiliated with a single party. All of the characters that the player can control are
 * affiliated with the same party, but not all of the *characters* affiliated with a party are necessarily controllable
 * by all *players* in the same party. For example, in a multiplayer RPG, two players may be in the same party but only
 * have the ability to control their own characters within the party (e.g., a party of 4 in which each player can only
 * control 2 characters each).
 */
class OPENPF2CORE_API IPF2PartyInterface : public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Constants
	// =================================================================================================================
	/**
	 * Special constant value that signifies that a character does not belong to any player.
	 */
	static const int32 PartyIndexNone;

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets the events object used for binding Blueprint callbacks to events from this component.
	 *
	 * @return
	 *	The events object for this interface.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual UPF2PartyInterfaceEvents* GetEvents() const = 0;

	/**
	 * Gets the player-readable name of this party.
	 *
	 * @return
	 *	The name of this party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual FText GetPartyName() const = 0;

	/**
	 * Gets the index of this party.
	 *
	 * @return
	 *	The zero-based numeric index of this party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual int32 GetPartyIndex() const = 0;

	/**
	 * Sets the index of this party.
	 *
	 * This should only be invoked by the game mode.
	 *
	 * @param NewPartyIndex
	 *	The new zero-based numeric index of this party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual void SetPartyIndex(int32 NewPartyIndex) = 0;

	/**
	 * Gets player controllers for all players belonging to this party.
	 *
	 * - On the server: This will contain all player controllers affiliated with this party.
	 * - On clients: This can only return the player controller for the active client, since player controllers from
	 *   other clients are not replicated. If the current player is not a member of this party, the result will be an
	 *   empty array even if there are other players affiliated with this party.
	 *
	 * @return
	 *	An array of interfaces for each of the controllers for players affiliated with this party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual TArray<TScriptInterface<IPF2PlayerControllerInterface>> GetMemberControllers() const = 0;

	/**
	 * Gets player state for all players belonging to this party.
	 *
	 * @return
	 *	An array of interfaces for each of the player states of players affiliated with this party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual TArray<TScriptInterface<IPF2PlayerStateInterface>> GetMemberStates() const = 0;

	/**
	 * Gets the characters belonging to this party.
	 *
	 * @return
	 *	An array of interfaces for each of the characters affiliated with this party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetMemberCharacters() const = 0;

	/**
	 * Gets the bounds and absolute world position of a bounding box encompassing all party members.
	 *
	 * @param CenterPoint
	 *   A reference to the point at the center of the party.
	 * @param BoxExtent
	 *   The bounds of an imaginary box that encompasses the party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual void GetBounds(FVector& CenterPoint, FVector& BoxExtent) = 0;

	/**
	 * Adds the player having the specified player controller to this party.
	 *
	 * @param Controller
	 *	The controller for the player to add to the party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual void AddPlayerToPartyByController(const TScriptInterface<IPF2PlayerControllerInterface>& Controller) = 0;

	/**
	 * Adds the player having the specified player state to this party.
	 *
	 * @param PlayerState
	 *	The state for the player to add to the party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual void AddPlayerToPartyByState(const TScriptInterface<IPF2PlayerStateInterface>& PlayerState) = 0;

	/**
	 * Removes the player having the specified player controller from this party.
	 *
	 * @param Controller
	 *	The controller for the player to remove from the party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual void RemovePlayerFromPartyByController(const TScriptInterface<IPF2PlayerControllerInterface>& Controller) = 0;

	/**
	 * Removes the player having the specified player state from this party.
	 *
	 * @param PlayerState
	 *	The state for the player to remove from the party.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Parties")
	virtual void RemovePlayerFromPartyByState(const TScriptInterface<IPF2PlayerStateInterface>& PlayerState) = 0;
};
