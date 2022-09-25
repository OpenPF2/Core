﻿// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#include "PF2PlayerControllerBase.h"

#include <AIController.h>

#include <GameFramework/PlayerState.h>

#include <Net/UnrealNetwork.h>

#include "PF2CharacterInterface.h"
#include "PF2OwnerTrackingInterface.h"
#include "PF2PartyInterface.h"
#include "PF2PlayerStateInterface.h"

#include "Commands/PF2CharacterCommand.h"

#include "GameModes/PF2GameModeInterface.h"

#include "Utilities/PF2ArrayUtilities.h"
#include "Utilities/PF2EnumUtilities.h"
#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

void APF2PlayerControllerBase::InitPlayerState()
{
	Super::InitPlayerState();

	this->Native_OnPlayerStateAvailable(this->GetPlayerState());
}

void APF2PlayerControllerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	this->Native_OnPlayerStateAvailable(this->GetPlayerState());
}

void APF2PlayerControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APF2PlayerControllerBase, ControllableCharacters);
}

void APF2PlayerControllerBase::SetPawn(APawn* NewPawn)
{
	UE_LOG(
		LogPf2Core,
		VeryVerbose,
		TEXT("[%s] Player controller ('%s') has taken possession of pawn ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetName()),
		*(GetFullNameSafe(NewPawn))
	);

	Super::SetPawn(NewPawn);
}

TScriptInterface<IPF2PlayerStateInterface> APF2PlayerControllerBase::GetPlayerState() const
{
	TScriptInterface<IPF2PlayerStateInterface> Result;

	if (this->PlayerState == nullptr)
	{
		Result = TScriptInterface<IPF2PlayerStateInterface>();
	}
	else
	{
		Result = PF2InterfaceUtilities::ToScriptInterface(Cast<IPF2PlayerStateInterface>(this->PlayerState));
	}

	return Result;
}


TArray<TScriptInterface<IPF2CharacterInterface>> APF2PlayerControllerBase::GetControllableCharacters() const
{
	return PF2ArrayUtilities::Reduce<TArray<TScriptInterface<IPF2CharacterInterface>>>(
		this->ControllableCharacters,
		TArray<TScriptInterface<IPF2CharacterInterface>>(),
		[](TArray<TScriptInterface<IPF2CharacterInterface>> Characters,
		   const TWeakInterfacePtr<IPF2CharacterInterface>  CurrentCharacter)
		{
			if (CurrentCharacter.IsValid())
			{
				Characters.Add(PF2InterfaceUtilities::ToScriptInterface(CurrentCharacter.Get()));
			}

			return Characters;
		});
}

APlayerController* APF2PlayerControllerBase::ToPlayerController()
{
	return this;
}

void APF2PlayerControllerBase::Native_OnModeOfPlayChanged(const EPF2ModeOfPlayType NewMode)
{
	UE_LOG(
		LogPf2Core,
		VeryVerbose,
		TEXT("[%s] Player controller ('%s') has been notified of a change in mode of play to '%s'."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetName()),
		*(PF2EnumUtilities::ToString(NewMode))
	);

	this->BP_OnModeOfPlayChanged(NewMode);
}

void APF2PlayerControllerBase::GiveCharacter(const TScriptInterface<IPF2CharacterInterface>& GivenCharacter)
{
	const TScriptInterface<IPF2PartyInterface>   ThisParty = this->GetPlayerState()->GetParty();
	TScriptInterface<IPF2OwnerTrackingInterface> OwnerTracker;
	int32                                        ThisPartyIndex  = IPF2PartyInterface::PartyIndexNone,
	                                             OtherPartyIndex = IPF2PartyInterface::PartyIndexNone;

	check(GivenCharacter != nullptr);

	if (ThisParty != nullptr)
	{
		ThisPartyIndex = ThisParty->GetPartyIndex();
	}

	OwnerTracker = GivenCharacter->GetOwnerTrackingComponent();

	if (OwnerTracker != nullptr)
	{
		const TScriptInterface<IPF2PartyInterface> OtherParty = OwnerTracker->GetParty();

		if (OtherParty != nullptr)
		{
			OtherPartyIndex = OtherParty->GetPartyIndex();
		}
	}

	if (ThisPartyIndex == OtherPartyIndex)
	{
		UE_LOG(
			LogPf2Core,
			Verbose,
			TEXT("[%s] Player controller ('%s') has been granted the ability to control a character ('%s')."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(this->GetIdForLogs()),
			*(GivenCharacter->GetIdForLogs())
		);

		this->ControllableCharacters.AddUnique(GivenCharacter->ToActor());
		this->Native_OnCharacterGiven(GivenCharacter);
	}
	else
	{
		UE_LOG(
			LogPf2Core,
			Error,
			TEXT("The given character ('%s') is affiliated with a different party ('%i') than the player ('%i')."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(GivenCharacter->GetIdForLogs()),
			ThisPartyIndex,
			OtherPartyIndex
		);
	}
}

void APF2PlayerControllerBase::ReleaseCharacter(const TScriptInterface<IPF2CharacterInterface>& ReleasedCharacter)
{
	UE_LOG(
		LogPf2Core,
		Verbose,
		TEXT("[%s] Player controller ('%s') can no longer control a character ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetIdForLogs()),
		*(ReleasedCharacter->GetIdForLogs())
	);

	this->ControllableCharacters.Remove(ReleasedCharacter->ToActor());
	this->Native_OnCharacterReleased(ReleasedCharacter);
}

bool APF2PlayerControllerBase::Server_ExecuteCharacterCommand_Validate(
	const FGameplayAbilitySpecHandle AbilitySpecHandle,
	AActor* CharacterActor)
{
	IPF2CharacterInterface* TargetCharacter = Cast<IPF2CharacterInterface>(CharacterActor);
	APawn*                  CharacterPawn;

	if (TargetCharacter == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Server_ExecuteCharacterCommand(%s,%s): Character must implement IPF2CharacterInterface."),
			*(AbilitySpecHandle.ToString()),
			*(GetNameSafe(CharacterActor))
		);

		return false;
	}

	CharacterPawn = TargetCharacter->ToPawn();

	if (CharacterPawn == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Server_ExecuteCharacterCommand(%s,%s): Non-pawn character passed to player controller ('%s')."),
			*(AbilitySpecHandle.ToString()),
			*(TargetCharacter->GetIdForLogs()),
			*(this->GetIdForLogs())
		);

		return false;
	}

	if ((CharacterPawn->GetController() != this) && !this->GetControllableCharacters().Contains(CharacterPawn))
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Server_ExecuteCharacterCommand(%s,%s): Target character must be controllable by this player controller ('%s')."),
			*(AbilitySpecHandle.ToString()),
			*(TargetCharacter->GetIdForLogs()),
			*(this->GetIdForLogs())
		);

		return false;
	}

	return true;
}

