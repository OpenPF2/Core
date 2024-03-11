// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ModesOfPlay/Encounter/PF2CharacterQueueComponent.h"

#include <Net/UnrealNetwork.h>

#include "PF2CharacterInterface.h"

#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

UPF2CharacterQueueComponent::UPF2CharacterQueueComponent() : Events(nullptr), ControlledCharacterIndex(0)
{
	this->SetIsReplicatedByDefault(true);
}

void UPF2CharacterQueueComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPF2CharacterQueueComponent, Queue);
	DOREPLIFETIME(UPF2CharacterQueueComponent, ControlledCharacterIndex);
}

UObject* UPF2CharacterQueueComponent::GetGenericEventsObject() const
{
	return this->GetEvents();
}

UPF2CharacterQueueInterfaceEvents* UPF2CharacterQueueComponent::GetEvents() const
{
	if (this->Events == nullptr)
	{
		// BUGBUG: This has to be instantiated here rather than via CreateDefaultSubobject() in the constructor, or it
		// breaks multiplayer. It seems that when created in the constructor, all instances of this component end up
		// sharing one events object leading to all players receiving the event whenever a multicast event is broadcast.
		// This typically results in a crash since the addresses of callbacks aren't valid on all clients.
		this->Events = NewObject<UPF2CharacterQueueInterfaceEvents>(
			const_cast<UPF2CharacterQueueComponent*>(this),
			FName(TEXT("InterfaceEvents"))
		);
	}

	return this->Events;
}

TScriptInterface<IPF2CharacterInterface> UPF2CharacterQueueComponent::GetControlledCharacter() const
{
	return this->ControlledCharacter;
}

void UPF2CharacterQueueComponent::Add(const TScriptInterface<IPF2CharacterInterface> Character)
{
	check(this->Count() <= (static_cast<uint32>(UINT8_MAX) + 1));

	this->Queue.AddUnique(Character->ToActor());

	this->Native_OnCharacterAdded(Character);
	this->Native_OnCharactersChanged();
}

bool UPF2CharacterQueueComponent::Remove(const TScriptInterface<IPF2CharacterInterface> Character)
{
	AActor*     CharacterActor       = Character->ToActor();
	const int32 RemovedIndex         = this->Queue.Find(CharacterActor);
	const int32 CountOfRemoved       = this->Queue.Remove(CharacterActor);
	const bool  bWasCharacterRemoved = (CountOfRemoved > 0);

	if (bWasCharacterRemoved)
	{
		check(RemovedIndex != INDEX_NONE);

		this->Native_OnCharacterRemoved(Character, RemovedIndex);
		this->Native_OnCharactersChanged();
	}

	return bWasCharacterRemoved;
}

int UPF2CharacterQueueComponent::Count() const
{
	return this->Queue.Num();
}

void UPF2CharacterQueueComponent::Clear()
{
	TArray<AActor*> OldQueue = this->Queue;

	this->Queue.Empty();

	for (int CharacterIndex = 0; CharacterIndex < OldQueue.Num(); ++CharacterIndex)
	{
		AActor*                 Character     = OldQueue[CharacterIndex];
		IPF2CharacterInterface* CharacterIntf = Cast<IPF2CharacterInterface>(Character);

		check(CharacterIntf != nullptr);

		this->Native_OnCharacterRemoved(
			PF2InterfaceUtilities::ToScriptInterface(CharacterIntf),
			CharacterIndex
		);
	}

	this->Native_OnCharactersChanged();
}

TScriptInterface<IPF2CharacterInterface> UPF2CharacterQueueComponent::PreviousCharacter()
{
	// Only rewind if we have elements.
	if (this->Count() != 0)
	{
		uint8 NewCharacterIndex;

		if (this->ControlledCharacterIndex == 0)
		{
			// We're already at the start, so restart with the character at the end of the queue.
			NewCharacterIndex = this->GetMaxIndex();
		}
		else
		{
			// We're not yet at the start, so just jump back one character in the queue.
			NewCharacterIndex = this->ControlledCharacterIndex - 1;
		}

		this->SetControlledCharacterIndex(NewCharacterIndex);
	}

	return this->GetControlledCharacter();
}

