// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ModesOfPlay/Encounter/PF2EncounterModeOfPlayRuleSetBase.h"

#include "OpenPF2Core.h"
#include "PF2CharacterInterface.h"
#include "PF2PlayerControllerInterface.h"

#include "Commands/PF2CharacterCommandInterface.h"
#include "Commands/PF2CommandQueueInterface.h"

#include "ModesOfPlay/Encounter/PF2CharacterInitiativeQueueComponent.h"

#include "Utilities/PF2EnumUtilities.h"
#include "Utilities/PF2LogUtilities.h"

APF2EncounterModeOfPlayRuleSetBase::APF2EncounterModeOfPlayRuleSetBase()
{
	this->CharacterInitiativeQueue =
		this->CreateDefaultSubobject<UPF2CharacterInitiativeQueueComponent>(TEXT("CharacterInitiativeQueue"));
}

void APF2EncounterModeOfPlayRuleSetBase::OnModeOfPlayEnd(const EPF2ModeOfPlayType ModeOfPlay)
{
	Super::OnModeOfPlayEnd(ModeOfPlay);

	// Be sure to cleanly stop any encounter-specific behavior for each character still in the encounter.
	this->RemoveAllCharactersFromEncounter();
}

bool APF2EncounterModeOfPlayRuleSetBase::HavePlayableCharacters() const
{
	bool Result = false;

	if (!this->GetCharacterInitiativeQueue()->IsEmpty())
	{
		for (const TScriptInterface<IPF2CharacterInterface> Character : this->GetAllCharactersInInitiativeOrder())
		{
			if (Character->IsAlive())
			{
				Result = true;
				break;
			}
		}
	}

	return Result;
}

void APF2EncounterModeOfPlayRuleSetBase::SetCharacterInitiative(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	const int32                                     Initiative)
{
	this->GetCharacterInitiativeQueue()->SetCharacterInitiative(Character, Initiative);
}

bool APF2EncounterModeOfPlayRuleSetBase::IsInitiativeSetForCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character) const
{
	return this->GetCharacterInitiativeQueue()->IsInitiativeSetForCharacter(Character);
}

void APF2EncounterModeOfPlayRuleSetBase::ClearInitiativeForCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character)
{
	this->GetCharacterInitiativeQueue()->ClearInitiativeForCharacter(Character);
}

void APF2EncounterModeOfPlayRuleSetBase::ClearInitiativeForAllCharacters()
{
	this->GetCharacterInitiativeQueue()->ClearInitiativeForAllCharacters();
}

TScriptInterface<IPF2CharacterInterface> APF2EncounterModeOfPlayRuleSetBase::GetNextCharacterByInitiative()
{
	return this->GetCharacterInitiativeQueue()->GetNextCharacterByInitiative();
}

TArray<TScriptInterface<IPF2CharacterInterface>> APF2EncounterModeOfPlayRuleSetBase::GetAllCharactersInInitiativeOrder() const
{
	return this->GetCharacterInitiativeQueue()->GetCharactersInInitiativeOrder();
}

TScriptInterface<IPF2CharacterInterface> APF2EncounterModeOfPlayRuleSetBase::GetActiveCharacter() const
{
	return this->ActiveCharacter;
}

void APF2EncounterModeOfPlayRuleSetBase::MoveInitiativeHigherThanCharacter(
	const TScriptInterface<IPF2CharacterInterface>& AffectedCharacter,
	const TScriptInterface<IPF2CharacterInterface>& OtherCharacter) const
{
	if (AffectedCharacter == nullptr)
	{
		UE_LOG(
			LogPf2CoreInitiative,
			Error,
			TEXT("MoveInitiativeHigherThanCharacter(): Given a null 'affected' character.")
		);
	}
	else if (OtherCharacter == nullptr)
	{
		UE_LOG(
			LogPf2CoreInitiative,
			Error,
			TEXT("MoveInitiativeHigherThanCharacter(): Given a null 'other' character.")
		);
	}
	else
	{
		const TScriptInterface<IPF2CharacterInitiativeQueueInterface> Initiatives = this->GetCharacterInitiativeQueue();
		const int32 OtherInitiative = Initiatives->GetCharacterInitiative(OtherCharacter);

		if (OtherInitiative == INDEX_NONE)
		{
			UE_LOG(
				LogPf2CoreInitiative,
				Error,
				TEXT("MoveInitiativeHigherThanCharacter(): No initiative has been set for other character '%s'."),
				*(OtherCharacter->GetIdForLogs())
			);
		}
		else
		{
			UE_LOG(
				LogPf2CoreInitiative,
				VeryVerbose,
				TEXT("[%s] Attempting to move character ('%s') before character ('%s') in initiative queue (higher initiative)."),
				*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
				*(AffectedCharacter->GetIdForLogs()),
				*(OtherCharacter->GetIdForLogs())
			);

			Initiatives->InsertCharacterAtOrAboveInitiative(AffectedCharacter, OtherInitiative);
		}
	}
}

