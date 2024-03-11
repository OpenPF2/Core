// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#include "PF2GameModeBase.h"

#include <EngineUtils.h>

#include <GameFramework/Character.h>

#include "PF2CharacterInterface.h"
#include "PF2GameStateInterface.h"
#include "PF2PartyInterface.h"
#include "PF2PlayerStateInterface.h"

#include "Actors/Components/PF2OwnerTrackingInterface.h"

#include "Commands/PF2CharacterCommandInterface.h"

#include "Libraries/PF2CharacterCommandLibrary.h"

#include "ModesOfPlay/PF2ModeOfPlayRuleSetInterface.h"

#include "Utilities/PF2EnumUtilities.h"
#include "Utilities/PF2InterfaceUtilities.h"

TScriptInterface<IPF2ModeOfPlayRuleSetInterface> APF2GameModeBase::CreateModeOfPlayRuleSet(
	const EPF2ModeOfPlayType ModeOfPlay)
{
	TScriptInterface<IPF2ModeOfPlayRuleSetInterface> RuleSetWrapper;

	if (this->ModeRuleSets.Contains(ModeOfPlay))
	{
		UClass* const RuleSetType = this->ModeRuleSets[ModeOfPlay];
		UObject*      NewRuleSet;

		// Rule sets are usually actors so that they can be composed of actor components, but the interface doesn't
		// strictly require them to be actors. Therefore, we have to instantiate a rule set appropriately for its type,
		// since actors have to be added to the world (so that actor callbacks like BeginPlay are invoked), while base
		// UObjects can't be.
		if (RuleSetType->IsChildOf(AActor::StaticClass()))
		{
			NewRuleSet = this->GetWorld()->SpawnActor(RuleSetType);
		}
		else
		{
			NewRuleSet = NewObject<UObject>(this, RuleSetType);
		}

		RuleSetWrapper = TScriptInterface<IPF2ModeOfPlayRuleSetInterface>(NewRuleSet);
	}

	return RuleSetWrapper;
}

void APF2GameModeBase::TransferCharacterOwnership(
	const TScriptInterface<IPF2CharacterInterface>        Character,
	const TScriptInterface<IPF2PlayerControllerInterface> NewController)
{
	TScriptInterface<IPF2OwnerTrackingInterface> OwnerTracker;

	check(Character.GetInterface() != nullptr);

	OwnerTracker = Character->GetOwnerTrackingComponent();

	if (OwnerTracker.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2Core,
			Warning,
			TEXT("Character ('%s') lacks an owner tracking component, so it will not be able to respond properly to ownership changes."),
			*(NewController->GetIdForLogs())
		);
	}
	else
	{
		const TScriptInterface<IPF2PlayerStateInterface> OldOwnerPlayerState = OwnerTracker->GetStateOfOwningPlayer();

		if (OldOwnerPlayerState.GetInterface() != nullptr)
		{
			const TScriptInterface<IPF2PlayerControllerInterface> OldController =
				OldOwnerPlayerState->GetPlayerControllerIntf();

			OldController->ReleaseCharacter(Character);
		}

		OwnerTracker->SetOwningPlayerByController(NewController);
	}

	if (NewController.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2Core,
			Verbose,
			TEXT("Character ('%s') is now no longer owned by any character."),
			*(Character->GetIdForLogs())
		);
	}
	else
	{
		UE_LOG(
			LogPf2Core,
			Verbose,
			TEXT("Player controller ('%s') now owns character ('%s')."),
			*(NewController->GetIdForLogs()),
			*(Character->GetIdForLogs())
		);

		NewController->GiveCharacter(Character);
	}

	// Set player controller as networking authority for the character.
	Character->ToActor()->SetOwner(NewController->ToPlayerController());
}

void APF2GameModeBase::SwitchPartyOfPlayer(const TScriptInterface<IPF2PlayerControllerInterface> PlayerController,
                                           const TScriptInterface<IPF2PartyInterface>            NewParty)
{
	const TScriptInterface<IPF2PlayerStateInterface> PlayerState = PlayerController->GetPlayerState();
	TScriptInterface<IPF2PartyInterface>             OldParty;

	check(PlayerState.GetInterface() != nullptr);

	OldParty = PlayerState->GetParty();

	if (OldParty != NewParty)
	{
		for (const auto& Character : PlayerController->GetControllableCharacters())
		{
			PlayerController->ReleaseCharacter(Character);
		}

		// We already released all the characters, so this allows us to reuse the logic without duplicating it here.
		this->SwitchPartyOfPlayerAndOwnedCharacters(PlayerController, NewParty);
	}
}

