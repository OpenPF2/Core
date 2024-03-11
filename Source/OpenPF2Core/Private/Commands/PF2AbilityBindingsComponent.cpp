// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Commands/PF2AbilityBindingsComponent.h"

#include <AbilitySystemComponent.h>

#include <Components/InputComponent.h>

#include "PF2CharacterInterface.h"
#include "PF2PlayerControllerInterface.h"

#include "Abilities/PF2InteractableAbilityInterface.h"

#include "Commands/PF2AbilityExecutionFilterContext.h"
#include "Commands/PF2AbilityExecutionFilterInterface.h"
#include "Commands/PF2AbilityInputBinding.h"
#include "Commands/PF2CharacterCommand.h"

#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

UObject* UPF2AbilityBindingsComponent::GetGenericEventsObject() const
{
	return this->GetEvents();
}

UPF2AbilityBindingsInterfaceEvents* UPF2AbilityBindingsComponent::GetEvents() const
{
	if (this->Events == nullptr)
	{
		// BUGBUG: This has to be instantiated here rather than via CreateDefaultSubobject() in the constructor, or it
		// breaks multiplayer. It seems that when created in the constructor, this component ends up as part of the CDO
		// and then all instances of this component share *one* events object, leading to all game clients being
		// notified about every multicast event broadcast for all instances. This typically results in a crash since the
		// addresses of callbacks aren't valid for the player controllers that don't own the component handling the
		// event.
		this->Events = NewObject<UPF2AbilityBindingsInterfaceEvents>(
			const_cast<UPF2AbilityBindingsComponent*>(this),
			FName(TEXT("InterfaceEvents"))
		);
	}

	return this->Events;
}

TMap<UInputAction*, TScriptInterface<IPF2InteractableAbilityInterface>> UPF2AbilityBindingsComponent::GetBindingsMap() const
{
	UAbilitySystemComponent*         Asc         = this->GetOwningCharacter()->GetAbilitySystemComponent();
	TArray<UPF2AbilityInputBinding*> AllBindings;

	this->Bindings.GenerateValueArray(AllBindings);

	return PF2ArrayUtilities::Reduce(
		AllBindings,
		TMap<UInputAction*, TScriptInterface<IPF2InteractableAbilityInterface>>(),
		[Asc](TMap<UInputAction*, TScriptInterface<IPF2InteractableAbilityInterface>> ResultMap,
		      const UPF2AbilityInputBinding*                                          CurrentBinding)
		{
			const FGameplayAbilitySpec* AbilitySpec = Asc->FindAbilitySpecFromHandle(CurrentBinding->AbilitySpecHandle);

			if (AbilitySpec != nullptr)
			{
				IPF2InteractableAbilityInterface* Ability =
					Cast<IPF2InteractableAbilityInterface>(AbilitySpec->Ability);

				if (Ability != nullptr)
				{
					ResultMap.Add(
						CurrentBinding->Action,
						PF2InterfaceUtilities::ToScriptInterface<IPF2InteractableAbilityInterface>(Ability)
					);
				}
			}

			return ResultMap;
		}
	);
}

void UPF2AbilityBindingsComponent::SetBinding(UInputAction* Action, const FGameplayAbilitySpec& AbilitySpec)
{
	this->SetBindingWithoutBroadcast(Action, AbilitySpec);
	this->Native_OnBindingsChanged();
}

void UPF2AbilityBindingsComponent::ClearBindings()
{
	if (this->IsConnectedToInput())
	{
		for (auto PairIterator = this->Bindings.CreateIterator(); PairIterator; ++PairIterator)
		{
			UPF2AbilityInputBinding* Binding = PairIterator.Value();

			this->DisconnectBindingFromInput(Binding);
		}
	}

	this->Bindings.Empty();
	this->Native_OnBindingsChanged();
}

void UPF2AbilityBindingsComponent::ClearBinding(const UInputAction* Action)
{
	if (this->Bindings.Contains(Action))
	{
		this->DisconnectBindingFromInput(this->Bindings[Action]);
		this->Bindings.Remove(Action);
		this->Native_OnBindingsChanged();
	}
}

