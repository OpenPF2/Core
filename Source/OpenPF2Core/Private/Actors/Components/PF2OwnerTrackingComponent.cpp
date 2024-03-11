// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.
//

#include "Actors/Components/PF2OwnerTrackingComponent.h"

#include <Engine/World.h>

#include <GameFramework/PlayerState.h>

#include <Net/UnrealNetwork.h>

#include "PF2CharacterInterface.h"
#include "PF2PartyInterface.h"
#include "PF2PlayerControllerInterface.h"
#include "PF2PlayerStateInterface.h"

#include "Utilities/PF2InterfaceUtilities.h"

UPF2OwnerTrackingComponent::UPF2OwnerTrackingComponent() : Events(nullptr)
{
	this->SetIsReplicatedByDefault(true);
}

void UPF2OwnerTrackingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPF2OwnerTrackingComponent, OwningPlayerState);
	DOREPLIFETIME(UPF2OwnerTrackingComponent, Party);
}

UObject* UPF2OwnerTrackingComponent::GetGenericEventsObject() const
{
	return this->GetEvents();
}

UPF2OwnerTrackingInterfaceEvents* UPF2OwnerTrackingComponent::GetEvents() const
{
	if (this->Events == nullptr)
	{
		// BUGBUG: This has to be instantiated here rather than via CreateDefaultSubobject() in the constructor, or it
		// breaks multiplayer. It seems that when created in the constructor, this component ends up as part of the CDO
		// and then all instances of this component share *one* events object, leading to all game clients being
		// notified about every multicast event broadcast for all instances. This typically results in a crash since the
		// addresses of callbacks aren't valid for clients who don't own the component handling the event.
		this->Events = NewObject<UPF2OwnerTrackingInterfaceEvents>(
			const_cast<UPF2OwnerTrackingComponent*>(this),
			FName(TEXT("InterfaceEvents"))
		);
	}

	return this->Events;
}

TScriptInterface<IPF2PartyInterface> UPF2OwnerTrackingComponent::GetParty() const
{
	return this->Party;
}

void UPF2OwnerTrackingComponent::SetParty(const TScriptInterface<IPF2PartyInterface> NewParty)
{
	const TScriptInterface<IPF2PartyInterface> OldParty = this->Party;

	if (NewParty != OldParty)
	{
		const TScriptInterface<IPF2PlayerStateInterface> PlayerState      = this->GetStateOfOwningPlayer();
		int32                                            OwningPartyIndex = IPF2PartyInterface::PartyIndexNone,
		                                                 NewPartyIndex    = IPF2PartyInterface::PartyIndexNone;

		if (PlayerState.GetInterface() != nullptr)
		{
			const TScriptInterface<IPF2PartyInterface> OwningPlayerParty = PlayerState->GetParty();

			if (OwningPlayerParty.GetInterface() != nullptr)
			{
				OwningPartyIndex = OwningPlayerParty->GetPartyIndex();
			}
		}

		if (NewParty.GetInterface() != nullptr)
		{
			NewPartyIndex = NewParty->GetPartyIndex();
		}

		if ((PlayerState.GetInterface() == nullptr) || (OwningPartyIndex == NewPartyIndex))
		{
			this->Party = Cast<AInfo>(NewParty.GetObject());

			this->Native_OnPartyChanged(OldParty, NewParty);
		}
		else
		{
			UE_LOG(
				LogPf2Core,
				Error,
				TEXT("Owner tracker ('%s') cannot be affiliated with a party ('%i') that differs from that of the party ('%i') of its owning player ('%s')."),
				*(this->GetIdForLogs()),
				NewPartyIndex,
				OwningPartyIndex,
				*(PlayerState->GetIdForLogs())
			);
		}
	}
}

TScriptInterface<IPF2PlayerStateInterface> UPF2OwnerTrackingComponent::GetStateOfOwningPlayer() const
{
	return this->OwningPlayerState;
}

void UPF2OwnerTrackingComponent::SetOwningPlayerByController(
	const TScriptInterface<IPF2PlayerControllerInterface> NewController)
{
	this->SetOwningPlayerByState(NewController->GetPlayerState());
}