void APF2GameModeBase::SwitchPartyOfPlayerAndOwnedCharacters(
	const TScriptInterface<IPF2PlayerControllerInterface> PlayerController,
	const TScriptInterface<IPF2PartyInterface>            NewParty)
{
	TScriptInterface<IPF2PlayerStateInterface> PlayerState;
	TScriptInterface<IPF2PartyInterface>       OldParty;

	check(PlayerController.GetInterface() != nullptr);

	PlayerState = PlayerController->GetPlayerState();
	check(PlayerState.GetInterface() != nullptr);

	OldParty = PlayerState->GetParty();

	if (OldParty != NewParty)
	{
		// Remove from old party, if there is one.
		if (OldParty.GetInterface() != nullptr)
		{
			OldParty->RemovePlayerFromPartyByState(PlayerState);
		}

		PlayerState->SetParty(NewParty);

		// Associate all controllable characters with the new party.
		for (const auto& Character : PlayerController->GetControllableCharacters())
		{
			const TScriptInterface<IPF2OwnerTrackingInterface> OwnerTracker = Character->GetOwnerTrackingComponent();

			OwnerTracker->SetParty(NewParty);
		}

		// Notify the new party, if there is one.
		if (NewParty.GetInterface() != nullptr)
		{
			NewParty->AddPlayerToPartyByState(PlayerState);
		}
	}
}

void APF2GameModeBase::RequestEncounterMode()
{
	this->AttemptModeOfPlaySwitch(EPF2ModeOfPlayType::Encounter);
}

void APF2GameModeBase::RequestExplorationMode()
{
	this->AttemptModeOfPlaySwitch(EPF2ModeOfPlayType::Exploration);
}

void APF2GameModeBase::RequestDowntimeMode()
{
	this->AttemptModeOfPlaySwitch(EPF2ModeOfPlayType::Downtime);
}

void APF2GameModeBase::AddCharacterToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	const TScriptInterface<IPF2ModeOfPlayRuleSetInterface> RuleSet = this->GetModeOfPlayRuleSet();

	if (RuleSet.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Error,
			TEXT("No MoPRS is set. Ignoring request to add character ('%s') to encounter."),
			*(Character->GetIdForLogs())
		);
	}
	else
	{
		RuleSet->OnCharacterAddedToEncounter(Character);
	}
}

void APF2GameModeBase::RemoveCharacterFromEncounter(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	const TScriptInterface<IPF2ModeOfPlayRuleSetInterface> RuleSet = this->GetModeOfPlayRuleSet();

	if (RuleSet.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Error,
			TEXT("No MoPRS is set. Ignoring request to remove character ('%s') from encounter."),
			*(Character->GetCharacterName().ToString())
		);
	}
	else
	{
		RuleSet->OnCharacterRemovedFromEncounter(Character);
	}
}

EPF2CommandExecuteOrQueueResult APF2GameModeBase::AttemptToExecuteOrQueueCommand(
	const TScriptInterface<IPF2CharacterCommandInterface>& Command)
{
	EPF2CommandExecuteOrQueueResult                        Result;
	const TScriptInterface<IPF2ModeOfPlayRuleSetInterface> RuleSet = this->GetModeOfPlayRuleSet();

	if (RuleSet.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Error,
			TEXT("No MoPRS is set. Performing command ('%s') without queuing."),
			*(Command->GetCommandLabel().ToString())
		);

		Result =
			UPF2CharacterCommandLibrary::ImmediateResultToExecuteOrQueueResult(Command->AttemptExecuteImmediately());
	}
	else
	{
		Result = IPF2ModeOfPlayRuleSetInterface::Execute_AttemptToExecuteOrQueueCommand(RuleSet.GetObject(), Command);
	}

	return Result;
}

bool APF2GameModeBase::AttemptToQueueCommand(const TScriptInterface<IPF2CharacterCommandInterface>& Command)
{
	bool                                                   bWasQueued;
	const TScriptInterface<IPF2ModeOfPlayRuleSetInterface> RuleSet    = this->GetModeOfPlayRuleSet();

	if (RuleSet.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Error,
			TEXT("No MoPRS is set. Dropping command ('%s') without queuing."),
			*(Command->GetCommandLabel().ToString())
		);

		bWasQueued = false;
	}
	else
	{
		bWasQueued = IPF2ModeOfPlayRuleSetInterface::Execute_AttemptToQueueCommand(RuleSet.GetObject(), Command);
	}

	return bWasQueued;
}

