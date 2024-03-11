// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Commands/PF2CommandQueueComponent.h"

#include <Net/UnrealNetwork.h>

#include <UObject/WeakInterfacePtr.h>

#include "Commands/PF2CharacterCommandInterface.h"

#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

const uint8 UPF2CommandQueueComponent::CommandLimitNone = 0;

UPF2CommandQueueComponent::UPF2CommandQueueComponent(): Events(nullptr), SizeLimit(CommandLimitNone)
{
	this->SetIsReplicatedByDefault(true);
}

void UPF2CommandQueueComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPF2CommandQueueComponent, Queue);
}

UObject* UPF2CommandQueueComponent::GetGenericEventsObject() const
{
	return this->GetEvents();
}

UPF2CommandQueueInterfaceEvents* UPF2CommandQueueComponent::GetEvents() const
{
	if (this->Events == nullptr)
	{
		// BUGBUG: This has to be instantiated here rather than via CreateDefaultSubobject() in the constructor, or it
		// breaks multiplayer. It seems that when created in the constructor, all instances of this component end up
		// sharing one events object leading to all players receiving the event whenever a multicast event is broadcast.
		// This typically results in a crash since the addresses of callbacks aren't valid on all clients.
		this->Events = NewObject<UPF2CommandQueueInterfaceEvents>(
			const_cast<UPF2CommandQueueComponent*>(this),
			FName(TEXT("InterfaceEvents"))
		);
	}

	return this->Events;
}

void UPF2CommandQueueComponent::Enqueue(const TScriptInterface<IPF2CharacterCommandInterface>& Command)
{
	AInfo* CommandActor = Command->ToActor();

	if ((this->SizeLimit != CommandLimitNone) && (this->Queue.Num() == this->SizeLimit))
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Verbose,
			TEXT("Command queue ('%s') is already at maximum capacity ('%d'), so command ('%s') will not be enqueued."),
			*(this->GetIdForLogs()),
			this->SizeLimit,
			*(Command->GetIdForLogs())
		);
	}
	else
	{
		checkf(!this->Queue.Contains(CommandActor), TEXT("The same command can only exist in the queue once."));

		UE_LOG(
			LogPf2CoreAbilities,
			VeryVerbose,
			TEXT("Queueing command ('%s') at end of command queue ('%s')."),
			*(Command->GetIdForLogs()),
			*(this->GetIdForLogs())
		);

		this->Queue.Add(CommandActor);

		this->Native_OnCommandAdded(Command);
		this->Native_OnCommandsChanged();
	}
}

void UPF2CommandQueueComponent::EnqueueAt(const TScriptInterface<IPF2CharacterCommandInterface>& Command,
                                          const int32                                            Position)
{
	AInfo* CommandActor = Command->ToActor();

	if (Position < 0)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Cannot queue command ('%s') in command queue ('%s'): Position ('%d') cannot be negative."),
			*(Command->GetIdForLogs()),
			*(this->GetIdForLogs()),
			Position
		);
		return;
	}

	if (Position > this->Queue.Num())
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Cannot queue command ('%s') in command queue ('%s'): Position ('%d') cannot be greater than queue size ('%d')."),
			*(Command->GetIdForLogs()),
			*(this->GetIdForLogs()),
			Position,
			this->Queue.Num()
		);
		return;
	}

	checkf(!this->Queue.Contains(CommandActor), TEXT("The same command can only exist in the queue once."));

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("Queueing command ('%s') at position '%d' in command queue ('%s')."),
		*(Command->GetIdForLogs()),
		Position,
		*(this->GetIdForLogs())
	);

	// Insert the new command before enforcing limits (in case we are inserting this new command at the end of the
	// queue).
	this->Queue.Insert(CommandActor, Position);

	// Now, if necessary, drop the last command.
	if ((this->SizeLimit != CommandLimitNone) && (this->Queue.Num() == this->SizeLimit))
	{
		AInfo* RemovedElement = this->Queue.Pop(false);

		UE_LOG(
			LogPf2CoreAbilities,
			Verbose,
			TEXT("Command queue ('%s') is already at maximum capacity ('%d'), so the last command in the queue ('%s') was dropped."),
			*(this->GetIdForLogs()),
			this->SizeLimit,
			*(IPF2LogIdentifiableInterface::GetIdForLogs(RemovedElement))
		);
	}

	this->Native_OnCommandAdded(Command);
	this->Native_OnCommandsChanged();
}

