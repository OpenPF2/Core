// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Components/ActorComponent.h>

#include <Containers/Map.h>

#include "PF2CharacterInterface.h"
#include "PF2EventEmitterInterface.h"

#include "Abilities/PF2InputActionMapping.h"

#include "Actors/Components/PF2ActorComponentBase.h"

#include "Commands/PF2AbilityBindingsInterface.h"
#include "Commands/PF2AbilityInputBinding.h"

#include "PF2AbilityBindingsComponent.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2AbilityExecutionFilterInterface;
class UEnhancedInputComponent;
class UInputAction;
class UPF2AbilityExecutionFilterBase;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * A component for characters that need to support binding execution of Gameplay Abilities to input actions.
 *
 * Unlike the native input binding support offered by the GAS, this version makes it possible to dynamically bind and
 * unbind input to abilities as needed, allowing input to be routed to whichever character the player is currently
 * controlling. This also works automatically with the character command system, ensuring that commands will be queued
 * during encounters if required by the MoPRS.
 */
UCLASS(ClassGroup="OpenPF2-Characters", meta=(BlueprintSpawnableComponent))
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2AbilityBindingsComponent :
	public UPF2ActorComponentBase,
	public IPF2EventEmitterInterface,
	public IPF2AbilityBindingsInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Fields
	// =================================================================================================================
	/**
	 * The default map of input actions to the abilities they trigger.
	 *
	 * This is used to pre-populate bindings for this component. If left blank, the owning character has no default
	 * bindings and must be assigned bindings at run-time. If populated, each binding can still be overridden at
	 * run-time (e.g., if you are writing a game in which the player can remap actions).
	 */
	UPROPERTY(
		EditDefaultsOnly,
		Category="OpenPF2 - Command Bindings",
		meta=(TitleProperty=InputAction)
	)
	TArray<FPF2InputActionMapping> DefaultAbilityMappings;

	/**
	 * Filters to apply to the activation of abilities through this component.
	 *
	 * This makes it possible to redirect an input action to one or more abilities depending upon the state of the game
	 * or something about the character. For example, consider an input action that works as follows:
	 * - While in exploration mode: the input action results in the character attempting their default attack.
	 * - While in encounter mode:
	 *   - If the character is targeting an empty square: the input action results in the character moving to the
	 *     square.
	 *   - If the character is targeting an square containing an enemy: the input action results in the character
	 *     attempting their default attack.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		meta=(MustImplement="/Script/OpenPF2Core.PF2AbilityExecutionFilterInterface"),
		DisplayName="Ability Execution Filters",
		Category="OpenPF2 - Command Bindings"
	)
	TArray<TSubclassOf<UObject>> Filters;

protected:
	/**
	 * The events object used for binding Blueprint callbacks to events from this component.
	 */
	UPROPERTY(Transient)
	mutable UPF2AbilityBindingsInterfaceEvents* Events;

private:
	/**
	 * The input component to which this component is currently wired.
	 *
	 * This is not replicated, since it is only of relevance to local clients.
	 */
	UPROPERTY(Transient)
	UEnhancedInputComponent* InputComponent;

	/**
	 * The association between input actions and Gameplay Abilities.
	 *
	 * This is not replicated because input is only a concern of the local client and not the server. Instead, abilities
	 * should be loaded by invoking LoadAbilitiesFromCharacter() after abilities have replicated through the ASC from
	 * the server. This is handled automatically when using the default OpenPF2 player controller implementation.
	 */
	UPROPERTY()
	TMap<const UInputAction*, UPF2AbilityInputBinding*> Bindings;