void APF2GameModeBase::AttemptToCancelCommand(const TScriptInterface<IPF2CharacterCommandInterface>& Command)
{
	const TScriptInterface<IPF2ModeOfPlayRuleSetInterface> RuleSet = this->GetModeOfPlayRuleSet();

	if (RuleSet.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Error,
			TEXT("No MoPRS is set. Cannot cancel command ('%s')."),
			*(Command->GetCommandLabel().ToString())
		);
	}
	else
	{
		IPF2ModeOfPlayRuleSetInterface::Execute_AttemptToCancelCommand(RuleSet.GetObject(), Command);
	}
}

void APF2GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Start off in exploration mode.
	this->AttemptModeOfPlaySwitch(EPF2ModeOfPlayType::Exploration);
}

void APF2GameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	TScriptInterface<IPF2ModeOfPlayRuleSetInterface> RuleSet;

	this->AssignPlayerIndex(NewPlayer);
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	RuleSet = this->GetModeOfPlayRuleSet();

	if (RuleSet.GetInterface() != nullptr)
	{
		IPF2PlayerControllerInterface* PlayerControllerIntf = Cast<IPF2PlayerControllerInterface>(NewPlayer);

		if (PlayerControllerIntf == nullptr)
		{
			ACharacter*             Character     = NewPlayer->GetCharacter();
			IPF2CharacterInterface* CharacterIntf = Cast<IPF2CharacterInterface>(Character);

			if (CharacterIntf == nullptr)
			{
				UE_LOG(
					LogPf2Core,
					Error,
					TEXT("Unable to notify player controller (%s) nor pawn that character (%s) is starting. Neither the character nor player controller are compatible with OpenPF2."),
					*(GetNameSafe(NewPlayer)),
					*(GetNameSafe(Character))
				);
			}
			else
			{
				UE_LOG(
					LogPf2Core,
					Error,
					TEXT("Unable to notify player controller (%s) that character (%s) is starting. The player controller is not compatible with OpenPF2. The pawn will be notified."),
					*(GetNameSafe(NewPlayer)),
					*(GetNameSafe(Character))
				);

				// Player controller is not compatible with OpenPF2; fallback to just interacting with the pawn.
				RuleSet->OnPlayableCharacterStarting(
					PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterInterface>(CharacterIntf)
				);
			}
		}
		else
		{
			// Trigger a "starting" callback for all the characters owned by the player who is joining.
			PlayerControllerIntf->Native_OnPlayableCharactersStarting(RuleSet);
		}
	}
}

TScriptInterface<IPF2ModeOfPlayRuleSetInterface> APF2GameModeBase::GetModeOfPlayRuleSet()
{
	TScriptInterface<IPF2ModeOfPlayRuleSetInterface> RuleSet;
	const IPF2GameStateInterface*                    Pf2GameState = this->GetGameState<IPF2GameStateInterface>();

	if (Pf2GameState == nullptr)
	{
		UE_LOG(
			LogPf2Core,
			Error,
			TEXT("Mode of Play Rule Set (MoPRS) support is not enabled because the current game state is not compatible with OpenPF2.")
		);

		RuleSet = TScriptInterface<IPF2ModeOfPlayRuleSetInterface>();
	}
	else
	{
		RuleSet = Pf2GameState->GetModeOfPlayRuleSet();
	}

	return RuleSet;
}