void UPF2CommandQueueComponent::PeekNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand)
{
	if (this->Count() != 0)
	{
		IPF2CharacterCommandInterface* NextCommandPtr = Cast<IPF2CharacterCommandInterface>(this->Queue[0]);

		NextCommand = PF2InterfaceUtilities::ToScriptInterface(NextCommandPtr);
	}
}

void UPF2CommandQueueComponent::PopNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand)
{
	if (this->Count() != 0)
	{
		this->PeekNext(NextCommand);

		UE_LOG(
			LogPf2CoreAbilities,
			VeryVerbose,
			TEXT("Popping command ('%s') from command queue ('%s')."),
			*(NextCommand->GetIdForLogs()),
			*(this->GetIdForLogs())
		);

		this->Queue.RemoveAt(0, 1, false);

		this->Native_OnCommandRemoved(NextCommand);
		this->Native_OnCommandsChanged();
	}
}

void UPF2CommandQueueComponent::DropNext()
{
	if (this->Count() != 0)
	{
		TScriptInterface<IPF2CharacterCommandInterface> NextCommand;

		this->PeekNext(NextCommand);

		UE_LOG(
			LogPf2CoreAbilities,
			VeryVerbose,
			TEXT("Removing command ('%s') from command queue ('%s')."),
			*(NextCommand->GetIdForLogs()),
			*(this->GetIdForLogs())
		);

		this->Queue.RemoveAt(0, 1, false);

		this->Native_OnCommandRemoved(NextCommand);
		this->Native_OnCommandsChanged();
	}
}

EPF2CommandExecuteImmediatelyResult UPF2CommandQueueComponent::PopAndExecuteNext()
{
	EPF2CommandExecuteImmediatelyResult             Result;
	TScriptInterface<IPF2CharacterCommandInterface> NextCommand;

	// We don't pop the command (yet) because it may be blocked and we don't want it to lose its place in the queue if
	// it is.
	this->PeekNext(NextCommand);

	if (NextCommand.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			VeryVerbose,
			TEXT("No actions are currently queued for command queue ('%s')."),
			*(this->GetIdForLogs())
		);

		Result = EPF2CommandExecuteImmediatelyResult::None;
	}
	else
	{
		IPF2CharacterCommandInterface* NextCommandIntf = PF2InterfaceUtilities::FromScriptInterface(NextCommand);

		UE_LOG(
			LogPf2CoreAbilities,
			VeryVerbose,
			TEXT("Attempt to execute next command ('%s') in command queue ('%s')."),
			*(NextCommandIntf->GetIdForLogs()),
			*(this->GetIdForLogs())
		);

		Result = NextCommandIntf->AttemptExecuteImmediately();

		if (Result == EPF2CommandExecuteImmediatelyResult::Blocked)
		{
			UE_LOG(
				LogPf2CoreAbilities,
				VeryVerbose,
				TEXT("Next command ('%s') in command queue ('%s') was blocked and will not be removed from the queue."),
				*(NextCommandIntf->GetIdForLogs()),
				*(this->GetIdForLogs())
			);
		}
		else
		{
			// Now it's safe to drop the command.
			this->Remove(NextCommand);
		}
	}

	return Result;
}

bool UPF2CommandQueueComponent::Remove(const TScriptInterface<IPF2CharacterCommandInterface>& Command)
{
	AInfo*      CommandActor       = Command->ToActor();
	const int32 CountOfRemoved     = this->Queue.Remove(CommandActor);
	const bool  bWasCommandRemoved = (CountOfRemoved > 0);

	if (bWasCommandRemoved)
	{
		this->Native_OnCommandRemoved(Command);
		this->Native_OnCommandsChanged();
	}

	return bWasCommandRemoved;
}

int UPF2CommandQueueComponent::Count()
{
	return this->Queue.Num();
}

void UPF2CommandQueueComponent::Clear()
{
	this->Queue.Empty(this->SizeLimit);
	this->Native_OnCommandsChanged();
}