void APF2EncounterModeOfPlayRuleSetBase::MoveInitiativeLowerThanCharacter(
	const TScriptInterface<IPF2CharacterInterface>& AffectedCharacter,
	const TScriptInterface<IPF2CharacterInterface>& OtherCharacter) const
{
	if (AffectedCharacter == nullptr)
	{
		UE_LOG(
			LogPf2CoreInitiative,
			Error,
			TEXT("MoveInitiativeLowerThanCharacter(): Given a null 'affected' character.")
		);
	}
	else if (OtherCharacter == nullptr)
	{
		UE_LOG(
			LogPf2CoreInitiative,
			Error,
			TEXT("MoveInitiativeLowerThanCharacter(): Given a null 'other' character.")
		);
	}
	else
	{
		const TScriptInterface<IPF2CharacterInitiativeQueueInterface> Initiatives = this->GetCharacterInitiativeQueue();
		const int32 OtherInitiative = Initiatives->GetCharacterInitiative(OtherCharacter);

		if (OtherInitiative == INDEX_NONE)
		{
			UE_LOG(
				LogPf2CoreInitiative,
				Error,
				TEXT("MoveInitiativeLowerThanCharacter(): No initiative has been set for other character '%s'."),
				*(OtherCharacter->GetIdForLogs())
			);
		}
		else
		{
			UE_LOG(
				LogPf2CoreInitiative,
				VeryVerbose,
				TEXT("[%s] Attempting to move character ('%s') after character ('%s') in initiative queue (lower initiative)."),
				*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
				*(AffectedCharacter->GetIdForLogs()),
				*(OtherCharacter->GetIdForLogs())
			);

			Initiatives->InsertCharacterAtOrBelowInitiative(AffectedCharacter, OtherInitiative);
		}
	}
}

void APF2EncounterModeOfPlayRuleSetBase::StartTurnForCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character)
{
	const TScriptInterface<IPF2PlayerControllerInterface> PlayerController = Character->GetPlayerController();

	check(Character.GetInterface() != nullptr);

	UE_LOG(
		LogPf2CoreEncounters,
		Verbose,
		TEXT("Starting turn for character ('%s')."),
		*(Character->GetIdForLogs())
	);

	this->BP_OnCharacterTurnStart(Character);
	this->SetActiveCharacter(Character);

	if (PlayerController.GetInterface() != nullptr)
	{
		IPF2PlayerControllerInterface::Execute_Multicast_OnEncounterTurnStarted(PlayerController.GetObject());
	}

	IPF2CharacterInterface::Execute_Multicast_OnEncounterTurnStarted(Character.GetObject());
}

void APF2EncounterModeOfPlayRuleSetBase::EndTurnForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	const TScriptInterface<IPF2PlayerControllerInterface> PlayerController = Character->GetPlayerController();

	check(Character.GetInterface() != nullptr);

	UE_LOG(
		LogPf2CoreEncounters,
		Verbose,
		TEXT("Ending turn for character ('%s')."),
		*(Character->GetIdForLogs())
	);

	this->BP_OnCharacterTurnEnd(Character);
	this->SetActiveCharacter(TScriptInterface<IPF2CharacterInterface>(nullptr));

	if (PlayerController.GetInterface() != nullptr)
	{
		IPF2PlayerControllerInterface::Execute_Multicast_OnEncounterTurnEnded(PlayerController.GetObject());
	}

	IPF2CharacterInterface::Execute_Multicast_OnEncounterTurnEnded(Character.GetObject());
}

bool APF2EncounterModeOfPlayRuleSetBase::DoesCharacterHaveNextCommandQueued(
	const TScriptInterface<IPF2CharacterInterface>& Character) const
{
	bool                                              Result;
	const TScriptInterface<IPF2CommandQueueInterface> CommandQueue = Character->GetCommandQueueComponent();

	if (CommandQueue.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Warning,
			TEXT("Character ('%s') lacks a command queue component; it is not possible to queue commands."),
			*(Character->GetIdForLogs())
		);

		Result = false;
	}
	else
	{
		TScriptInterface<IPF2CharacterCommandInterface> NextCommand;

		CommandQueue->PeekNext(NextCommand);

		Result = (NextCommand.GetInterface() != nullptr);
	}

	return Result;
}