void APF2GameModeBase::AssignPlayerIndex(const APlayerController* PlayerController) const
{
	IPF2PlayerStateInterface* PlayerStateIntf = PlayerController->GetPlayerState<IPF2PlayerStateInterface>();

	if (PlayerStateIntf != nullptr)
	{
		const uint8 NextPlayerIndex = this->GeneratePlayerIndex();

		PlayerStateIntf->SetPlayerIndex(NextPlayerIndex);
	}
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
TScriptInterface<IPF2PartyInterface> APF2GameModeBase::SpawnParty(const TSubclassOf<AActor> PartyType)
{
	TScriptInterface<IPF2PartyInterface> NewParty;

	if (PartyType->ImplementsInterface(UPF2PartyInterface::StaticClass()))
	{
		IPF2PartyInterface* Party = this->GetWorld()->SpawnActor<IPF2PartyInterface>(PartyType);

		Party->SetPartyIndex(this->GeneratePartyIndex());

		NewParty = PF2InterfaceUtilities::ToScriptInterface(Party);
	}
	else
	{
		UE_LOG(
			LogPf2Core,
			Error,
			TEXT("Party Type must implement IPF2PartyInterface.")
		);

		NewParty = TScriptInterface<IPF2PartyInterface>(nullptr);
	}

	return NewParty;
}

void APF2GameModeBase::AttemptModeOfPlaySwitch(const EPF2ModeOfPlayType NewModeOfPlay)
{
	const TScriptInterface<IPF2GameStateInterface> Pf2GameState =
		PF2InterfaceUtilities::ToScriptInterface(this->GetGameStateIntf());

	if (Pf2GameState.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2Core,
			Error,
			TEXT("Cannot transition to new mode of play (%s) because current game state is not compatible with OpenPF2."),
			*PF2EnumUtilities::ToString(NewModeOfPlay)
		);
	}
	else
	{
		const EPF2ModeOfPlayType                               OldModeOfPlay  = Pf2GameState->GetModeOfPlay();
		const TScriptInterface<IPF2ModeOfPlayRuleSetInterface> OldRuleSet     = Pf2GameState->GetModeOfPlayRuleSet();
		bool                                                   bCanTransition;

		if (OldModeOfPlay == EPF2ModeOfPlayType::None)
		{
			// We're not in any mode.
			bCanTransition = true;
		}
		else if (OldRuleSet.GetInterface() == nullptr)
		{
			// This typically should not happen. If it does, then the game designer is missing a rule set mapping. By
			// default, we'll allow the transition since we have no rule set to veto it.
			UE_LOG(
				LogPf2Core,
				Warning,
				TEXT("There is no loaded rule set, so a requested transition from the current mode of play (%s) to a new mode of play (%s) has automatically been allowed."),
				*PF2EnumUtilities::ToString(OldModeOfPlay),
				*PF2EnumUtilities::ToString(NewModeOfPlay)
			);

			bCanTransition = true;
		}
		else if (!IPF2ModeOfPlayRuleSetInterface::Execute_CanTransitionTo(OldRuleSet.GetObject(), Pf2GameState, NewModeOfPlay))
		{
			UE_LOG(
				LogPf2Core,
				Warning,
				TEXT("Refusing to transition from current mode of play (%s) to new mode (%s) because loaded rule set does not allow the transition."),
				*PF2EnumUtilities::ToString(Pf2GameState->GetModeOfPlay()),
				*PF2EnumUtilities::ToString(NewModeOfPlay)
			);

			bCanTransition = false;
		}
		else
		{
			bCanTransition = true;
		}

		if (bCanTransition)
		{
			this->ForceSwitchModeOfPlay(NewModeOfPlay);
		}
	}
}

void APF2GameModeBase::ForceSwitchModeOfPlay(const EPF2ModeOfPlayType NewModeOfPlay)
{
	const TScriptInterface<IPF2GameStateInterface> Pf2GameState =
		PF2InterfaceUtilities::ToScriptInterface(this->GetGameStateIntf());

	if (Pf2GameState.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2Core,
			Error,
			TEXT("Cannot transition to new mode of play (%s) because current game state is not compatible with OpenPF2."),
			*PF2EnumUtilities::ToString(NewModeOfPlay)
		);
	}
	else
	{
		const EPF2ModeOfPlayType                               OldModeOfPlay = Pf2GameState->GetModeOfPlay();
		const TScriptInterface<IPF2ModeOfPlayRuleSetInterface> OldRuleSet    = Pf2GameState->GetModeOfPlayRuleSet();
		const TScriptInterface<IPF2ModeOfPlayRuleSetInterface> NewRuleSet    = this->CreateModeOfPlayRuleSet(NewModeOfPlay);

		if (OldRuleSet.GetInterface() != nullptr)
		{
			AActor* OldRuleSetActor = Cast<AActor>(OldRuleSet.GetObject());

			OldRuleSet->OnModeOfPlayEnd(OldModeOfPlay);

			if (OldRuleSetActor != nullptr)
			{
				// Rule sets are usually actors, but the interface doesn't strictly require them to be. If the old rule
				// set was implemented as an actor, then we also need to remove it from the world.
				OldRuleSetActor->Destroy();
			}
		}

		Pf2GameState->SetModeOfPlay(NewModeOfPlay, NewRuleSet);

		if (NewRuleSet.GetInterface() != nullptr)
		{
			NewRuleSet->OnModeOfPlayStart(NewModeOfPlay);
		}
	}
}