void APF2PlayerControllerBase::Server_ExecuteCharacterCommand_Implementation(
	const FGameplayAbilitySpecHandle AbilitySpecHandle,
	AActor* CharacterActor)
{
	IPF2CharacterInterface*        TargetCharacter = Cast<IPF2CharacterInterface>(CharacterActor);
	IPF2CharacterCommandInterface* CharacterCommandIntf;
	APawn*                         CharacterPawn;

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("Server_ExecuteCharacterCommand(%s,%s) called on player controller ('%s')."),
		*(AbilitySpecHandle.ToString()),
		*(GetNameSafe(CharacterActor)),
		*(this->GetIdForLogs())
	);

	// Already checked by Validate() callback.
	check(TargetCharacter != nullptr);

	// Already checked by Validate() callback.
	CharacterPawn = TargetCharacter->ToPawn();
	check(CharacterPawn != nullptr);

	CharacterCommandIntf = APF2CharacterCommand::Create(TargetCharacter, AbilitySpecHandle);

	CharacterCommandIntf->AttemptExecuteOrQueue();
}

bool APF2PlayerControllerBase::Server_CancelCharacterCommand_Validate(AInfo* Command)
{
	const IPF2CharacterCommandInterface* CommandIntf = Cast<IPF2CharacterCommandInterface>(Command);

	if (CommandIntf == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Server_CancelCharacterCommand(%s): Command must implement IPF2CharacterCommandInterface."),
			*(GetNameSafe(Command))
		);

		return false;
	}
	else
	{
		const TScriptInterface<IPF2CharacterInterface> TargetCharacter = CommandIntf->GetTargetCharacter();

		if (TargetCharacter == nullptr)
		{
			UE_LOG(
				LogPf2CoreAbilities,
				Error,
				TEXT("Server_CancelCharacterCommand(%s): Target character cannot be null."),
				*(GetNameSafe(Command))
			);

			return false;
		}

		if ((TargetCharacter->ToPawn()->GetController() != this) &&
			!this->GetControllableCharacters().Contains(TargetCharacter->ToActor()))
		{
			UE_LOG(
				LogPf2CoreAbilities,
				Error,
				TEXT("Server_CancelCharacterCommand(%s): Target character ('%s') must be controllable by this player controller ('%s')."),
				*(GetNameSafe(Command)),
				*(TargetCharacter->GetIdForLogs()),
				*(this->GetIdForLogs())
			);

			return false;
		}
	}

	return true;
}