void UPF2AbilityBindingsComponent::LoadAbilitiesFromCharacter()
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

	for (const FPF2InputActionMapping& Mapping : this->GetDefaultAbilityMappings())
	{
		const UGameplayAbility* TargetAbility = Mapping.GetAbility();
		bool                    bFoundAbility = false;

		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities)
		{
			const UGameplayAbility* Ability = AbilitySpec.Ability;

			if (TargetAbility == Ability)
			{
				const IPF2InteractableAbilityInterface* AbilityIntf = Cast<IPF2InteractableAbilityInterface>(Ability);

				if (AbilityIntf == nullptr)
				{
					UE_LOG(
						LogPf2CoreInput,
						Warning,
						TEXT("[%s] Ability ('%s') does not implement IPF2InteractableAbilityInterface."),
						*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
						*(GetNameSafe(Ability))
					);
				}
				else
				{
					UInputAction* DefaultAction = Mapping.GetInputAction();

					this->SetBindingWithoutBroadcast(DefaultAction, AbilitySpec);

					++NumMappedAbilities;
				}

				bFoundAbility = true;
			}
		}

		if (!bFoundAbility)
		{
			UE_LOG(
				LogPf2CoreInput,
				Warning,
				TEXT("[%s] Character ('%s') has not been granted the ability ('%s') that is bound to input action ('%s')."),
				*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
				*(Character->GetIdForLogs()),
				*(GetNameSafe(TargetAbility)),
				*(GetNameSafe(Mapping.GetInputAction()))
			);
		}
	}

	UE_LOG(
		LogPf2CoreInput,
		VeryVerbose,
		TEXT("[%s] Loaded input bindings for %d abilities from TargetCharacter ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		NumMappedAbilities,
		*(Character->GetIdForLogs())
	);

	if (this->IsConnectedToInput())
	{
		// Wire up all the new bindings.
		this->ConnectToInput(this->GetInputComponent());
	}

	this->Native_OnBindingsChanged();
}

void UPF2AbilityBindingsComponent::ConnectToInput(UEnhancedInputComponent* NewInputComponent)
{
	check(NewInputComponent != nullptr);
	checkf(
		!this->IsConnectedToInput() || (this->InputComponent == NewInputComponent),
		TEXT("Command bindings cannot be wired-up to two different input components at the same time.")
	);

	for (auto PairIterator = this->Bindings.CreateIterator(); PairIterator; ++PairIterator)
	{
		UPF2AbilityInputBinding* Binding = PairIterator.Value();

		Binding->ConnectToInput(NewInputComponent);
	}

	this->InputComponent = NewInputComponent;

	this->Native_OnInputConnected();
}

void UPF2AbilityBindingsComponent::DisconnectFromInput()
{
	if (this->IsConnectedToInput())
	{
		for (auto PairIterator = this->Bindings.CreateIterator(); PairIterator; ++PairIterator)
		{
			UPF2AbilityInputBinding* Binding = PairIterator.Value();

			Binding->DisconnectFromInput(this->InputComponent);
		}

		this->InputComponent = nullptr;

		this->Native_OnInputDisconnected();
	}
}

void UPF2AbilityBindingsComponent::ExecuteBoundAbility(const UInputAction*              Action,
                                                       const FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	IPF2CharacterInterface*                         CharacterIntf         = this->GetOwningCharacter();
	TScriptInterface<IPF2CharacterInterface>        Character;
	TScriptInterface<IPF2PlayerControllerInterface> PlayerController;
	FGameplayAbilitySpecHandle                      FilteredAbilityHandle = AbilitySpecHandle;

	FGameplayEventData FilteredAbilityPayload =
		this->BuildPayloadForAbilityActivation(AbilitySpecHandle);

	check(CharacterIntf != nullptr);

	Character        = PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);
	PlayerController = CharacterIntf->GetPlayerController();

	check(PlayerController.GetInterface() != nullptr);

	if (this->FilterAbilityActivation(Action, Character, FilteredAbilityHandle, FilteredAbilityPayload))
	{
		PlayerController->Server_ExecuteAbilitySpecAsCharacterCommandWithPayload(
			FilteredAbilityHandle,
			CharacterIntf->ToActor(),
			FilteredAbilityPayload
		);
	}
}

UActorComponent* UPF2AbilityBindingsComponent::ToActorComponent()
{
	return this;
}

IPF2CharacterInterface* UPF2AbilityBindingsComponent::GetOwningCharacter() const
{
	AActor*                 OwningActor;
	IPF2CharacterInterface* OwningCharacter;

	OwningActor = this->GetOwner();
	check(OwningActor != nullptr);

	OwningCharacter = Cast<IPF2CharacterInterface>(OwningActor);
	checkf(OwningCharacter != nullptr, TEXT("Owning character must implement IPF2CharacterInterface."));

	return OwningCharacter;
}