TArray<TScriptInterface<IPF2CharacterCommandInterface>> UPF2CommandQueueComponent::ToArray() const
{
	return PF2ArrayUtilities::ReduceToArray<TScriptInterface<IPF2CharacterCommandInterface>>(
		this->Queue,
		[](TArray<TScriptInterface<IPF2CharacterCommandInterface>>& Commands,
		   const TWeakInterfacePtr<IPF2CharacterCommandInterface>&  CurrentCommand)
		{
			if (CurrentCommand.IsValid())
			{
				Commands.Add(PF2InterfaceUtilities::ToScriptInterface(CurrentCommand.Get()));
			}
		});
}

UActorComponent* UPF2CommandQueueComponent::ToActorComponent()
{
	return this;
}

void UPF2CommandQueueComponent::OnRep_Queue(const TArray<AInfo*>& OldQueue)
{
	const UPF2CommandQueueInterfaceEvents* InterfaceEvents = this->GetEvents();

	// Skip unnecessary overhead if we have no listeners. This is only safe because our Native_ callbacks don't do
	// anything other than notify listeners.
	if (InterfaceEvents->OnCommandAdded.IsBound() || InterfaceEvents->OnCommandRemoved.IsBound())
	{
		TArray<IPF2CharacterCommandInterface*> RemovedCommands,
		                                       AddedCommands;

		// BUGBUG: By the time we're here, this should definitely be an OpenPF2 command, but UE will sometimes replicate
		// entries in this->Queue as NULL.
		PF2ArrayUtilities::CapturePtrDeltasWithCast(OldQueue, this->Queue, RemovedCommands, AddedCommands);

		for (IPF2CharacterCommandInterface* const& RemovedCommand : RemovedCommands)
		{
			this->Native_OnCommandRemoved(PF2InterfaceUtilities::ToScriptInterface(RemovedCommand));
		}

		for (IPF2CharacterCommandInterface* const& AddedCommand : AddedCommands)
		{
			this->Native_OnCommandAdded(PF2InterfaceUtilities::ToScriptInterface(AddedCommand));
		}
	}

	this->Native_OnCommandsChanged();
}

void UPF2CommandQueueComponent::Native_OnCommandsChanged()
{
	const FPF2CommandQueueChangedDelegate& OnCommandsChanged = this->GetEvents()->OnCommandsChanged;

	// Skip unnecessary overhead if we have no listeners.
	if (OnCommandsChanged.IsBound())
	{
		TArray<TScriptInterface<IPF2CharacterCommandInterface>> NewCommands;

		for (AInfo* NewCommand : this->Queue)
		{
			// BUGBUG: By the time we're here, this should definitely be an OpenPF2 command, but UE will sometimes
			// replicate entries in this->Queue as NULL.
			if (NewCommand != nullptr)
			{
				NewCommands.Add(
					PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterCommandInterface>(
						Cast<IPF2CharacterCommandInterface>(NewCommand)
					)
				);
			}
		}

		UE_LOG(
			LogPf2CoreAbilities,
			VeryVerbose,
			TEXT("[%s] Command queue changed ('%s') - %d elements."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(this->GetIdForLogs()),
			NewCommands.Num()
		);

		OnCommandsChanged.Broadcast(this, NewCommands);
	}
	else
	{
		UE_LOG(
			LogPf2CoreAbilities,
			VeryVerbose,
			TEXT("[%s] Command queue changed ('%s')."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(this->GetIdForLogs())
		);
	}
}

void UPF2CommandQueueComponent::Native_OnCommandAdded(
	const TScriptInterface<IPF2CharacterCommandInterface>& CommandAdded)
{
	const FPF2CommandAddedToOrRemovedFromQueueDelegate& OnCommandAdded = this->GetEvents()->OnCommandAdded;

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] Command ('%s') added to queue ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(CommandAdded->GetIdForLogs()),
		*(this->GetIdForLogs())
	);

	if (OnCommandAdded.IsBound())
	{
		OnCommandAdded.Broadcast(this, CommandAdded);
	}
}

void UPF2CommandQueueComponent::Native_OnCommandRemoved(
	const TScriptInterface<IPF2CharacterCommandInterface>& CommandRemoved)
{
	const FPF2CommandAddedToOrRemovedFromQueueDelegate& OnCommandRemoved = this->GetEvents()->OnCommandRemoved;

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] Command ('%s') removed from queue ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(CommandRemoved->GetIdForLogs()),
		*(this->GetIdForLogs())
	);

	if (OnCommandRemoved.IsBound())
	{
		OnCommandRemoved.Broadcast(this, CommandRemoved);
	}
}