void APF2PlayerControllerBase::Server_CancelCharacterCommand_Implementation(AInfo* Command)
{
	IPF2CharacterCommandInterface* CommandIntf = Cast<IPF2CharacterCommandInterface>(Command);

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("Server_CancelCharacterCommand(%s) called on player controller ('%s')."),
		*(GetNameSafe(Command)),
		*(this->GetIdForLogs())
	);

	// Already checked by Validate() callback.
	check(CommandIntf != nullptr);

	// Just defer back to the command. Since we're on the server side, this should not result in infinite recursion
	// because the server implementation is for the command to call into the game mode.
	CommandIntf->AttemptCancel();
}

void APF2PlayerControllerBase::Multicast_OnEncounterTurnStarted_Implementation()
{
	this->BP_OnEncounterTurnStarted();
}

void APF2PlayerControllerBase::Multicast_OnEncounterTurnEnded_Implementation()
{
	this->BP_OnEncounterTurnEnded();
}

FString APF2PlayerControllerBase::GetIdForLogs() const
{
	return this->GetName();
}

void APF2PlayerControllerBase::OnRep_ControllableCharacters(const TArray<AActor*> OldCharacters)
{
	TArray<IPF2CharacterInterface*> RemovedCharacters,
	                                AddedCharacters;

	// Identify which characters were removed.
	for (AActor* const OldCharacter : OldCharacters)
	{
		IPF2CharacterInterface* CharacterIntf = Cast<IPF2CharacterInterface>(OldCharacter);

		// BUGBUG: By the time we're here, this should definitely be a PF2 character, but UE will sometimes replicate
		// entries in this->ControllableCharacters as NULL.
		if ((CharacterIntf != nullptr) && !this->ControllableCharacters.Contains(OldCharacter))
		{
			RemovedCharacters.Add(CharacterIntf);
		}
	}

	// Identify which characters were added.
	for (AActor* const NewCharacter : this->ControllableCharacters)
	{
		IPF2CharacterInterface* CharacterIntf = Cast<IPF2CharacterInterface>(NewCharacter);

		// BUGBUG: By the time we're here, this should definitely be a PF2 character, but UE will sometimes replicate
		// entries in this->ControllableCharacters as NULL.
		if ((CharacterIntf != nullptr) && !OldCharacters.Contains(NewCharacter))
		{
			AddedCharacters.Add(CharacterIntf);
		}
	}

	for (IPF2CharacterInterface* const& RemovedCharacter : RemovedCharacters)
	{
		this->Native_OnCharacterReleased(PF2InterfaceUtilities::ToScriptInterface(RemovedCharacter));
	}

	for (IPF2CharacterInterface* const& AddedCharacter : AddedCharacters)
	{
		this->Native_OnCharacterGiven(PF2InterfaceUtilities::ToScriptInterface(AddedCharacter));
	}
}

void APF2PlayerControllerBase::Native_OnPlayerStateAvailable(
	const TScriptInterface<IPF2PlayerStateInterface> NewPlayerState)
{
	if (NewPlayerState == nullptr)
	{
		return;
	}

	UE_LOG(
		LogPf2Core,
		VeryVerbose,
		TEXT("[%s] Player controller ('%s') has made player state ('%s') available for character ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetIdForLogs()),
		*(NewPlayerState->GetIdForLogs()),
		*(NewPlayerState->ToPlayerState()->GetPlayerName())
	);

	this->BP_OnPlayerStateAvailable(NewPlayerState);
}

void APF2PlayerControllerBase::Native_OnCharacterGiven(const TScriptInterface<IPF2CharacterInterface>& GivenCharacter)
{
	this->BP_OnCharacterGiven(GivenCharacter);
}

void APF2PlayerControllerBase::Native_OnCharacterReleased(
	const TScriptInterface<IPF2CharacterInterface>& ReleasedCharacter)
{
	this->BP_OnCharacterGiven(ReleasedCharacter);
}