void UPF2OwnerTrackingComponent::SetOwningPlayerByState(const TScriptInterface<IPF2PlayerStateInterface> NewPlayerState)
{
	const TScriptInterface<IPF2PlayerStateInterface> OldOwningPlayerState = this->GetStateOfOwningPlayer();

	if (NewPlayerState != OldOwningPlayerState)
	{
		this->OwningPlayerState = Cast<APlayerState>(NewPlayerState.GetObject());

		this->Native_OnOwningPlayerStateChanged(OldOwningPlayerState, NewPlayerState);
	}
}

bool UPF2OwnerTrackingComponent::IsSamePartyAsActor(AActor* OtherActor) const
{
	bool Result = false;

	if (OtherActor != nullptr)
	{
		const TScriptInterface<IPF2PlayerStateInterface> MyOwner = this->GetStateOfOwningPlayer();
		const IPF2OwnerTrackingInterface* OtherComponent =
			PF2InterfaceUtilities::FindComponentByInterface<IPF2OwnerTrackingInterface, UPF2OwnerTrackingInterface>(OtherActor);

		if ((MyOwner.GetInterface() != nullptr) && (OtherComponent != nullptr))
		{
			const TScriptInterface<IPF2PlayerStateInterface> OtherOwner = OtherComponent->GetStateOfOwningPlayer();

			Result = MyOwner->IsSamePartyAsPlayerWithState(OtherOwner);
		}
	}

	return Result;
}

bool UPF2OwnerTrackingComponent::IsSamePartyAsPlayerWithController(
	const TScriptInterface<IPF2PlayerControllerInterface> OtherController) const
{
	bool                                             Result      = false;
	const TScriptInterface<IPF2PlayerStateInterface> PlayerState = this->GetStateOfOwningPlayer();

	if (PlayerState.GetInterface() != nullptr)
	{
		TScriptInterface<IPF2PlayerStateInterface> OtherPlayerState;

		check(OtherController.GetInterface() != nullptr);

		OtherPlayerState = OtherController->GetPlayerState();

		Result = PlayerState->IsSamePartyAsPlayerWithState(OtherPlayerState);
	}

	return Result;
}

UActorComponent* UPF2OwnerTrackingComponent::ToActorComponent()
{
	return this;
}

void UPF2OwnerTrackingComponent::OnRep_OwningPlayerState(APlayerState* OldOwner)
{
	const TScriptInterface<IPF2PlayerStateInterface> OldPf2Owner = OldOwner;
	const TScriptInterface<IPF2PlayerStateInterface> NewPf2Owner = this->GetStateOfOwningPlayer();

    this->Native_OnOwningPlayerStateChanged(OldPf2Owner, NewPf2Owner);
}

void UPF2OwnerTrackingComponent::OnRep_Party(AInfo* OldParty)
{
	const TScriptInterface<IPF2PartyInterface> OldPf2Party = OldParty;
	const TScriptInterface<IPF2PartyInterface> NewPf2Party = this->GetParty();

	this->Native_OnPartyChanged(OldPf2Party, NewPf2Party);
}

void UPF2OwnerTrackingComponent::Native_OnOwningPlayerStateChanged(
	const TScriptInterface<IPF2PlayerStateInterface> OldOwner,
	const TScriptInterface<IPF2PlayerStateInterface> NewOwner)
{
	const FPF2OwnerComponentOwningPlayerStateChangedDelegate& OnOwnerChanged = this->GetEvents()->OnOwnerChanged;

	if (OnOwnerChanged.IsBound())
	{
		OnOwnerChanged.Broadcast(this, this->GetOwner(), OldOwner, NewOwner);
	}
}

void UPF2OwnerTrackingComponent::Native_OnPartyChanged(const TScriptInterface<IPF2PartyInterface> OldParty,
                                                       const TScriptInterface<IPF2PartyInterface> NewParty)
{
	const FPF2OwnerComponentPartyChangedDelegate& OnPartyChanged = this->GetEvents()->OnPartyChanged;

	if (OnPartyChanged.IsBound())
	{
		OnPartyChanged.Broadcast(this, this->GetOwner(), OldParty, NewParty);
	}
}