void UPF2AbilityBindingsComponent::SetBindingWithoutBroadcast(
	UInputAction*               Action,
	const FGameplayAbilitySpec& AbilitySpec)
{
	UPF2AbilityInputBinding* NewBinding = NewObject<UPF2AbilityInputBinding>(this);

	if (this->Bindings.Contains(Action))
	{
		// Disconnect the old binding before replacing it.
		this->DisconnectBindingFromInput(this->Bindings[Action]);
	}

	NewBinding->Initialize(Action, AbilitySpec, this);

	// If we are already connected to input, ensure the new binding is as well.
	if (this->IsConnectedToInput())
	{
		NewBinding->ConnectToInput(this->GetInputComponent());
	}

	this->Bindings.Add(Action, NewBinding);
}

void UPF2AbilityBindingsComponent::DisconnectBindingFromInput(UPF2AbilityInputBinding* Binding) const
{
	return Binding->DisconnectFromInput(this->GetInputComponent());
}

FGameplayEventData UPF2AbilityBindingsComponent::BuildPayloadForAbilityActivation(
	const FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	return FGameplayEventData();
}

bool UPF2AbilityBindingsComponent::FilterAbilityActivation(
	const UInputAction*                            InAction,
	const TScriptInterface<IPF2CharacterInterface> InCharacter,
	FGameplayAbilitySpecHandle&                    InOutAbilitySpecHandle,
	FGameplayEventData&                            InOutAbilityPayload)
{
	FPF2AbilityExecutionFilterContext FilterContext =
		FPF2AbilityExecutionFilterContext(InAction, InCharacter, InOutAbilitySpecHandle, InOutAbilityPayload);

	for (const TSubclassOf<UObject> FilterType : this->Filters)
	{
		UObject*                                   RawFilter = FilterType.GetDefaultObject();
		const IPF2AbilityExecutionFilterInterface* Filter    = Cast<IPF2AbilityExecutionFilterInterface>(RawFilter);

		if (Filter == nullptr)
		{
			UE_LOG(
				LogPf2CoreInput,
				Error,
				TEXT("Command bindings component ('%s') has a null ability execution filter."),
				*(this->GetIdForLogs())
			);
		}
		else
		{
			UE_LOG(
				LogPf2CoreInput,
				VeryVerbose,
				TEXT("[%s] [%s] BEFORE applying filter ('%s') - Ability handle: %s."),
				*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
				*(this->GetIdForLogs()),
				*(Filter->GetIdForLogs()),
				*(FilterContext.GetHandleOfAbilityToExecute().ToString())
			);

			FilterContext = Filter->Execute_FilterAbilityActivation(RawFilter, FilterContext);

			UE_LOG(
				LogPf2CoreInput,
				VeryVerbose,
				TEXT("[%s] [%s] AFTER applying filter ('%s') - Proceed: %s, Ability handle: %s."),
				*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
				*(this->GetIdForLogs()),
				*(Filter->GetIdForLogs()),
				(FilterContext.ShouldProceed() ? TEXT("true") : TEXT("false")),
				*(FilterContext.GetHandleOfAbilityToExecute().ToString())
			);

			if (!FilterContext.ShouldProceed())
			{
				// The last filter vetoed execution, so let's call the whole thing off.
				return false;
			}
		}
	}

	InOutAbilitySpecHandle = FilterContext.GetHandleOfAbilityToExecute();
	InOutAbilityPayload    = FilterContext.GetAbilityPayload();

	return true;
}

void UPF2AbilityBindingsComponent::Native_OnBindingsChanged()
{
	const FPF2AbilityBindingsChangedDelegate BindingsChangedDelegate = this->GetEvents()->OnAbilityBindingsChanged;

	if (BindingsChangedDelegate.IsBound())
	{
		BindingsChangedDelegate.Broadcast(this);
	}
}

void UPF2AbilityBindingsComponent::Native_OnInputConnected()
{
	const FPF2AbilityBindingsInputConnectionChangedDelegate& OnInputConnected = this->GetEvents()->OnInputConnected;

	UE_LOG(
		LogPf2CoreInput,
		Verbose,
		TEXT("[%s] Command bindings component ('%s') connected to input."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetIdForLogs())
	);

	if (OnInputConnected.IsBound())
	{
		OnInputConnected.Broadcast(this);
	}
}

void UPF2AbilityBindingsComponent::Native_OnInputDisconnected()
{
	const FPF2AbilityBindingsInputConnectionChangedDelegate& OnInputDisconnected =
		this->GetEvents()->OnInputDisconnected;

	UE_LOG(
		LogPf2CoreInput,
		Verbose,
		TEXT("[%s] Command bindings component ('%s') disconnected from input."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetIdForLogs())
	);

	if (OnInputDisconnected.IsBound())
	{
		OnInputDisconnected.Broadcast(this);
	}
}
