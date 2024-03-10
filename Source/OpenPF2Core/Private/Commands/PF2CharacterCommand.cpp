// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Commands/PF2CharacterCommand.h"

#include <GameFramework/GameModeBase.h>

#include <Kismet/GameplayStatics.h>

#include <Net/UnrealNetwork.h>

#include "PF2GameModeInterface.h"
#include "PF2PlayerControllerInterface.h"

#include "Abilities/PF2InteractableAbilityInterface.h"

#include "Actors/Components/PF2AbilitySystemInterface.h"

#include "Utilities/PF2EnumUtilities.h"
#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

IPF2CharacterCommandInterface* APF2CharacterCommand::Create(AActor*                          OwningCharacterActor,
                                                            const FGameplayAbilitySpecHandle AbilitySpecHandle,
                                                            const FGameplayEventData&        AbilityPayload,
                                                            const EPF2CommandQueuePosition   QueuePositionPreference)
{
	UWorld*               World   = OwningCharacterActor->GetWorld();
	APF2CharacterCommand* Command;

	check(OwningCharacterActor->Implements<UPF2CharacterInterface>());

	Command = World->SpawnActorDeferred<APF2CharacterCommand>(StaticClass(), FTransform(), OwningCharacterActor);

	Command->FinalizeConstruction(OwningCharacterActor, AbilitySpecHandle, AbilityPayload, QueuePositionPreference);

	return Command;
}

void APF2CharacterCommand::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APF2CharacterCommand, OwningCharacter);
	DOREPLIFETIME(APF2CharacterCommand, AbilitySpecHandle);
	DOREPLIFETIME(APF2CharacterCommand, AbilityPayload);
	DOREPLIFETIME(APF2CharacterCommand, QueuePositionPreference);
}

TScriptInterface<IPF2CharacterInterface> APF2CharacterCommand::GetOwningCharacter() const
{
	check(this->OwningCharacter);
	check(this->OwningCharacter->Implements<UPF2CharacterInterface>());

	return TScriptInterface<IPF2CharacterInterface>(this->OwningCharacter);
}

UTexture2D* APF2CharacterCommand::GetCommandIcon() const
{
	UTexture2D*                             CommandIcon = nullptr;
	const IPF2InteractableAbilityInterface* AbilityIntf = this->GetAbilityIntf();

	if (AbilityIntf != nullptr)
	{
		CommandIcon = AbilityIntf->GetAbilityIcon();
	}

	return CommandIcon;
}

FText APF2CharacterCommand::GetCommandLabel() const
{
	FText                                   CommandLabel;
	const IPF2InteractableAbilityInterface* AbilityIntf = this->GetAbilityIntf();

	if (AbilityIntf == nullptr)
	{
		CommandLabel = FText();
	}
	else
	{
		CommandLabel = AbilityIntf->GetAbilityLabel();
	}

	return CommandLabel;
}

FText APF2CharacterCommand::GetCommandDescription() const
{
	FText                                   CommandDescription;
	const IPF2InteractableAbilityInterface* AbilityIntf = this->GetAbilityIntf();

	if (AbilityIntf == nullptr)
	{
		CommandDescription = FText();
	}
	else
	{
		CommandDescription = AbilityIntf->GetAbilityDescription();
	}

	return CommandDescription;
}

EPF2CommandQueuePosition APF2CharacterCommand::GetQueuePositionPreference() const
{
	return this->QueuePositionPreference;
}

EPF2CommandExecuteOrQueueResult APF2CharacterCommand::AttemptExecuteOrQueue()
{
	EPF2CommandExecuteOrQueueResult Result      = EPF2CommandExecuteOrQueueResult::None;
	const UWorld* const             World       = this->GetWorld();
	IPF2GameModeInterface*          PF2GameMode;

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] AttemptExecuteOrQueue() called on command ('%s')."),
		*(PF2LogUtilities::GetHostNetId(World)),
		*(this->GetIdForLogs())
	);

	check(World);

	PF2GameMode = Cast<IPF2GameModeInterface>(World->GetAuthGameMode());

	if (PF2GameMode == nullptr)
	{
		// TODO: If we have an actual need for this use case, we could probably support it by routing the invocation
		// through the local player controller. The reason this isn't supported at the moment is because character
		// commands have to be spawned on the server to have any effect, so we create them and invoke them there, and
		// therefore don't (yet) have a need to invoke them locally.
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("[%s] AttemptExecuteOrQueue() can only be called on the server."),
			*(PF2LogUtilities::GetHostNetId(World))
		);
	}
	else
	{
		const TScriptInterface<IPF2CharacterCommandInterface> CommandIntf =
			PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterCommandInterface>(this);

		Result = PF2GameMode->AttemptToExecuteOrQueueCommand(CommandIntf);
	}

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] AttemptExecuteOrQueue() result for command ('%s'): %s."),
		*(PF2LogUtilities::GetHostNetId(World)),
		*(this->GetIdForLogs()),
		*(PF2EnumUtilities::ToString(Result))
	);

	return Result;
}

