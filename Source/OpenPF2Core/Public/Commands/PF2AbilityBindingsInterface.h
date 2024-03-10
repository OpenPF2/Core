// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayAbilitySpec.h>
#include <UObject/ScriptInterface.h>

#include "Actors/Components/PF2ActorComponentInterface.h"

#include "PF2AbilityBindingsInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2AbilityBindingsInterface;
class IPF2CharacterInterface;
class IPF2InteractableAbilityInterface;
class UEnhancedInputComponent;
class UInputComponent;
class UInputAction;
class UPF2AbilityInputBinding;

// =====================================================================================================================
// Normal Declarations - Delegates
// =====================================================================================================================
/**
 * Delegate for Blueprints to react to a bindings component getting wired up to input or disconnected from input.
 *
 * @param BindingsComponent
 *	The component broadcasting this event.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FPF2AbilityBindingsInputConnectionChangedDelegate,
	const TScriptInterface<IPF2AbilityBindingsInterface>&, BindingsComponent
);

/**
 * Delegate for reacting to command bindings changing/being rebound.
 *
 * @param BindingsComponent
 *	The component broadcasting this event.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FPF2AbilityBindingsChangedDelegate,
	const TScriptInterface<IPF2AbilityBindingsInterface>&, BindingsComponent
);

// =====================================================================================================================
// Normal Declarations - Types
// =====================================================================================================================
/**
 * The "Events" object for PF2AbilityBindingsInterface.
 *
 * This is a concrete UObject that contains only the dynamic multicast delegates that instances of the interface expose
 * to consumers for binding.
 *
 * @see IPF2EventEmitterInterface
 */
UCLASS()
class OPENPF2CORE_API UPF2AbilityBindingsInterfaceEvents : public UObject
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Fields - Multicast Delegates
	// =================================================================================================================
	/**
	 * Event fired when ability bindings in the owning component change (e.g. abilities are bound to different inputs).
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Ability Bindings")
	FPF2AbilityBindingsChangedDelegate OnAbilityBindingsChanged;

	/**
	 * Event fired when local input is connected to this component.
	 *
	 * This event is only fired on clients.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Ability Bindings")
	FPF2AbilityBindingsInputConnectionChangedDelegate OnInputConnected;

	/**
	 * Event fired when local input is disconnected from this component.
	 *
	 * This event is only fired on clients.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Ability Bindings")
	FPF2AbilityBindingsInputConnectionChangedDelegate OnInputDisconnected;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2AbilityBindingsInterface : public UPF2ActorComponentInterface
{
	GENERATED_BODY()
};

/**
 * An interface for components that maintain Gameplay Ability input action bindings for characters.
 */
class OPENPF2CORE_API IPF2AbilityBindingsInterface : public IPF2ActorComponentInterface
{
	GENERATED_BODY()

public:
	/**
	 * Gets the events object used for binding Blueprint callbacks to events from this component.
	 *
	 * @return
	 *	The events object for this interface.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Ability Bindings")
	virtual UPF2AbilityBindingsInterfaceEvents* GetEvents() const = 0;

	/**
	 * Gets a copy of the bindings in this component.
	 *
	 * @return
	 *	The current bindings between input and commands.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Ability Bindings")
	virtual TMap<UInputAction*, TScriptInterface<IPF2InteractableAbilityInterface>> GetBindingsMap() const = 0;

	/**
	 * Binds an ability to a particular input action.
	 *
	 * @param Action
	 *	The action to which the ability will be bound.
	 * @param AbilitySpec
	 *	The ability to bind.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Ability Bindings")
	virtual void SetBinding(UInputAction* Action, const FGameplayAbilitySpec& AbilitySpec) = 0;

	/**
	 * Clears all bindings from this component.
	 *
	 * If input is currently wired up, bindings are removed from input before being cleared.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Ability Bindings")
	virtual void ClearBindings() = 0;

	/**
	 * Clear a specific binding from this component.
	 *
	 * @param Action
	 *	The action for which a binding is to be cleared.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Ability Bindings")
	virtual void ClearBinding(const UInputAction* Action) = 0;

	/**
	 * Populates the bindings array from the abilities that have been granted to the owning character.
	 *
	 * To prevent duplicate bindings, this can only be called when no bindings have yet been defined or all have been
	 * cleared.
	 *
	 * If input is currently wired up, the new bindings are automatically added to input.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Ability Bindings")
	virtual void LoadAbilitiesFromCharacter() = 0;

	/**
	 * Wires-up all bindings to receive input from the given player input component.
	 *
	 * Only new bindings that have an action assigned and have not yet been wired up will be affected; any binding that
	 * was previously wired up will be ignored.
	 *
	 * @param InputComponent
	 *	The component to which input should be bound.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Ability Bindings")
	virtual void ConnectToInput(UEnhancedInputComponent* InputComponent) = 0;

	/**
	 * Removes the association between actions and the input component, allowing all bindings to be re-assigned.
	 *
	 * This must be called before changing the action of any binding.
	 *
	 * Only bindings that have been wired up will be affected; any new binding that is not yet wired up will be ignored.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Ability Bindings")
	virtual void DisconnectFromInput() = 0;

	/**
	 * Executes the specified ability on the owning character in response to the specified action.
	 *
	 * This is expected to be invoked only by a command binding.
	 *
	 * @param Action
	 *	The input action that invoked the binding.
	 * @param AbilitySpecHandle
	 *	The handle for the ability to activate.
	 */
	virtual void ExecuteBoundAbility(const UInputAction*              Action,
	                                 const FGameplayAbilitySpecHandle AbilitySpecHandle) = 0;
};