void APF2EncounterModeOfPlayRuleSetBase::QueueCommandForCharacter(
	const TScriptInterface<IPF2CharacterInterface>&        Character,
	const TScriptInterface<IPF2CharacterCommandInterface>& Command)
{
	const TScriptInterface<IPF2CommandQueueInterface> CommandQueue = Character->GetCommandQueueComponent();

	if (CommandQueue.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Error,
			TEXT("Character ('%s') lacks a command queue component; unable to queue command ('%s')."),
			*(Character->GetIdForLogs()),
			*(Command->GetIdForLogs())
		);
	}
	else
	{
		const EPF2CommandQueuePosition QueuePositionPreference = Command->GetQueuePositionPreference();

		UE_LOG(
			LogPf2CoreEncounters,
			VeryVerbose,
			TEXT("Queuing command ('%s') for character ('%s') at ('%s')."),
			*(Command->GetIdForLogs()),
			*(Character->GetIdForLogs()),
			*(PF2EnumUtilities::ToString(QueuePositionPreference))
		);

		switch (QueuePositionPreference)
		{
			case EPF2CommandQueuePosition::BeginningOfQueue:
				CommandQueue->EnqueueAt(Command, 0);
				break;

			case EPF2CommandQueuePosition::NextAfterBeginningOfQueue:
				if (CommandQueue->Count() > 0)
				{
					// At least one command is queued, so place the new command right after the first command in the
					// queue.
					CommandQueue->EnqueueAt(Command, 1);
				}
				else
				{
					// Nothing is queued so make this command the next command executed.
					CommandQueue->EnqueueAt(Command, 0);
				}
				break;

			case EPF2CommandQueuePosition::EndOfQueue:
				// Intentional fall-through.
			default:
				CommandQueue->Enqueue(Command);
		}
	}
}

void APF2EncounterModeOfPlayRuleSetBase::CancelQueuedCommandsForAllCharacters()
{
	for (const TScriptInterface<IPF2CharacterInterface> Character : this->GetAllCharactersInInitiativeOrder())
	{
		const TScriptInterface<IPF2CommandQueueInterface> CommandQueue = Character->GetCommandQueueComponent();

		if (CommandQueue.GetInterface() == nullptr)
		{
			UE_LOG(
				LogPf2CoreEncounters,
				Warning,
				TEXT("Character ('%s') lacks a command queue component; no commands to clear."),
				*(Character->GetIdForLogs())
			);
		}
		else
		{
			CommandQueue->Clear();
		}
	}
}

EPF2CommandExecuteImmediatelyResult APF2EncounterModeOfPlayRuleSetBase::ExecuteNextQueuedCommandForCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character)
{
	EPF2CommandExecuteImmediatelyResult               Result;
	const TScriptInterface<IPF2CommandQueueInterface> CommandQueue = Character->GetCommandQueueComponent();

	if (CommandQueue.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Warning,
			TEXT("Character ('%s') lacks a command queue component; unable to execute any commands."),
			*(Character->GetIdForLogs())
		);

		Result = EPF2CommandExecuteImmediatelyResult::None;
	}
	else
	{
		Result = CommandQueue->PopAndExecuteNext();
	}

	return Result;
}

void APF2EncounterModeOfPlayRuleSetBase::PeekNextQueuedCommandForCharacter(
	const TScriptInterface<IPF2CharacterInterface>&  Character,
	TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) const
{
	const TScriptInterface<IPF2CommandQueueInterface> CommandQueue = Character->GetCommandQueueComponent();

	if (CommandQueue.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Warning,
			TEXT("Character ('%s') lacks a command queue component; it is not possible to queue commands."),
			*(Character->GetIdForLogs())
		);

		NextCommand = TScriptInterface<IPF2CharacterCommandInterface>(nullptr);
	}
	else
	{
		CommandQueue->PeekNext(NextCommand);
	}
}

void APF2EncounterModeOfPlayRuleSetBase::PopNextCommandQueuedForCharacter(
	const TScriptInterface<IPF2CharacterInterface>&  Character,
	TScriptInterface<IPF2CharacterCommandInterface>& NextCommand)
{
	const TScriptInterface<IPF2CommandQueueInterface> CommandQueue = Character->GetCommandQueueComponent();

	if (CommandQueue.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Warning,
			TEXT("Character ('%s') lacks a command queue component; it is not possible to queue commands."),
			*(Character->GetIdForLogs())
		);

		NextCommand = TScriptInterface<IPF2CharacterCommandInterface>(nullptr);
	}
	else
	{
		CommandQueue->PopNext(NextCommand);
	}
}

void APF2EncounterModeOfPlayRuleSetBase::SetActiveCharacter(
	const TScriptInterface<IPF2CharacterInterface>& NewActiveCharacter)
{
	this->ActiveCharacter = NewActiveCharacter;
}

void APF2EncounterModeOfPlayRuleSetBase::RemoveAllCharactersFromEncounter()
{
	for (const TScriptInterface<IPF2CharacterInterface>& Character : this->GetAllCharactersInInitiativeOrder())
	{
		this->RemoveCharacterFromEncounter(Character);
	}
}
