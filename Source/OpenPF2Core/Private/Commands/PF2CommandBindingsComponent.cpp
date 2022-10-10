﻿// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Commands/PF2CommandBindingsComponent.h"

#include <AbilitySystemComponent.h>

#include <Components/InputComponent.h>

#include "PF2CharacterInterface.h"
#include "PF2PlayerControllerInterface.h"

#include "Abilities/PF2GameplayAbilityInterface.h"

#include "Commands/PF2CharacterCommand.h"
#include "Commands/PF2CommandInputBinding.h"

#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

void UPF2CommandBindingsComponent::ClearBindings()
{
	if (this->IsConnectedToInput())
	{
		for (FPF2CommandInputBinding& Binding : this->Bindings)
		{
			Binding.DisconnectFromInput(this->GetInputComponent());
		}
	}

	this->Bindings.Empty();
}

void UPF2CommandBindingsComponent::LoadAbilitiesFromCharacter()
{
	const IPF2CharacterInterface* Character              = this->GetOwningCharacter();
	UAbilitySystemComponent*      AbilitySystemComponent = Character->GetAbilitySystemComponent();
	TArray<FGameplayAbilitySpec>  ActivatableAbilities   = AbilitySystemComponent->GetActivatableAbilities();
	int32                         NumMappedAbilities     = 0;

	checkf(
		this->Bindings.Num() == 0,
		TEXT("Abilities must be loaded from a character before custom bindings are added.")
	);

	UE_LOG(
		LogPf2CoreInput,
		VeryVerbose,
		TEXT("[%s] Loading %d abilities from TargetCharacter ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		ActivatableAbilities.Num(),
		*(Character->GetIdForLogs())
	);

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities)
	{
		const UGameplayAbility*             Ability       = AbilitySpec.Ability;
		const IPF2GameplayAbilityInterface* AbilityIntf   = Cast<IPF2GameplayAbilityInterface>(Ability);
		FName                               DefaultAction;

		if (AbilityIntf != nullptr)
		{
			DefaultAction = AbilityIntf->GetDefaultInputActionMapping();
			++NumMappedAbilities;
		}
		else
		{
			// Fallback for interoperability with non-OpenPF2 abilities.
			DefaultAction = FName();
		}

		this->Bindings.Add(FPF2CommandInputBinding(DefaultAction, AbilitySpec, this));
	}

	UE_LOG(
		LogPf2CoreInput,
		VeryVerbose,
		TEXT("[%s] Loaded %d abilities with default action mappings from TargetCharacter ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		NumMappedAbilities,
		*(Character->GetIdForLogs())
	);

	if (this->IsConnectedToInput())
	{
		// Wire up all the new bindings.
		this->ConnectToInput(this->GetInputComponent());
	}
}

void UPF2CommandBindingsComponent::ConnectToInput(UInputComponent* NewInputComponent)
{
	checkf(
		!this->IsConnectedToInput() || (this->InputComponent == NewInputComponent),
		TEXT("Command bindings cannot be wired-up to two different input components at the same time.")
	);

	for (FPF2CommandInputBinding& Binding : this->Bindings)
	{
		Binding.ConnectToInput(NewInputComponent);
	}

	this->InputComponent = NewInputComponent;

	this->Native_OnInputConnected();
}

void UPF2CommandBindingsComponent::DisconnectFromInput()
{
	if (this->IsConnectedToInput())
	{
		for (FPF2CommandInputBinding& Binding : this->Bindings)
		{
			Binding.DisconnectFromInput(this->InputComponent);
		}

		this->InputComponent = nullptr;
	}

	this->Native_OnInputDisconnected();
}

void UPF2CommandBindingsComponent::ExecuteBoundAbility(const FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	IPF2CharacterInterface*                               Character        = this->GetOwningCharacter();
	const TScriptInterface<IPF2PlayerControllerInterface> PlayerController = Character->GetPlayerController();

	check(PlayerController != nullptr);

	PlayerController->Server_ExecuteCharacterCommand(AbilitySpecHandle, Character->ToActor());
}

UActorComponent* UPF2CommandBindingsComponent::ToActorComponent()
{
	return this;
}

FString UPF2CommandBindingsComponent::GetIdForLogs() const
{
	// ReSharper disable CppRedundantParentheses
	return FString::Format(
		TEXT("{0}.{1}"),
		{
			*(GetNameSafe(this->GetOwner())),
			*(this->GetName())
		}
	);
}

IPF2CharacterInterface* UPF2CommandBindingsComponent::GetOwningCharacter() const
{
	AActor*                 OwningActor;
	IPF2CharacterInterface* OwningCharacter;

	OwningActor = this->GetOwner();
	check(OwningActor != nullptr);

	OwningCharacter = Cast<IPF2CharacterInterface>(OwningActor);
	checkf(OwningCharacter != nullptr, TEXT("Owning character must implement IPF2CharacterInterface."));

	return OwningCharacter;
}

void UPF2CommandBindingsComponent::Native_OnInputConnected()
{
	UE_LOG(
		LogPf2CoreInput,
		Verbose,
		TEXT("[%s] Command bindings component ('%s') connected to input."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetIdForLogs())
	);

	this->OnInputConnected.Broadcast();
}

void UPF2CommandBindingsComponent::Native_OnInputDisconnected()
{
	UE_LOG(
		LogPf2CoreInput,
		Verbose,
		TEXT("[%s] Command bindings component ('%s') disconnected from input."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetIdForLogs())
	);

	this->OnInputDisconnected.Broadcast();
}