public:
	// =================================================================================================================
	// Public Constructor
	// =================================================================================================================
	/**
	 * Default constructor.
	 */
	explicit UPF2AbilityBindingsComponent() : Events(nullptr), InputComponent(nullptr)
	{
	}

	// =================================================================================================================
	// Public Methods - IPF2EventEmitterInterface Implementation
	// =================================================================================================================
	virtual UObject* GetGenericEventsObject() const override;

	// =================================================================================================================
	// Public Methods - IPF2AbilityBindingsInterface Implementation
	// =================================================================================================================
	virtual UPF2AbilityBindingsInterfaceEvents* GetEvents() const override;

	virtual TMap<UInputAction*, TScriptInterface<IPF2InteractableAbilityInterface>> GetBindingsMap() const override;

	virtual void SetBinding(UInputAction* Action, const FGameplayAbilitySpec& AbilitySpec) override;

	virtual void ClearBindings() override;

	virtual void ClearBinding(const UInputAction* Action) override;

	virtual void LoadAbilitiesFromCharacter() override;

	virtual void ConnectToInput(UEnhancedInputComponent* NewInputComponent) override;

	virtual void DisconnectFromInput() override;

	virtual void ExecuteBoundAbility(const UInputAction*              Action,
	                                 const FGameplayAbilitySpecHandle AbilitySpecHandle) override;

	// =================================================================================================================
	// Public Methods - IPF2ActorComponentInterface Implementation
	// =================================================================================================================
	virtual UActorComponent* ToActorComponent() override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	virtual FString GetIdForLogs() const override
	{
		return Super::GetIdForLogs();
	}

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the enhanced input component to which this bindings component is wired up to, if it is wired up.
	 *
	 * @return
	 *	- If this component is currently wired-up to input: the input component to which it is wired up.
	 *	- If this component is not currently wired-up to input: null.
	 */
	FORCEINLINE UEnhancedInputComponent* GetInputComponent() const
	{
		return this->InputComponent;
	}

	/**
	 * Gets whether this component is currently wired up to input.
	 *
	 * @return
	 *	- true if this component is currently wired up to input.
	 *	- false if it is not currently wired up to input.
	 */
	FORCEINLINE bool IsConnectedToInput() const
	{
		return this->GetInputComponent() != nullptr;
	}

	/**
	 * Gets the default map of input actions to the abilities they trigger.
	 *
	 * @return
	 *	The default bindings for this component.
	 */
	FORCEINLINE TArray<FPF2InputActionMapping> GetDefaultAbilityMappings() const
	{
		return this->DefaultAbilityMappings;
	}

	/**
	 * Gets the character of which this component is a part.
	 *
	 * @return
	 *	The character actor that owns this component.
	 */
	IPF2CharacterInterface* GetOwningCharacter() const;

	/**
	 * Binds an ability to a particular input action without notifying listeners.
	 *
	 * @param Action
	 *	The action to which the ability will be bound.
	 * @param AbilitySpec
	 *	The ability to bind.
	 */
	void SetBindingWithoutBroadcast(UInputAction* Action, const FGameplayAbilitySpec& AbilitySpec);

	/**
	 * Stops listening for input for the specified binding.
	 *
	 * @param Binding
	 *	The binding to disconnect from input.
	 */
	void DisconnectBindingFromInput(UPF2AbilityInputBinding* Binding) const;

	/**
	 * Builds a payload for an activation of the ability having the specified handle.
	 *
	 * This provides an opportunity for sub-classes to inject additional context (e.g., target data, snapshots of
	 * game context, etc.) about the activation. Since ability activation only happens on the server in OpenPF2, this
	 * method only gets invoked on the server.
	 *
	 * @param AbilitySpecHandle
	 *	The handle of the ability that is about to be activated or queued.
	 *
	 * @return
	 *	The gameplay event payload to pass to the ability when it is activated.
	 */
	 virtual FGameplayEventData BuildPayloadForAbilityActivation(const FGameplayAbilitySpecHandle AbilitySpecHandle);

	/**
	 * Applies ability execution filters to the activation of a bound ability.
	 *
	 * Filters may veto execution of the filter. If a filter does so, the result of this method will be "false".
	 *
	 * @param InAction
	 *	The input action that was invoked.
	 * @param InCharacter
	 *	The character on which the action will be performed.
	 * @param InOutAbilitySpecHandle
	 *	The handle for the ability to activate. This will be modified by filter execution.
	 * @param InOutAbilityPayload
	 *	The payload to pass to the activated ability. This will be modified by filter execution.
	 *
	 * @return
	 *	- "true" if the ability should proceed to be executed.
	 *	- "false" if the ability should not be executed because the last filter executed vetoed it.
	 */
	bool FilterAbilityActivation(
		const UInputAction*                            InAction,
		const TScriptInterface<IPF2CharacterInterface> InCharacter,
		FGameplayAbilitySpecHandle&                    InOutAbilitySpecHandle,
		FGameplayEventData&                            InOutAbilityPayload);

	// =================================================================================================================
	// Protected Native Event Callbacks
	// =================================================================================================================
	/**
	 * Callback invoked in C++ code when bindings are added, removed, or cleared.
	 *
	 * This notifies all event listeners that bindings have changed.
	 */
	virtual void Native_OnBindingsChanged();

	/**
	 * Callback invoked in C++ code when input has been connected to this component.
	 *
	 * This notifies all event listeners that input has been connected.
	 */
	virtual void Native_OnInputConnected();

	/**
	 * Callback invoked in C++ code when input has been disconnected from this component.
	 *
	 * This notifies all event listeners that input has been disconnected.
	 */
	virtual void Native_OnInputDisconnected();
};
