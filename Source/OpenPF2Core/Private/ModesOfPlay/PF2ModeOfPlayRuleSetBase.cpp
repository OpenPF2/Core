// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ModesOfPlay/PF2ModeOfPlayRuleSetBase.h"

#include <Engine/World.h>

// ReSharper disable once CppUnusedIncludeDirective
#include <GameFramework/GameModeBase.h>

#include "Commands/PF2CharacterCommandInterface.h"
#include "Commands/PF2CommandQueueInterface.h"

#include "Libraries/PF2CharacterCommandLibrary.h"
#include "Libraries/PF2CharacterLibrary.h"

#include "Utilities/PF2InterfaceUtilities.h"

APF2ModeOfPlayRuleSetBase::APF2ModeOfPlayRuleSetBase()
{
	this->DyingConditionTag       = PF2GameplayAbilityUtilities::GetTag(DyingConditionTagName);
	this->DeadConditionTag        = PF2GameplayAbilityUtilities::GetTag(DeadConditionTagName);
	this->UnconsciousConditionTag = PF2GameplayAbilityUtilities::GetTag(UnconsciousConditionTagName);
}

void APF2ModeOfPlayRuleSetBase::OnModeOfPlayStart(const EPF2ModeOfPlayType ModeOfPlay)
{
	this->BP_OnModeOfPlayStart(ModeOfPlay);
}

void APF2ModeOfPlayRuleSetBase::OnPlayableCharacterStarting(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	this->BP_OnPlayableCharacterStarting(Character);
}

void APF2ModeOfPlayRuleSetBase::OnCharacterAddedToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	const TWeakObjectPtr<AActor> CharacterPtr = Character->ToActor();

	if (this->ConditionCallbackHandles.Contains(CharacterPtr))
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Error,
			TEXT("OnCharacterAddedToEncounter() was invoked with character ('%s') that already has condition callbacks registered."),
			*(Character->GetIdForLogs())
		);
	}
	else
	{
		this->RegisterTagCallback(
			CharacterPtr,
			this->UnconsciousConditionTag,
			&APF2ModeOfPlayRuleSetBase::Native_OnCharacterUnconscious,
			&APF2ModeOfPlayRuleSetBase::Native_OnCharacterConscious
		);

		this->RegisterTagCallback(
			CharacterPtr,
			this->DyingConditionTag,
			&APF2ModeOfPlayRuleSetBase::Native_OnCharacterDying,
			&APF2ModeOfPlayRuleSetBase::Native_OnCharacterRecoveredFromDying
		);

		this->RegisterTagCallback(
			CharacterPtr,
			this->DeadConditionTag,
			&APF2ModeOfPlayRuleSetBase::Native_OnCharacterDead,
			nullptr
		);
	}

	this->BP_OnCharacterAddedToEncounter(Character);
}

void APF2ModeOfPlayRuleSetBase::OnCharacterRemovedFromEncounter(
	const TScriptInterface<IPF2CharacterInterface>& Character)
{
	const TWeakObjectPtr<AActor> CharacterPtr = Character->ToActor();

	if (this->ConditionCallbackHandles.Contains(CharacterPtr))
	{
		this->UnregisterAllTagCallbacksForCharacter(CharacterPtr);
		this->ConditionCallbackHandles.Remove(CharacterPtr);
	}
	else
	{
		UE_LOG(
			LogPf2CoreEncounters,
			Error,
			TEXT("OnCharacterRemovedFromEncounter() was invoked with character ('%s') that had no callbacks registered."),
			*(Character->GetIdForLogs())
		);
	}

	this->BP_OnCharacterRemovedFromEncounter(Character);
}

void APF2ModeOfPlayRuleSetBase::OnModeOfPlayEnd(const EPF2ModeOfPlayType ModeOfPlay)
{
	for (const auto& [CharacterPtr, _] : this->ConditionCallbackHandles)
	{
		this->UnregisterAllTagCallbacksForCharacter(CharacterPtr);
	}

	this->ConditionCallbackHandles.Empty();

	this->BP_OnModeOfPlayEnd(ModeOfPlay);
}

EPF2CommandExecuteOrQueueResult APF2ModeOfPlayRuleSetBase::AttemptToExecuteOrQueueCommand_Implementation(
	const TScriptInterface<IPF2CharacterCommandInterface>& Command)
{
	EPF2CommandExecuteOrQueueResult Result;

	// By default there is no queue, so we perform the command immediately.
	Result = UPF2CharacterCommandLibrary::ImmediateResultToExecuteOrQueueResult(Command->AttemptExecuteImmediately());

	return Result;
}