TScriptInterface<IPF2CharacterInterface> UPF2CharacterQueueComponent::NextCharacter()
{
	// Only advance if we have elements.
	if (this->Count() != 0)
	{
		uint8 NewCharacterIndex;

		if (this->ControlledCharacterIndex == this->GetMaxIndex())
		{
			// We're already at the end, so restart with the character at the start of the queue.
			NewCharacterIndex = 0;
		}
		else
		{
			// We're not yet at the end, so just jump forward one character in the queue.
			NewCharacterIndex = this->ControlledCharacterIndex + 1;
		}

		this->SetControlledCharacterIndex(NewCharacterIndex);
	}

	return this->GetControlledCharacter();
}

TArray<TScriptInterface<IPF2CharacterInterface>> UPF2CharacterQueueComponent::ToArray() const
{
	return PF2ArrayUtilities::ReduceToArray<TScriptInterface<IPF2CharacterInterface>>(
		this->Queue,
		[](TArray<TScriptInterface<IPF2CharacterInterface>>& Characters,
		   const TWeakInterfacePtr<IPF2CharacterInterface>& CurrentCharacter)
		{
			if (CurrentCharacter.IsValid())
			{
				Characters.Add(PF2InterfaceUtilities::ToScriptInterface(CurrentCharacter.Get()));
			}
		});
}

UActorComponent* UPF2CharacterQueueComponent::ToActorComponent()
{
	return this;
}

void UPF2CharacterQueueComponent::SetControlledCharacterIndex(const uint8 NewControlledCharacterIndex)
{
	check(NewControlledCharacterIndex < this->Count());

	this->ControlledCharacterIndex = NewControlledCharacterIndex;

	this->UpdateControlledCharacter();
}

void UPF2CharacterQueueComponent::UpdateControlledCharacter()
{
	const TScriptInterface<IPF2CharacterInterface> OldCharacter = this->ControlledCharacter;
	TScriptInterface<IPF2CharacterInterface>       NewCharacter;

	if (this->Count() == 0)
	{
		// No active character.
		NewCharacter = TScriptInterface<IPF2CharacterInterface>();
	}
	else
	{
		AActor*                 ControlledCharacterActor;
		IPF2CharacterInterface* ControlledCharacterIntf;

		check(this->ControlledCharacterIndex < this->Count());

		ControlledCharacterActor = this->Queue[this->ControlledCharacterIndex];
		ControlledCharacterIntf  = Cast<IPF2CharacterInterface>(ControlledCharacterActor);

		if (ControlledCharacterIntf == nullptr)
		{
			// BUGBUG: By the time we're here, this should definitely be an OpenPF2 character, but UE will sometimes
			// replicate entries in this->ControllableCharacters as NULL.
			NewCharacter = TScriptInterface<IPF2CharacterInterface>();
		}
		else
		{
			NewCharacter = PF2InterfaceUtilities::ToScriptInterface(ControlledCharacterIntf);
		}
	}

	this->ControlledCharacter = NewCharacter;

	if (OldCharacter != NewCharacter)
	{
		this->Native_OnControlledCharacterChanged(OldCharacter, NewCharacter);
	}
}

void UPF2CharacterQueueComponent::OnRep_CharacterQueue(const TArray<AActor*>& OldCharacters)
{
	TArray<IPF2CharacterInterface*> RemovedCharacters,
	                                AddedCharacters;

	// BUGBUG: By the time we're here, this should definitely be an OpenPF2 character, but UE will sometimes replicate
	// entries in this->ControllableCharacters as NULL.
	PF2ArrayUtilities::CapturePtrDeltasWithCast(OldCharacters, this->Queue, RemovedCharacters, AddedCharacters);

	// We execute this logic even if we have no registered listeners because we still need to do internal bookkeeping
	// when the queue changes.
	for (IPF2CharacterInterface* const& RemovedCharacter : RemovedCharacters)
	{
		const int32 RemovedIndex = OldCharacters.Find(RemovedCharacter->ToActor());

		check(RemovedIndex != INDEX_NONE);

		this->Native_OnCharacterRemoved(PF2InterfaceUtilities::ToScriptInterface(RemovedCharacter), RemovedIndex);
	}

	for (IPF2CharacterInterface* const& AddedCharacter : AddedCharacters)
	{
		this->Native_OnCharacterAdded(PF2InterfaceUtilities::ToScriptInterface(AddedCharacter));
	}

	if ((RemovedCharacters.Num() != 0) || (AddedCharacters.Num() != 0))
	{
		this->Native_OnCharactersChanged();
	}
}