EPF2CommandExecuteImmediatelyResult APF2CharacterCommand::AttemptExecuteImmediately()
{
	EPF2CommandExecuteImmediatelyResult Result  = EPF2CommandExecuteImmediatelyResult::None;
	IPF2AbilitySystemInterface*         AscIntf = this->GetAbilitySystemComponent();

	if (AscIntf != nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			VeryVerbose,
			TEXT("[%s] AttemptExecuteImmediately() called on command ('%s')."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(this->GetIdForLogs())
		);

		FGameplayEventData Payload = this->GetAbilityPayload();

		// Ensure there is a source for the attack if none was supplied by the command itself.
		if (!IsValid(Payload.Instigator))
		{
			const TScriptInterface<IPF2CharacterInterface> InstigatorCharacter = this->GetOwningCharacter();
			FGameplayTagContainer                          InstigatorTags;

			InstigatorCharacter->GetAbilitySystemComponent()->GetOwnedGameplayTags(InstigatorTags);

			Payload.Instigator = InstigatorCharacter->ToActor();
			Payload.InstigatorTags.AppendTags(InstigatorTags);
		}

		if (AscIntf->TriggerAbilityWithPayload(this->GetAbilitySpecHandle(), Payload))
		{
			Result = EPF2CommandExecuteImmediatelyResult::Activated;
		}
		else
		{
			Result = EPF2CommandExecuteImmediatelyResult::Blocked;
		}

		UE_LOG(
			LogPf2CoreAbilities,
			VeryVerbose,
			TEXT("[%s] AttemptExecuteImmediately() result for command ('%s'): %s."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(this->GetIdForLogs()),
			*(PF2EnumUtilities::ToString(Result))
		);
	}

	return Result;
}

bool APF2CharacterCommand::AttemptQueue()
{
	bool                   bWasQueued = false;
	const UWorld* const    World      = this->GetWorld();
	IPF2GameModeInterface* PF2GameMode;

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] AttemptQueue() called on command ('%s')."),
		*(PF2LogUtilities::GetHostNetId(World)),
		*(this->GetIdForLogs())
	);

	check(World);

	PF2GameMode = Cast<IPF2GameModeInterface>(World->GetAuthGameMode());

	if (PF2GameMode == nullptr)
	{
		// TODO: If we have an actual need for this use case, we could probably support it by routing the invocation
		// through the local player controller. The reason this isn't supported at the moment is because character
		// commands have to be spawned on the server to have any effect, so we create them and invoke them there, and
		// therefore don't (yet) have a need to invoke them locally.
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("[%s] AttemptQueue() can only be called on the server."),
			*(PF2LogUtilities::GetHostNetId(World))
		);
	}
	else
	{
		const TScriptInterface<IPF2CharacterCommandInterface> CommandIntf =
			PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterCommandInterface>(this);

		bWasQueued = PF2GameMode->AttemptToQueueCommand(CommandIntf);
	}

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] AttemptQueue() result for command ('%s'): %s."),
		*(PF2LogUtilities::GetHostNetId(World)),
		*(this->GetIdForLogs()),
		bWasQueued ? TEXT("true") : TEXT("false")
	);

	return bWasQueued;
}

void APF2CharacterCommand::FinalizeConstruction(AActor*                          InOwningCharacter,
                                                const FGameplayAbilitySpecHandle InAbilitySpecHandle,
                                                const FGameplayEventData&        InAbilityPayload,
                                                const EPF2CommandQueuePosition   InQueuePositionPreference)
{
	this->OwningCharacter         = InOwningCharacter;
	this->AbilitySpecHandle       = InAbilitySpecHandle;
	this->AbilityPayload          = InAbilityPayload;
	this->QueuePositionPreference = InQueuePositionPreference;

	UGameplayStatics::FinishSpawningActor(this, FTransform());
}

void APF2CharacterCommand::Cancel_WithRemoteServer()
{
	const TScriptInterface<IPF2CharacterInterface>        Character        = this->GetOwningCharacter();
	const TScriptInterface<IPF2PlayerControllerInterface> PlayerController = Character->GetPlayerController();

	if (PlayerController.GetInterface() == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Character ('%s') is not controllable by the local client and/or does not belong to an OpenPF2-compatible player controller; unable to cancel command ('%s')."),
			*(Character->GetIdForLogs()),
			*(this->GetIdForLogs())
		);
	}
	else
	{
		PlayerController->Server_CancelCharacterCommand(this);
	}
}

