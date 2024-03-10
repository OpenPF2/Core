// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "Actors/Components/PF2ActorComponentInterface.h"

#include "Commands/PF2CommandExecuteImmediatelyResult.h"

#include "PF2CommandQueueInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterCommandInterface;

// =====================================================================================================================
// Normal Declarations - Delegates
// =====================================================================================================================
/**
 * Delegate for Blueprints to react to the queue changing (e.g., commands added or removed, or queue cleared).
 *
 * @param CommandQueueComponent
 *	The component broadcasting this event.
 * @param Command
 *	The command that was added or removed.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPF2CommandAddedToOrRemovedFromQueueDelegate,
	const TScriptInterface<IPF2CommandQueueInterface> &,    CommandQueueComponent,
	const TScriptInterface<IPF2CharacterCommandInterface>&, Command
);

/**
 * Delegate for Blueprints to react to the queue contents changing (e.g., commands added or removed, or queue cleared).
 *
 * @param CommandQueueComponent
 *	The component broadcasting this event.
 * @param Commands
 *	An updated copy of the command queue after the change has occurred.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPF2CommandQueueChangedDelegate,
	const TScriptInterface<IPF2CommandQueueInterface>&,             CommandQueueComponent,
	const TArray<TScriptInterface<IPF2CharacterCommandInterface>>&, Commands
);

// =====================================================================================================================
// Normal Declarations - Types
// =====================================================================================================================
/**
 * The "Events" object for PF2CommandQueueInterface.
 *
 * This is a concrete UObject that contains only the dynamic multicast delegates that instances of the interface expose
 * to consumers for binding.
 *
 * @see IPF2EventEmitterInterface
 */
UCLASS()
class OPENPF2CORE_API UPF2CommandQueueInterfaceEvents : public UObject
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Fields - Multicast Delegates
	// =================================================================================================================
	/**
	 * Event fired when the commands in the queue have changed (commands added, commands removed, or queue cleared).
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Command Queues")
	FPF2CommandQueueChangedDelegate OnCommandsChanged;

	/**
	 * Event fired when a command has been added to this queue.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Command Queues")
	FPF2CommandAddedToOrRemovedFromQueueDelegate OnCommandAdded;

	/**
	 * Event fired when a command has been removed from this queue.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Command Queues")
	FPF2CommandAddedToOrRemovedFromQueueDelegate OnCommandRemoved;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2CommandQueueInterface : public UPF2ActorComponentInterface
{
	GENERATED_BODY()
};

/**
 * An interface for objects that maintain queues of commands/actions.
 */
class OPENPF2CORE_API IPF2CommandQueueInterface : public IPF2ActorComponentInterface
{
	GENERATED_BODY()

public:
	/**
	 * Gets the events object used for binding Blueprint callbacks to events from this component.
	 *
	 * @return
	 *	The events object for this interface.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual UPF2CommandQueueInterfaceEvents* GetEvents() const = 0;

	/**
	 * Adds a command to the end of the queue.
	 *
	 * If the queue has a size limit and the queue is full, the command will not be enqueued.
	 *
	 * @param Command
	 *	The command to attempt to add to the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual void Enqueue(const TScriptInterface<IPF2CharacterCommandInterface>& Command) = 0;

	/**
	 * Adds a command at the specified position in the queue.
	 *
	 * The Position cannot be negative, but may be equal to the size of the queue (even zero). If equal to the size of
	 * the queue, the queue will grow to accommodate the additional item.
	 *
	 * If the queue has a size limit and the queue is full, the command will be enqueued and then the last command in
	 * the queue will be dropped to ensure that the queue does not grow beyond the limit. This means that if the command
	 * being enqueued is placed at the end of a full queue, it will get dropped immediately after it has been enqueued.
	 *
	 * @param Command
	 *	The command to attempt to add to the queue.
	 * @param Position
	 *	The position at which the command will be added within the queue. This cannot be negative and must be no larger
	 *	than the size of the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual void EnqueueAt(const TScriptInterface<IPF2CharacterCommandInterface>& Command,
	                       const int32                                            Position) = 0;

	/**
	 * Returns the next command (if there is one) in the queue.
	 *
	 * The command is not removed from the queue.
	 *
	 * @param NextCommand
	 *	The output: either the next command in the queue; or an invalid reference if there are no commands in the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual void PeekNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) = 0;

	/**
	 * Removes and returns the first/oldest command (if there is one) from the queue.
	 *
	 * The command is removed from the queue.
	 *
	 * @param NextCommand
	 *	The output: either the next command in the queue; or an invalid reference if there are no commands in the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual void PopNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) = 0;

	/**
	 * Removes the first/oldest command (if there is one) from the queue and drops it.
	 *
	 * The command is removed from the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual void DropNext() = 0;

	/**
	 * Removes and executes the first/oldest command (if there is one) from the queue.
	 *
	 * The command is removed from the queue.
	 *
	 * @return
	 *	- EPF2CommandExecuteImmediatelyResult::None if there are no commands to execute.
	 *	- EPF2CommandExecuteImmediatelyResult::Activated if there was a queued command that was executed.
	 *	- EPF2CommandExecuteImmediatelyResult::Blocked if there was a queued command but it could not be executed yet,
	 *	  typically because it is blocked by another ability that is active on the character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual EPF2CommandExecuteImmediatelyResult PopAndExecuteNext() = 0;

	/**
	 * Removes a command from the queue.
	 *
	 * If the command is not in the queue, nothing happens (this method is idempotent).
	 *
	 * @param Command
	 *	The command to remove, if it exists in the queue.
	 *
	 * @return
	 *	- true if the command was removed from the queue.
	 *	- false if the command was not in the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual bool Remove(const TScriptInterface<IPF2CharacterCommandInterface>& Command) = 0;

	/**
	 * Gets the count of how many commands are in the queue.
	 *
	 * @return
	 *	How many items are in the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual int Count() = 0;

	/**
	 * Removes all commands from the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual void Clear() = 0;

	/**
	 * Converts the contents of this queue into an array of commands.
	 *
	 * Changes to the resulting array have no effect on the contents of this queue.
	 *
	 * @return
	 *	The array of commands contained in this queue, in the order they appear in the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Command Queues")
	virtual TArray<TScriptInterface<IPF2CharacterCommandInterface>> ToArray() const = 0;
};
