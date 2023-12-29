// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "GameModes/Encounters/PF2CharacterInitiativeQueueComponent.h"

#include "PF2CharacterInterface.h"
#include "PF2PlayerControllerInterface.h"

#include "Libraries/PF2CharacterLibrary.h"

#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"
#include "Utilities/PF2MapUtilities.h"

bool UPF2CharacterInitiativeQueueComponent::IsEmpty()
{
	// ReSharper disable once CppRedundantParentheses
	return (this->CurrentCharacterSequence.Num() == 0);
}

int32 UPF2CharacterInitiativeQueueComponent::GetCharacterInitiative(
	const TScriptInterface<IPF2CharacterInterface>& Character) const
{
	int32                   Result;
	IPF2CharacterInterface* Pf2Character    = PF2InterfaceUtilities::FromScriptInterface(Character);
	const int32*            FoundInitiative = CharactersByInitiatives.FindKey(Pf2Character);

	if (FoundInitiative == nullptr)
	{
		Result = INDEX_NONE;
	}
	else
	{
		Result = *FoundInitiative;
	}

    return Result;
}

void UPF2CharacterInitiativeQueueComponent::SetCharacterInitiative(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	const int32                                     Initiative)
{
	if (Initiative <= 0)
	{
		UE_LOG(
			LogPf2CoreInitiative,
			Error,
			TEXT("[%s] Initiative for character ('%s') must be greater than 0; attempted to set it to '%d'."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(Character->GetIdForLogs()),
			Initiative
		);
	}
	else if (this->GetCharacterInitiative(Character) == Initiative)
	{
		UE_LOG(
			LogPf2CoreInitiative,
			Verbose,
			TEXT("[%s] Initiative for character ('%s') is already set to desired value ('%d'); no update necessary."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(Character->GetIdForLogs()),
			Initiative
		);
	}
	else
	{
		IPF2CharacterInterface* Pf2Character = PF2InterfaceUtilities::FromScriptInterface(Character);

		UE_LOG(
			LogPf2CoreInitiative,
			VeryVerbose,
			TEXT("[%s] Initiative ('%d') set for character ('%s')."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			Initiative,
			*(Character->GetIdForLogs())
		);

		// Ensure any existing initiative for this character is cleared.
		this->RemoveCharacterFromInitiativeMap(Pf2Character);

		this->CharactersByInitiatives.Add(Initiative, Pf2Character);
		this->RebuildCharacterSequence();
	}
}

bool UPF2CharacterInitiativeQueueComponent::IsInitiativeSetForCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character) const
{
	const IPF2CharacterInterface* Pf2Character = PF2InterfaceUtilities::FromScriptInterface(Character);

	return this->CurrentCharacterSequence.Contains(Pf2Character);
}

void UPF2CharacterInitiativeQueueComponent::InsertCharacterAtOrAboveInitiative(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	const int32                                     TargetInitiative)
{
	UE_LOG(
		LogPf2CoreInitiative,
		VeryVerbose,
		TEXT("[%s] Attempting to insert character ('%s') at or above initiative ('%d')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(Character->GetIdForLogs()),
		TargetInitiative
	);

	this->InsertCharacterAtOrRelativeToInitiative(Character, TargetInitiative, 1);
}

void UPF2CharacterInitiativeQueueComponent::InsertCharacterAtOrBelowInitiative(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	const int32                                     TargetInitiative)
{
	UE_LOG(
		LogPf2CoreInitiative,
		VeryVerbose,
		TEXT("[%s] Inserting character ('%s') at or below initiative ('%d')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(Character->GetIdForLogs()),
		TargetInitiative
	);

	this->InsertCharacterAtOrRelativeToInitiative(Character, TargetInitiative, -1);
}

void UPF2CharacterInitiativeQueueComponent::ClearInitiativeForCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character)
{
	const IPF2CharacterInterface* Pf2Character = PF2InterfaceUtilities::FromScriptInterface(Character);

	UE_LOG(
		LogPf2CoreInitiative,
		VeryVerbose,
		TEXT("[%s] Initiative cleared for character ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(Character->GetIdForLogs())
	);

	this->RemoveCharacterFromInitiativeMap(Pf2Character);
	this->RebuildCharacterSequence();
}

void UPF2CharacterInitiativeQueueComponent::ClearInitiativeForAllCharacters()
{
	UE_LOG(
		LogPf2CoreInitiative,
		VeryVerbose,
		TEXT("[%s] Initiative cleared for all characters."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld()))
	);

	this->CharactersByInitiatives.Empty();
	this->CurrentCharacterSequence.Empty();

	this->PreviousCharacter      = nullptr;
	this->PreviousCharacterIndex = -1;
}

TScriptInterface<IPF2CharacterInterface> UPF2CharacterInitiativeQueueComponent::GetNextCharacterByInitiative()
{
	TScriptInterface<IPF2CharacterInterface> Result;
	const int32                              CharacterCount = this->CurrentCharacterSequence.Num();

	if (CharacterCount == 0)
	{
		UE_LOG(
			LogPf2CoreInitiative,
			VeryVerbose,
			TEXT("[%s] There are no characters with initiative registered with this component."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld()))
		);

		// No characters to return.
		Result = TScriptInterface<IPF2CharacterInterface>();
	}
	else
	{
		int32                   NextCharacterIndex;
		IPF2CharacterInterface* NextCharacter;

		if ((this->PreviousCharacterIndex == -1) || (this->PreviousCharacterIndex == (CharacterCount - 1)))
		{
			// Restart at the beginning.
			NextCharacterIndex = 0;
		}
		else
		{
			// Advance to the character with the next-lowest initiative.
			NextCharacterIndex = this->PreviousCharacterIndex + 1;
		}

		NextCharacter = this->CurrentCharacterSequence[NextCharacterIndex];

		this->PreviousCharacterIndex = NextCharacterIndex;
		this->PreviousCharacter      = NextCharacter;

		Result = PF2InterfaceUtilities::ToScriptInterface(NextCharacter);
	}

	return Result;
}

TArray<TScriptInterface<IPF2CharacterInterface>> UPF2CharacterInitiativeQueueComponent::GetCharactersInInitiativeOrder() const
{
	return PF2InterfaceUtilities::ToScriptInterfaces<IPF2CharacterInterface>(this->CurrentCharacterSequence);
}

UActorComponent* UPF2CharacterInitiativeQueueComponent::ToActorComponent()
{
	return this;
}

TArray<TScriptInterface<IPF2PlayerControllerInterface>> UPF2CharacterInitiativeQueueComponent::GetPlayerControllers() const
{
	return UPF2CharacterLibrary::GetPlayerControllers(this->GetWorld());
}

TArray<TScriptInterface<IPF2CharacterInterface>> UPF2CharacterInitiativeQueueComponent::GetPlayerControlledCharacters() const
{
	return UPF2CharacterLibrary::GetPlayerControlledCharacters(this->GetWorld());
}

void UPF2CharacterInitiativeQueueComponent::RebuildCharacterSequence()
{
	TArray<int32>                   Initiatives;
	TArray<IPF2CharacterInterface*> NewCharacterSequence;

	const TArray<IPF2CharacterInterface*> PlayableCharacters =
		PF2InterfaceUtilities::FromScriptInterfaces<IPF2CharacterInterface>(this->GetPlayerControlledCharacters());

	this->CharactersByInitiatives.KeyStableSort(TGreater<int32>());
	this->CharactersByInitiatives.GetKeys(Initiatives);

	for (const int32 Initiative : Initiatives)
	{
		TArray<IPF2CharacterInterface*> CharactersForInitiative;

		this->CharactersByInitiatives.MultiFind(Initiative, CharactersForInitiative, true);

		CharactersForInitiative.StableSort(
			[PlayableCharacters](IPF2CharacterInterface& A, IPF2CharacterInterface& B)
			{
				bool       bCharacterAComesFirst = false;
				const bool bIsCharacterAPlayable = PlayableCharacters.Contains(&A),
				           bIsCharacterBPlayable = PlayableCharacters.Contains(&B);

				if (bIsCharacterAPlayable && !bIsCharacterBPlayable)
				{
					// PCs come after NPCs.
					bCharacterAComesFirst = false;
				}
				else if (bIsCharacterAPlayable == bIsCharacterBPlayable)
				{
					// Characters of the same type (either both NPCs or both PCs) retain their positions based on
					// insertion order.
					bCharacterAComesFirst = false;
				}
				else if (!bIsCharacterAPlayable && bIsCharacterBPlayable)
				{
					// NPCs come before PCs.
					bCharacterAComesFirst = true;
				}

				return bCharacterAComesFirst;
			}
		);

		NewCharacterSequence.Append(CharactersForInitiative);
	}

	this->CurrentCharacterSequence = NewCharacterSequence;

	// Fix-up our iterator so that we maintain where we're pointing in the sequence after everything shuffled around.
	if (this->PreviousCharacter != nullptr)
	{
		for (int CharacterIndex = 0; CharacterIndex < NewCharacterSequence.Num(); ++CharacterIndex)
		{
			if (this->PreviousCharacter == NewCharacterSequence[CharacterIndex])
			{
				this->PreviousCharacterIndex = CharacterIndex;
				break;
			}
		}
	}
}

void UPF2CharacterInitiativeQueueComponent::RemoveCharacterFromInitiativeMap(const IPF2CharacterInterface* Character)
{
	// If the character being removed is the character whose turn it is to act, we need to update which character is
	// active so that we don't inadvertently affect the turns of other characters after the character's previous
	// or new position.
	if (this->PreviousCharacter == Character)
	{
		int32 NewPreviousCharacterIndex;

		// If the character being removed was the last character we returned, jump back to pointing at the character
		// right before the character being removed so that the next character we return comes after what just got
		// removed.
		if (this->PreviousCharacterIndex > 0)
		{
			NewPreviousCharacterIndex = this->PreviousCharacterIndex - 1;
		}
		else
		{
			// The length will drop after the removal, but that doesn't need to be taken into account yet. We just need
			// to grab which character occupies the last slot before the length changes.
			NewPreviousCharacterIndex = this->CurrentCharacterSequence.Num() - 1;
		}

		// We do not update PreviousCharacterIndex here; it will get updated during the call to
		// RebuildCharacterSequence().
		this->PreviousCharacter = this->CurrentCharacterSequence[NewPreviousCharacterIndex];
	}

	for (auto It = this->CharactersByInitiatives.CreateIterator(); It; ++It)
	{
		if (It.Value() == Character)
		{
			// Remove existing initiative for this character.
			It.RemoveCurrent();
			break;
		}
	}
}

void UPF2CharacterInitiativeQueueComponent::InsertCharacterAtOrRelativeToInitiative(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	const int32                                     TargetInitiative,
	const int32                                     Offset)
{
	if (TargetInitiative <= 0)
	{
		UE_LOG(
			LogPf2CoreInitiative,
			Error,
			TEXT("[%s] Initiative for character ('%s') must be greater than 0; attempted to shift it to '%d'."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(Character->GetIdForLogs()),
			TargetInitiative
		);
	}
    else if (this->GetCharacterInitiative(Character) == TargetInitiative)
    {
		// Step 1: If the target character already has the specified initiative score, no changes to initiative are
		// made.
    	UE_LOG(
			LogPf2CoreInitiative,
			Verbose,
			TEXT("[%s] No need to adjust initiative of character ('%s'); their initiative is already equal to the target initiative ('%d')."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(Character->GetIdForLogs()),
			TargetInitiative
		);
    }
	else
	{
		int32 NewInitiative = TargetInitiative;

		// Step 2: If no character in the queue has the target initiative score, set the initiative of the target
		// character to the specified initiative score.
		if (this->CharactersByInitiatives.Contains(TargetInitiative))
		{
			// Step 3: If at least one character in the queue has the target initiative score:
			// Step 3a: Increment the target initiative score by the offset.
			NewInitiative += Offset;

			// Step 3b: If there is at least one character in the queue that has an initiative equal to the new
			// initiative score OR we have the special case of a new initiative score equal to 0:
			if ((NewInitiative == 0) || this->CharactersByInitiatives.Contains(NewInitiative))
			{
				// Step 3b I: All initiative scores are scaled up by 10, to ensure gaps between the existing initiative
				// scores.
				this->CharactersByInitiatives = PF2MapUtilities::Reduce(
					this->CharactersByInitiatives,
					TMultiMap<int, IPF2CharacterInterface*>(),
					[this](
						TMultiMap<int, IPF2CharacterInterface*>    ResultMap,
						const TPair<int, IPF2CharacterInterface*>& Pair)
					{
						const int               Key   = Pair.Key;
						IPF2CharacterInterface* Value = Pair.Value;

						ResultMap.Add(Key * 10, Value);

						return ResultMap;
					}
				);

				// Step 3b II: Set the target initiative score to: <Original passed-in value> * 10 + Offset.
				NewInitiative = TargetInitiative * 10 + Offset;

				UE_LOG(
					LogPf2CoreInitiative,
					VeryVerbose,
					TEXT("[%s] Scaled up all initiative scores to deconflict initiative for character ('%s') to new initiative score ('%d')."),
					*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
					*(Character->GetIdForLogs()),
					NewInitiative
				);
			}
			else
			{
				UE_LOG(
					LogPf2CoreInitiative,
					VeryVerbose,
					TEXT("[%s] Deconflicted initiative for character ('%s') to new initiative score ('%d')."),
					*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
					*(Character->GetIdForLogs()),
					NewInitiative
				);
			}
		}

		this->SetCharacterInitiative(Character, NewInitiative);
	}
}
