// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Engine/Texture2D.h>

#include "PF2CommandQueuePosition.h"

#include "Commands/PF2CommandExecuteImmediatelyResult.h"
#include "Commands/PF2CommandExecuteOrQueueResult.h"

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2CharacterCommandInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2CharacterCommandInterface : public UPF2LogIdentifiableInterface
{
	GENERATED_BODY()
};

/**
 * An interface for commands that can either be executed immediately, or defer their execution to a future time.
 */
class OPENPF2CORE_API IPF2CharacterCommandInterface : public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Instance Methods
	// =================================================================================================================
	/**
	 * Gets the character on which this command would be executed.
	 *
	 * @return
	 *	The character that is the target of this command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual TScriptInterface<IPF2CharacterInterface> GetOwningCharacter() const = 0;

	/**
	 * Gets an icon to represent this command, for whenever it is displayed to players/users.
	 *
	 * @return
	 *	A graphical representation of this command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual UTexture2D* GetCommandIcon() const = 0;

	/**
	 * Gets the name of this command, for whenever it is displayed to players/users.
	 *
	 * @return
	 *	A short textual description of this command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual FText GetCommandLabel() const = 0;

	/**
	 * Gets the description of this command, for whenever it is displayed to players/users.
	 *
	 * @return
	 *	A long textual description of this command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual FText GetCommandDescription() const = 0;

	/**
	 * Gets the preference for where in a command queue this command should be placed, if this command gets queued.
	 *
	 * @return
	 *	The queue position preference.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual EPF2CommandQueuePosition GetQueuePositionPreference() const = 0;

	/**
	 * Attempt to execute this command immediately, if possible; queue it for the character, if not possible.
	 *
	 * This method only has an effect on the server. If it is called on a client, the result will be
	 * EPF2CommandExecuteOrQueueResult::None. In rare circumstances, it is possible that the command is neither queued
	 * nor executed, in which case the result will be EPF2CommandExecuteOrQueueResult::Refused.
	 *
	 * @return
	 *	The outcome of attempting to execute the command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual EPF2CommandExecuteOrQueueResult AttemptExecuteOrQueue() = 0;

	/**
	 * Attempt to execute this command immediately, if possible; do nothing, if not possible.
	 *
	 * This method only has an effect on the server. If it is called on a client, the result will be
	 * EPF2CommandExecuteImmediatelyResult::None.
	 *
	 * @return
	 *	The outcome of attempting to execute the command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual EPF2CommandExecuteImmediatelyResult AttemptExecuteImmediately() = 0;

	/**
	 * Attempt to queue this command without trying to execute it first, if possible; do nothing, if not possible.
	 *
	 * This method only has an effect on the server. If it is called on a client, the result will be false.
	 *
	 * Whether this command actually gets queued, and where within the active character's queue this command gets
	 * queued, is subject to the Mode of Play Rule Set (MoPRS), which ultimately has the final say:
	 * - If the MoPRS is not enforcing queueing of commands, this command may get dropped.
	 * - If the MoPRS is enforcing queueing:
	 *   - This command may get queued at the end of the active character's queue even if this command has a preference
	 *     for a different place in the queue.
	 *   - This command may get dropped if the active character's queue has a size limit and the queue is full.
	 *
	 * @return
	 *	- true if the command was able to be queued.
	 *	- false if the command could not be queued.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual UPARAM(DisplayName="Was Queued") bool AttemptQueue() = 0;

	/**
	 * Request to cancel this command.
	 *
	 * The actual choice of whether the command is cancelled is deferred to the game mode and active Mode of Play Rule
	 * Set. This can be invoked on either the client or server and in either case, it will be executed on the server.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual void AttemptCancel() = 0;

	/**
	 * Gets the actor that is implementing this interface.
	 *
	 * @return
	 *	This character, as an actor.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual AInfo* ToActor() = 0;
};