bool APF2ModeOfPlayRuleSetBase::AttemptToQueueCommand_Implementation(
	const TScriptInterface<IPF2CharacterCommandInterface>& Command)
{
	// By default there is no queue, so we drop the command.
	return false;
}

void APF2ModeOfPlayRuleSetBase::AttemptToCancelCommand_Implementation(
	const TScriptInterface<IPF2CharacterCommandInterface>& Command)
{
	TScriptInterface<IPF2CharacterInterface>    Character;
	TScriptInterface<IPF2CommandQueueInterface> CommandQueue;

	if (Command.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("AttemptToCancelCommand(): Command cannot be null.")
		);

		return;
	}

	Character = Command->GetOwningCharacter();

	if (Character.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("AttemptToCancelCommand(%s): Command has null target character."),
			*(Command->GetIdForLogs())
		);

		return;
	}

	CommandQueue = Character->GetCommandQueueComponent();

	if (Character.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Verbose,
			TEXT("AttemptToCancelCommand(%s): Character ('%s') has no command queue component -- nothing to cancel."),
			*(Command->GetIdForLogs()),
			*(Character->GetIdForLogs())
		);

		return;
	}

	// Default implementation -- remove the command from the character's command queue, if one exists.
	CommandQueue->Remove(Command);
}

void APF2ModeOfPlayRuleSetBase::RegisterTagCallback(
	const TWeakObjectPtr<AActor>      CharacterPtr,
	const FGameplayTag&               Tag,
	void (APF2ModeOfPlayRuleSetBase::*OnTagAdded)(const TScriptInterface<IPF2CharacterInterface>&),
	void (APF2ModeOfPlayRuleSetBase::*OnTagRemoved)(const TScriptInterface<IPF2CharacterInterface>&))
{
	if ((OnTagAdded == nullptr) && (OnTagRemoved == nullptr))
	{
		UE_LOG(
			LogPf2Core,
			Error,
			TEXT("RegisterTagCallback() was invoked null pointers for both callbacks for tag ('%s'), so nothing was bound."),
			*(Tag.ToString())
		);
	}
	else
	{
		AActor*                 CharacterActor = CharacterPtr.Get();
		IPF2CharacterInterface* CharacterIntf  = Cast<IPF2CharacterInterface>(CharacterActor);

		// Actor might have been garbage collected since they were originally added for tracking by this MoPRS.
		if (CharacterIntf != nullptr)
		{
			UAbilitySystemComponent*            CharacterAsc = CharacterIntf->GetAbilitySystemComponent();
			TMap<FGameplayTag, FDelegateHandle> CallbacksForCharacter;

			if (this->ConditionCallbackHandles.Contains(CharacterPtr))
			{
				CallbacksForCharacter = this->ConditionCallbackHandles[CharacterPtr];
			}
			else
			{
				CallbacksForCharacter = TMap<FGameplayTag, FDelegateHandle>();
			}

			if (CallbacksForCharacter.Contains(Tag))
			{
				UE_LOG(
					LogPf2Core,
					Error,
					TEXT("RegisterTagCallback() was invoked with character ('%s') that already has a condition callback registered for tag ('%s')."),
					*(CharacterIntf->GetIdForLogs()),
					*(Tag.ToString())
				);
			}
			else
			{
				TScriptInterface<IPF2CharacterInterface> Character =
					PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

				CallbacksForCharacter.Add(
					Tag,
					CharacterAsc->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddLambda(
						[this, OnTagAdded, OnTagRemoved, Character](const FGameplayTag, const int32 NewCount)
						{
							if (NewCount > 0)
							{
								if (OnTagAdded != nullptr)
								{
									(this->*OnTagAdded)(Character);
								}
							}
							else
							{
								if (OnTagRemoved != nullptr)
								{
									(this->*OnTagRemoved)(Character);
								}
							}
						}
					)
				);

				this->ConditionCallbackHandles.Add(CharacterPtr, CallbacksForCharacter);
			}
		}
	}
}