void APF2CharacterCommand::Cancel_WithLocalServer()
{
	const TScriptInterface<IPF2CharacterInterface> Character    = this->GetOwningCharacter();
	AGameModeBase*                                 GameMode     = this->GetWorld()->GetAuthGameMode();
	IPF2GameModeInterface*                         GameModeIntf = Cast<IPF2GameModeInterface>(GameMode);

	if (GameModeIntf == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Game Mode ('%s') is not OpenPF2-compatible; unable to cancel command ('%s') for character ('%s')."),
			*(GetNameSafe(GameMode)),
			*(this->GetIdForLogs()),
			*(Character->GetIdForLogs())
		);
	}
	else
	{
		const TScriptInterface<IPF2CharacterCommandInterface> CommandIntf =
			PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterCommandInterface>(this);

		GameModeIntf->AttemptToCancelCommand(CommandIntf);
	}
}

void APF2CharacterCommand::AttemptCancel()
{
	if (this->GetWorld()->GetAuthGameMode() == nullptr)
	{
		// The game mode only exists on servers; if it does not exist, we must be running on a client.
		// Route the request through the local player controller so we can trigger this on the server.
		this->Cancel_WithRemoteServer();
	}
	else
	{
		// The game mode only exists on servers; if it exists, we must be running on the server.
		// We can notify the game mode directly.
		this->Cancel_WithLocalServer();
	}
}

AInfo* APF2CharacterCommand::ToActor()
{
	return this;
}

FString APF2CharacterCommand::GetIdForLogs() const
{
	const UGameplayAbility* WrappedAbility = this->GetAbility();

	// ReSharper disable CppRedundantParentheses
	return FString::Format(
		TEXT("{0}[{1}.{2}]"),
		{
			*(this->GetCommandLabel().ToString()),
			*(GetNameSafe(WrappedAbility)),
			*(this->GetName())
		}
	);
}

FGameplayAbilitySpec* APF2CharacterCommand::GetAbilitySpec() const
{
	FGameplayAbilitySpec*       AbilitySpec = nullptr;
	IPF2AbilitySystemInterface* AscIntf     = this->GetAbilitySystemComponent();

	if (AscIntf != nullptr)
	{
		const UAbilitySystemComponent*   Asc          = AscIntf->ToAbilitySystemComponent();
		const FGameplayAbilitySpecHandle TargetHandle = this->GetAbilitySpecHandle();
		const FString                    HostNetId    = PF2LogUtilities::GetHostNetId(this->GetWorld()),
		                                 AscId        = GetFullNameSafe(Asc),
		                                 HandleId     = TargetHandle.ToString();

		AbilitySpec = Asc->FindAbilitySpecFromHandle(TargetHandle);

		if (AbilitySpec == nullptr)
		{
			// FIXME: This warning gets emitted every time a command gets replicated to clients other than the client
			// who can control the character to which the command belongs. Ideally, there should be some way to make
			// commands only replicate from the server to the client that can control the character, but
			// bOnlyRelevantToOwner doesn't work here (yet?) because the "owner" of an AI-possessed character belong to
			// a player is not "owned" (in the net authority sense) by the controller for that player.
			UE_LOG(
				LogPf2CoreAbilities,
				Warning,
				TEXT("[%s] ASC ('%s') has no Gameplay Ability that matches handle ('%s')."),
				*HostNetId,
				*AscId,
				*HandleId
			);
		}
		else
		{
			UE_LOG(
				LogPf2CoreAbilities,
				VeryVerbose,
				TEXT("[%s] Found a Gameplay Ability ('%s') in the ASC ('%s') that matches the given handle ('%s')."),
				*HostNetId,
				*(AbilitySpec->GetDebugString()),
				*AscId,
				*HandleId
			);
		}
	}

	return AbilitySpec;
}

IPF2AbilitySystemInterface* APF2CharacterCommand::GetAbilitySystemComponent() const
{
	const TScriptInterface<IPF2CharacterInterface> CharacterIntf = this->GetOwningCharacter();
	UAbilitySystemComponent*                       Asc           = CharacterIntf->GetAbilitySystemComponent();
	IPF2AbilitySystemInterface*                    AscIntf       = Cast<IPF2AbilitySystemInterface>(Asc);

	if (AscIntf == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Warning,
			TEXT("[%s] Character ('%s') has no OpenPF2-compatible Ability System Component (ASC)."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(CharacterIntf->GetIdForLogs())
		);
	}

	return AscIntf;
}

UGameplayAbility* APF2CharacterCommand::GetAbility() const
{
	if (this->CachedAbility == nullptr)
	{
		const FGameplayAbilitySpec* AbilitySpec = this->GetAbilitySpec();
		UGameplayAbility*           Ability     = nullptr;

		if (AbilitySpec != nullptr)
		{
			Ability = AbilitySpec->Ability;
			check(Ability != nullptr);
		}

		this->CachedAbility = Ability;
	}

	return this->CachedAbility;
}
