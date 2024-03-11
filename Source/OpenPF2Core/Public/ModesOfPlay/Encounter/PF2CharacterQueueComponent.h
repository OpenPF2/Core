// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Components/ActorComponent.h>

#include "PF2EventEmitterInterface.h"

#include "Actors/Components/PF2ActorComponentBase.h"

#include "ModesOfPlay/Encounter/PF2CharacterQueueInterface.h"

#include "PF2CharacterQueueComponent.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2PlayerControllerInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * A component for managing a ring buffer of characters.
 *
 * This is typically used by player controllers to manage what characters the player can control, and to advance through
 * them, similar to how BioWare games like Dragon Age allow you to assume control of different party members.
 */
UCLASS(ClassGroup="OpenPF2-PlayerControllers", meta=(BlueprintSpawnableComponent))
class OPENPF2CORE_API UPF2CharacterQueueComponent final :
	public UPF2ActorComponentBase,
	public IPF2EventEmitterInterface,
	public IPF2CharacterQueueInterface
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
	mutable UPF2CharacterQueueInterfaceEvents* Events;

	/**
	 * The character(s) in this queue.
	 *
	 * This is an array of actors (instead of interfaces) for replication. UE will not replicate actors if they are
	 * declared/referenced through an interface property.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_CharacterQueue)
	TArray<AActor*> Queue;

	/**
	 * The index of the character that is currently active/selected in this queue.
	 *
	 * This is often referred to as the "cursor" for this queue.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_ControlledCharacterIndex)
	uint8 ControlledCharacterIndex;

	/**
	 * A locally cached copy of which character is currently active.
	 *
	 * This is NOT replicated; instead, it is derived from the value of ControlledCharacterIndex, which is replicated, and
	 * gets updated by UpdateControlledCharacter().
	 *
	 * @see UpdateControlledCharacter()
	 */
	UPROPERTY()
	TScriptInterface<IPF2CharacterInterface> ControlledCharacter;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2CharacterQueueComponent.
	 */
	explicit UPF2CharacterQueueComponent();

	// =================================================================================================================
	// Public Methods - UActorComponent Overrides
	// =================================================================================================================
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods - IPF2EventEmitterInterface Implementation
	// =================================================================================================================
	virtual UObject* GetGenericEventsObject() const override;

	// =================================================================================================================
	// Public Methods - IPF2CharacterQueueInterface Implementation
	// =================================================================================================================
	virtual UPF2CharacterQueueInterfaceEvents* GetEvents() const override;

	virtual TScriptInterface<IPF2CharacterInterface> GetControlledCharacter() const override;

	virtual void Add(TScriptInterface<IPF2CharacterInterface> Character) override;

	virtual bool Remove(TScriptInterface<IPF2CharacterInterface> Character) override;

	virtual int Count() const override;

	virtual void Clear() override;

	virtual TScriptInterface<IPF2CharacterInterface> PreviousCharacter() override;

	virtual TScriptInterface<IPF2CharacterInterface> NextCharacter() override;

	virtual TArray<TScriptInterface<IPF2CharacterInterface>> ToArray() const override;

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
	 * Gets the highest zero-based index available within the character queue.
	 *
	 * In the special case in which the queue is empty, this returns "0" even though it is not a valid index, as this
	 * will be the index of the first character added to the queue once the character has been added.
	 *
	 * @return
	 *	- If the queue is not empty: The highest valid, zero-based index available in the character queue (for example,
	 *	  if there are 10 items in the queue, this will return a value of "9").
	 *	- If the queue is empty: a value of "0".
	 */
	FORCEINLINE uint8 GetMaxIndex() const
	{
		return FMath::Max(this->Count() - 1, 0);
	}

	/**
	 * Sets the active character index to the specified value, notifying listeners in the process.
	 *
	 * @param NewControlledCharacterIndex
	 *   The new value for the active character index.
	 */
	void SetControlledCharacterIndex(const uint8 NewControlledCharacterIndex);

	/**
	 * Updates the locally cached active character reference.
	 *
	 * If this actually results in a change to the reference, event listeners are notified.
	 */
	void UpdateControlledCharacter();

	// =================================================================================================================
	// Protected Replication Callbacks
	// =================================================================================================================
	/**
	 * Notifies this component that the list of character(s) in the queue has been replicated.
	 *
	 * @param OldCharacters
	 *	The previous array of characters in the queue.
	 */
	UFUNCTION()
	void OnRep_CharacterQueue(const TArray<AActor*>& OldCharacters);

	/**
	 * Notifies this component that which character is active has been replicated.
	 */
	UFUNCTION()
	void OnRep_ControlledCharacterIndex();

	// =================================================================================================================
	// Protected Native Event Callbacks
	// =================================================================================================================
	/**
	 * Callback invoked when characters in this queue have changed (characters added or removed, or queue cleared).
	 *
	 * If replication is enabled for this component, this is invoked on both the owning client and the server.
	 */
	void Native_OnCharactersChanged();

	/**
	 * Callback invoked when a character has been added to the queue.
	 *
	 * If replication is enabled for this component, this is invoked on both the owning client and the server.
	 *
	 * @param AddedCharacter
	 *	The character that is now in the queue.
	 */
	void Native_OnCharacterAdded(const TScriptInterface<IPF2CharacterInterface>& AddedCharacter);

	/**
	 * Callback invoked when a character has been removed from the queue.
	 *
	 * If replication is enabled for this component, this is invoked on both the owning client and the server.
	 *
	 * @param RemovedCharacter
	 *	The character that is no longer in the queue.
	 * @param RemovedIndex
	 *	The index of the character that was removed.
	 */
	void Native_OnCharacterRemoved(const TScriptInterface<IPF2CharacterInterface>& RemovedCharacter,
	                               const uint8                                     RemovedIndex);

	/**
	 * Callback invoked when there is a change to which character in the queue is active.
	 *
	 * If replication is enabled for this component, this is invoked on both the owning client and the server.
	 *
	 * @param OldCharacter
	 *	The character that was previously active in the queue, if any.
	 * @param NewCharacter
	 *	The character that is now active in the queue.
	 */
	void Native_OnControlledCharacterChanged(const TScriptInterface<IPF2CharacterInterface>& OldCharacter,
	                                         const TScriptInterface<IPF2CharacterInterface>& NewCharacter);
};
