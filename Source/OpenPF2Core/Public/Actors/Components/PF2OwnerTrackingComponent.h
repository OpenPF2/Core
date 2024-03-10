// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#pragma once

#include <Components/ActorComponent.h>

#include <GameFramework/Controller.h>
#include <GameFramework/Info.h>

#include "PF2EventEmitterInterface.h"

#include "Actors/Components/PF2ActorComponentBase.h"
#include "Actors/Components/PF2OwnerTrackingInterface.h"

#include "PF2OwnerTrackingComponent.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2PlayerControllerInterface;
class IPF2PlayerStateInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UCLASS(ClassGroup="OpenPF2-Characters", meta=(BlueprintSpawnableComponent))
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2OwnerTrackingComponent :
	public UPF2ActorComponentBase,
	public IPF2EventEmitterInterface,
	public IPF2OwnerTrackingInterface
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
	mutable UPF2OwnerTrackingInterfaceEvents* Events;

private:
	// =================================================================================================================
	// Private Fields
	// =================================================================================================================
	/**
	 * The state of the player who owns the containing actor.
	 *
	 * This is a standard player state (instead of an interface) for replication. UE will not replicate actors if they
	 * are declared/referenced through an interface property. The value of this property MUST implement
	 * IPF2PlayerStateInterface.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_OwningPlayerState)
	APlayerState* OwningPlayerState;

	/**
	 * The party to which the containing actor is affiliated, if any.
	 *
	 * This is an info actor (instead of an interface) for replication. UE will not replicate actors if they are
	 * declared/referenced through an interface property. The value of this property MUST implement IPF2PartyInterface.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_Party)
	AInfo* Party;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2OwnerTrackingComponent.
	 */
	explicit UPF2OwnerTrackingComponent();

	// =================================================================================================================
	// Public Methods - UActorComponent Overrides
	// =================================================================================================================
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods - IPF2EventEmitterInterface Implementation
	// =================================================================================================================
	virtual UObject* GetGenericEventsObject() const override;

	// =================================================================================================================
	// Public Methods - IPF2OwnerTrackingInterface Implementation
	// =================================================================================================================
	virtual UPF2OwnerTrackingInterfaceEvents* GetEvents() const override;

	virtual TScriptInterface<IPF2PartyInterface> GetParty() const override;

	virtual void SetParty(const TScriptInterface<IPF2PartyInterface> NewParty) override;

	virtual TScriptInterface<IPF2PlayerStateInterface> GetStateOfOwningPlayer() const override;

	virtual void SetOwningPlayerByController(const TScriptInterface<IPF2PlayerControllerInterface> NewController) override;

	virtual void SetOwningPlayerByState(const TScriptInterface<IPF2PlayerStateInterface> NewPlayerState) override;

	virtual bool IsSamePartyAsActor(AActor* OtherActor) const override;

	virtual bool IsSamePartyAsPlayerWithController(
		const TScriptInterface<IPF2PlayerControllerInterface> OtherController
	) const override;

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
	// Protected Replication Callbacks
	// =================================================================================================================
	/**
	 * Notifies this component that the player state for the player who owns the containing actor has been replicated.
	 *
	 * @param OldOwner
	 *	The previous owner of the containing actor.
	 */
	UFUNCTION()
	void OnRep_OwningPlayerState(APlayerState* OldOwner);

	/**
	 * Notifies this component that the party to which the containing actor is affiliated has been replicated.
	 *
	 * @param OldParty
	 *	The previous party affiliation of the containing actor.
	 */
	UFUNCTION()
	void OnRep_Party(AInfo* OldParty);

	// =================================================================================================================
	// Protected Native Event Notifications
	// =================================================================================================================
	/**
	 * Notifies all event listeners that the player state that own the containing actor has changed.
	 *
	 * @param OldOwner
	 *	The player state corresponding to the player who was the previous owner of this actor. Can be null if there was
	 *	no prior owner.
	 * @param NewOwner
	 *	The player state corresponding to the player who is now the owner of this actor. Can be null if there is no
	 *	new owner (containing actor does not belong to anyone/is an orphan).
	 */
	void Native_OnOwningPlayerStateChanged(
		const TScriptInterface<IPF2PlayerStateInterface> OldOwner,
		const TScriptInterface<IPF2PlayerStateInterface> NewOwner
	);

	/**
	 * Notifies all event listeners that the party to which the containing actor is affiliated has changed.
	 *
	 * @param OldParty
	 *	The previous party affiliation, if any. Can be null if there was no prior party affiliation.
	 * @param NewParty
	 *	The new party affiliation, if any. Can be null if the containing actor should no longer be affiliated with a
	 *	party.
	 */
	void Native_OnPartyChanged(
		const TScriptInterface<IPF2PartyInterface> OldParty,
		const TScriptInterface<IPF2PartyInterface> NewParty
	);
};