void UPF2CharacterQueueComponent::OnRep_ControlledCharacterIndex()
{
	this->UpdateControlledCharacter();
}

void UPF2CharacterQueueComponent::Native_OnCharactersChanged()
{
	const FPF2CharacterQueueChangedDelegate CharacterChangedDelegate = this->GetEvents()->OnCharactersChanged;

	if (CharacterChangedDelegate.IsBound())
	{
		TArray<TScriptInterface<IPF2CharacterInterface>> NewCharacters;

		for (AActor* Character : this->Queue)
		{
			// BUGBUG: By the time we're here, this should definitely be an OpenPF2 character, but UE will sometimes
			// replicate entries in this->Queue as NULL.
			if (Character != nullptr)
			{
				NewCharacters.Add(
					PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterInterface>(
						Cast<IPF2CharacterInterface>(Character)
					)
				);
			}
		}

		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("[%s] Character queue changed ('%s') - %d elements."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(this->GetIdForLogs()),
			NewCharacters.Num()
		);

		CharacterChangedDelegate.Broadcast(this, NewCharacters);
	}
	else
	{
		UE_LOG(
			LogPf2Core,
			Verbose,
			TEXT("[%s] Character queue changed ('%s')."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(this->GetIdForLogs())
		);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPF2CharacterQueueComponent::Native_OnCharacterAdded(
	const TScriptInterface<IPF2CharacterInterface>& AddedCharacter)
{
	const FPF2CharacterAddedOrRemovedDelegate CharacterAddedDelegate = this->GetEvents()->OnCharacterAdded;

	UE_LOG(
		LogPf2Core,
		Verbose,
		TEXT("[%s] Character ('%s') added to queue ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(AddedCharacter->GetIdForLogs()),
		*(this->GetIdForLogs())
	);

	this->UpdateControlledCharacter();

	if (CharacterAddedDelegate.IsBound())
	{
		CharacterAddedDelegate.Broadcast(this, AddedCharacter);
	}
}

void UPF2CharacterQueueComponent::Native_OnCharacterRemoved(
	const TScriptInterface<IPF2CharacterInterface>& RemovedCharacter,
	const uint8                                     RemovedIndex)
{
	const FPF2CharacterAddedOrRemovedDelegate CharacterRemovedDelegate = this->GetEvents()->OnCharacterRemoved;

	UE_LOG(
		LogPf2Core,
		Verbose,
		TEXT("[%s] Character ('%s') removed from queue ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(RemovedCharacter->GetIdForLogs()),
		*(this->GetIdForLogs())
	);

	// Ensure that we keep the active character cursor in bounds. If the character that was removed was the active
	// character or a character prior to it, we move the active character index accordingly.
	if (this->ControlledCharacterIndex >= RemovedIndex)
	{
		if (this->ControlledCharacterIndex == 0)
		{
			// The last item just got removed, so jump directly to notifying listeners.
			this->UpdateControlledCharacter();
		}
		else
		{
			this->SetControlledCharacterIndex(this->ControlledCharacterIndex - 1);
		}
	}

	if (CharacterRemovedDelegate.IsBound())
	{
		CharacterRemovedDelegate.Broadcast(this, RemovedCharacter);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPF2CharacterQueueComponent::Native_OnControlledCharacterChanged(
	const TScriptInterface<IPF2CharacterInterface>& OldCharacter,
	const TScriptInterface<IPF2CharacterInterface>& NewCharacter)
{
	const FPF2ControlledCharacterChangedDelegate ControlledCharacterChangedDelegate =
		this->GetEvents()->OnControlledCharacterChanged;

	UE_LOG(
		LogPf2Core,
		Verbose,
		TEXT("[%s] Active character in queue ('%s') has changed (was '%s'; now '%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetIdForLogs()),
		*((OldCharacter.GetInterface() != nullptr) ? OldCharacter->GetIdForLogs() : "null"),
		*((NewCharacter.GetInterface() != nullptr) ? NewCharacter->GetIdForLogs() : "null")
	);

	if (ControlledCharacterChangedDelegate.IsBound())
	{
		ControlledCharacterChangedDelegate.Broadcast(this, OldCharacter, NewCharacter);
	}
}