void APF2ModeOfPlayRuleSetBase::UnregisterAllTagCallbacksForCharacter(
	const TWeakObjectPtr<AActor> CharacterPtr)
{
	if (CharacterPtr.IsValid())
	{
		TMap<FGameplayTag, FDelegateHandle>& CallbacksForCharacter = this->ConditionCallbackHandles[CharacterPtr];

		// Iterate over a copy since UnregisterTagCallback() modifies the map.
		for (const auto& [Tag, CallbackHandle] : TMap(CallbacksForCharacter))
		{
			this->UnregisterTagCallback(CharacterPtr, Tag, CallbackHandle);
		}

		CallbacksForCharacter.Empty();
	}
}

void APF2ModeOfPlayRuleSetBase::UnregisterTagCallback(
	const TWeakObjectPtr<AActor> CharacterPtr,
	const FGameplayTag&          Tag,
	const FDelegateHandle&       DelegateHandle)
{
	AActor*                       CharacterActor = CharacterPtr.Get();
	const IPF2CharacterInterface* Character      = Cast<IPF2CharacterInterface>(CharacterActor);

	// Actor might have been garbage collected since they were originally added for tracking by this MoPRS.
	if (Character != nullptr)
	{
		if (this->ConditionCallbackHandles.Contains(CharacterPtr))
		{
			TMap<FGameplayTag, FDelegateHandle>& CallbacksForCharacter = this->ConditionCallbackHandles[CharacterPtr];

			if (CallbacksForCharacter.Contains(Tag))
			{
				UAbilitySystemComponent* CharacterAsc = Character->GetAbilitySystemComponent();

				CharacterAsc->UnregisterGameplayTagEvent(DelegateHandle, Tag, EGameplayTagEventType::NewOrRemoved);
				CallbacksForCharacter.Remove(Tag);
			}
			else
			{
				UE_LOG(
					LogPf2Core,
					Error,
					TEXT("UnregisterTagCallback() was invoked with character ('%s') that had no callbacks registered for tag ('%s')."),
					*(Character->GetIdForLogs()),
					*(Tag.ToString())
				);
			}
		}
		else
		{
			UE_LOG(
				LogPf2Core,
				Error,
				TEXT("UnregisterTagCallback() was invoked with character ('%s') that had no callbacks registered."),
				*(Character->GetIdForLogs())
			);
		}
	}
}

void APF2ModeOfPlayRuleSetBase::Native_OnCharacterUnconscious(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	this->BP_OnCharacterUnconscious(Character);
}

void APF2ModeOfPlayRuleSetBase::Native_OnCharacterConscious(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	this->BP_OnCharacterConscious(Character);
}

void APF2ModeOfPlayRuleSetBase::Native_OnCharacterDying(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	this->BP_OnCharacterDying(Character);
}

void APF2ModeOfPlayRuleSetBase::Native_OnCharacterRecoveredFromDying(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	this->BP_OnCharacterRecoveredFromDying(Character);
}

void APF2ModeOfPlayRuleSetBase::Native_OnCharacterDead(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	this->BP_OnCharacterDead(Character);
}

TScriptInterface<IPF2GameModeInterface> APF2ModeOfPlayRuleSetBase::GetGameMode() const
{
	AGameModeBase* GameMode = this->GetWorld()->GetAuthGameMode();

	check(GameMode != nullptr);

	return TScriptInterface<IPF2GameModeInterface>(GameMode);
}

TArray<TScriptInterface<IPF2PlayerControllerInterface>> APF2ModeOfPlayRuleSetBase::GetPlayerControllers() const
{
	return UPF2CharacterLibrary::GetPlayerControllers(this->GetWorld());
}

TArray<TScriptInterface<IPF2CharacterInterface>> APF2ModeOfPlayRuleSetBase::GetPlayerControlledCharacters() const
{
	return UPF2CharacterLibrary::GetPlayerControlledCharacters(this->GetWorld());
}

void APF2ModeOfPlayRuleSetBase::AddCharacterToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	this->OnCharacterAddedToEncounter(Character);
}

void APF2ModeOfPlayRuleSetBase::AddAllPlayerControlledCharactersToEncounter()
{
	for (const TScriptInterface<IPF2CharacterInterface>& Character : this->GetPlayerControlledCharacters())
	{
		this->AddCharacterToEncounter(Character);
	}
}

void APF2ModeOfPlayRuleSetBase::RemoveCharacterFromEncounter(
	const TScriptInterface<IPF2CharacterInterface> Character)
{
	this->OnCharacterRemovedFromEncounter(Character);
}
