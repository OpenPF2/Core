// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#pragma once

#include <GameFramework/Info.h>

#include "PF2EventEmitterInterface.h"
#include "PF2PartyInterface.h"

#include "PF2Party.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2PlayerStateInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Default implementation of an OpenPF2-compatible party.
 */
UCLASS(BlueprintType, Blueprintable)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2Party : public AInfo, public IPF2EventEmitterInterface, public IPF2PartyInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The events object used for binding Blueprint callbacks to events from this component.
	 */
	UPROPERTY(Transient)
	mutable UPF2PartyInterfaceEvents* Events;

	/**
	 * The player-readable name of this party.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="OpenPF2 - Party")
	FText PartyName;

	/**
	 * The index of this party.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="OpenPF2 - Party")
	int32 PartyIndex;

	/**
	 * Player state for all players belonging to this party.
	 *
	 * This is an array of player state actors (instead of interfaces) for replication. UE will not replicate actors if
	 * they are declared/referenced through an interface property.
	 */
	UPROPERTY(Replicated)
	TArray<APlayerState*> MemberStates;

	/**
	 * The characters belonging to this party.
	 *
	 * This is an array of character actors (instead of interfaces) for replication. UE will not replicate actors if
	 * they are declared/referenced through an interface property.
	 */
	UPROPERTY(Replicated)
	TArray<AActor*> MemberCharacters;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for APF2Party.
	 */
	explicit APF2Party();

	// =================================================================================================================
	// Public Methods - AActor Overrides
	// =================================================================================================================
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods - IPF2EventEmitterInterface Implementation
	// =================================================================================================================
	virtual UObject* GetGenericEventsObject() const override;

	// =================================================================================================================
	// Public Methods - IPF2PartyInterface Implementation
	// =================================================================================================================
	virtual UPF2PartyInterfaceEvents* GetEvents() const override;
	virtual FText GetPartyName() const override;
	virtual int32 GetPartyIndex() const override;
	virtual void SetPartyIndex(int32 NewPartyIndex) override;
	virtual TArray<TScriptInterface<IPF2PlayerControllerInterface>> GetMemberControllers() const override;
	virtual TArray<TScriptInterface<IPF2PlayerStateInterface>> GetMemberStates() const override;
	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetMemberCharacters() const override;
	virtual void GetBounds(FVector& CenterPoint, FVector& BoxExtent) override;
	virtual void AddPlayerToPartyByController(const TScriptInterface<IPF2PlayerControllerInterface>& Controller) override;
	virtual void AddPlayerToPartyByState(const TScriptInterface<IPF2PlayerStateInterface>& PlayerState) override;
	virtual void RemovePlayerFromPartyByController(const TScriptInterface<IPF2PlayerControllerInterface>& Controller) override;
	virtual void RemovePlayerFromPartyByState(const TScriptInterface<IPF2PlayerStateInterface>& PlayerState) override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	virtual FString GetIdForLogs() const override;

protected:
	// =================================================================================================================
	// Protected Native Event Callbacks
	// =================================================================================================================
	/**
	 * Notifies this party that a player has been added to this party.
	 *
	 * @param PlayerState
	 *	The state of the player that corresponds to the player that was added to the party.
	 */
	void Native_OnPlayerAdded(const TScriptInterface<IPF2PlayerStateInterface>& PlayerState);

	/**
	 * Notifies this party that a player has been removed from this party.
	 *
	 * @param PlayerState
	 *	The state of the player that corresponds to the player that was removed from this party.
	 */
	void Native_OnPlayerRemoved(const TScriptInterface<IPF2PlayerStateInterface>& PlayerState);

	/**
	 * Notifies this party that players have been added to or removed from this party.
	 */
	void Native_OnMembersChanged();

	// =================================================================================================================
	// Blueprint Implementable Events
	// =================================================================================================================
	/**
	 * BP event invoked when a player has been added to this party.
	 *
	 * @param PlayerState
	 *	The state of the player that corresponds to the player that was added to this party.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Parties",
		meta=(DisplayName="On Player Added")
	)
	void BP_OnPlayerAdded(const TScriptInterface<IPF2PlayerStateInterface>& PlayerState);

	/**
	 * BP event invoked when a player has been removed from this party.
	 *
	 * @param PlayerState
	 *	The state of the player that corresponds to the player that was removed from this party.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Parties",
		meta=(DisplayName="On Player Removed")
	)
	void BP_OnPlayerRemoved(const TScriptInterface<IPF2PlayerStateInterface>& PlayerState);
};
