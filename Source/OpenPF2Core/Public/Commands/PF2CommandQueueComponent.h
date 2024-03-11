// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Components/ActorComponent.h>

#include <GameFramework/Info.h>

#include "PF2EventEmitterInterface.h"

#include "Actors/Components/PF2ActorComponentBase.h"

#include "Commands/PF2CharacterCommandInterface.h"
#include "Commands/PF2CommandQueueInterface.h"

#include "PF2CommandQueueComponent.generated.h"

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * A component for managing commands/actions that have been queued for a particular character.
 *
 * (The inspiration for this approach -- of wrapping Gameplay Abilities in a command that can be invoked at any time --
 * came from "ue4-orders-abilities" by Daedalic Entertainment,
 * https://github.com/DaedalicEntertainment/ue4-orders-abilities).
 */
UCLASS(ClassGroup="OpenPF2-Characters", meta=(BlueprintSpawnableComponent))
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2CommandQueueComponent :
	public UPF2ActorComponentBase,
	public IPF2EventEmitterInterface,
	public IPF2CommandQueueInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Constants
	// =================================================================================================================
	/**
	 * Special constant value that signifies that there is no limit to the number of commands that can be enqueued.
	 */
	static const uint8 CommandLimitNone;

protected:
	/**
	 * The events object used for binding Blueprint callbacks to events from this component.
	 */
	UPROPERTY(Transient)
	mutable UPF2CommandQueueInterfaceEvents* Events;

	/**
	 * The queue of commands for the owning character.
	 *
	 * This is an array of actors (instead of interfaces) for replication. UE will not replicate actors if they are
	 * declared/referenced through an interface property.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_Queue)
	TArray<AInfo*> Queue;

	/**
	 * The maximum number of commands that can be in the queue at one time.
	 *
	 * Once the queue reaches this size, no additional commands will be accepted until at least one command already in
	 * the queue has been executed or removed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="OpenPF2 - Command Queue")
	uint8 SizeLimit;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2CommandQueueComponent.
	 */
	explicit UPF2CommandQueueComponent();

	// =================================================================================================================
	// Public Methods - AActorComponent Overrides
	// =================================================================================================================
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods - IPF2EventEmitterInterface Implementation
	// =================================================================================================================
	virtual UObject* GetGenericEventsObject() const override;

	// =================================================================================================================
	// Public Methods - IPF2CommandQueueInterface Implementation
	// =================================================================================================================
	virtual UPF2CommandQueueInterfaceEvents* GetEvents() const override;

	virtual void Enqueue(const TScriptInterface<IPF2CharacterCommandInterface>& Command) override;

	virtual void EnqueueAt(const TScriptInterface<IPF2CharacterCommandInterface>& Command,
	                       const int32                                            Position) override;

	virtual void PeekNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) override;

	virtual void PopNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) override;

	virtual void DropNext() override;

	virtual EPF2CommandExecuteImmediatelyResult PopAndExecuteNext() override;

	virtual bool Remove(const TScriptInterface<IPF2CharacterCommandInterface>& Command) override;

	virtual int Count() override;

	virtual void Clear() override;

	virtual TArray<TScriptInterface<IPF2CharacterCommandInterface>> ToArray() const override;

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
	 * Replication callback for the "Queue" property.
	 *
	 * This notifies all event listeners that the contents of the queue has changed, as well as separately notifying
	 * listeners about each command that was added or removed.
	 */
	UFUNCTION()
	virtual void OnRep_Queue(const TArray<AInfo*>& OldQueue);

	// =================================================================================================================
	// Protected Event Notifications
	// =================================================================================================================
	/**
	 * Callback invoked when commands in this queue have changed (commands added, commands removed, or queue cleared).
	 */
	void Native_OnCommandsChanged();

	/**
	 * Callback invoked when a command has been added to this queue.
	 *
	 * @param CommandAdded
	 *	The command that was added.
	 */
	void Native_OnCommandAdded(const TScriptInterface<IPF2CharacterCommandInterface>& CommandAdded);

	/**
	 * Callback invoked when a command has been removed from this queue.
	 *
	 * @param CommandRemoved
	 *	The command that was removed.
	 */
	void Native_OnCommandRemoved(const TScriptInterface<IPF2CharacterCommandInterface>& CommandRemoved);
};
