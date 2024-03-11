// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayAbilitySpec.h>
#include <InputAction.h>

#include "PF2AbilityInputBinding.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class UEnhancedInputComponent;
class IPF2AbilityBindingsInterface;
class IPF2CharacterInterface;
class UGameplayAbility;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * A single binding between an input action and an ability that can be activated.
 *
 * The ability must already have been granted to the character.
 *
 * (This must be a UCLASS rather than a USTRUCT because enhanced input only supports binding callbacks from UObjects.)
 */
UCLASS(BlueprintType)
class UPF2AbilityInputBinding : public UObject
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Fields - Blueprint Accessible
	// =================================================================================================================
	/**
	 * The action to which the ability is bound.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Action;

	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The handle for the corresponding ability.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayAbilitySpecHandle AbilitySpecHandle;

protected:
	/**
	 * The handles for input action bindings, if this binding has been connected to input.
	 */
	UPROPERTY()
	TArray<int32> InputHandles;

	/**
	 * The component that is managing this binding.
	 *
	 * This is declared as an actor component instead of an interface so that UE interacts with it properly for
	 * replication. UE will not do this if this component were declared/referenced through an interface field.
	 */
	UPROPERTY()
	UActorComponent* BindingsOwner;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor (used by Blueprint).
	 */
	explicit UPF2AbilityInputBinding() : Action(nullptr), BindingsOwner(nullptr)
	{
	}

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Initializes this binding from a granted ability.
	 *
	 * @param NewAction
	 *	The action to which the ability will be bound.
	 * @param NewAbilitySpec
	 *	The specification for the ability.
	 * @param NewOwner
	 *	The component that is managing this binding.
	 */
	void Initialize(UInputAction*                 NewAction,
	                const FGameplayAbilitySpec&   NewAbilitySpec,
	                IPF2AbilityBindingsInterface* NewOwner);

	/**
	 * Determines whether the ability in this binding has been connected to an input component.
	 *
	 * @return
	 *	- true if this binding is presently connected to input.
	 *	- false if this binding is not connected to input at this time.
	 */
	FORCEINLINE bool IsConnectedToInput() const
	{
		return this->InputHandles.Num() != 0;
	}

	/**
	 * Wires-up this binding to receive input from the given enhanced input component.
	 *
	 * This only has an effect if this binding has an action assigned and this binding has not yet been wired up;
	 * otherwise, this call will have no effect.
	 *
	 * @param InputComponent
	 *	The component to which input should be bound.
	 */
	void ConnectToInput(UEnhancedInputComponent* InputComponent);

	/**
	 * Removes this binding from the given enhanced input component.
	 *
	 * This must be called before changing the action of any binding.
	 *
	 * This only has an effect if this binding has been wired up; otherwise, this call will have no effect.
	 */
	void DisconnectFromInput(UEnhancedInputComponent* InputComponent);

protected:
	// =================================================================================================================
	// Protected Instance Methods
	// =================================================================================================================
	/**
	 * Gets the action to which the ability is bound.
	 *
	 * @return
	 *	The action of this binding.
	 */
	FORCEINLINE const UInputAction* GetAction() const
	{
		return this->Action;
	}

	/**
	 * Determines whether there is a non-null action set in this binding.
	 *
	 * @return
	 *	true if this binding has a non-null action; or, false otherwise.
	 */
	FORCEINLINE bool HasAction() const
	{
		return (this->GetAction() != nullptr);
	}

	/**
	 * Gets the name of the action (if any) that this binding has been configured with.
	 *
	 * @return
	 *	The human-readable name of the bound action.
	 */
	FORCEINLINE FString GetActionName() const
	{
		return GetNameSafe(this->GetAction());
	}

	/**
	 * Gets the component that is managing this binding.
	 *
	 * @return
	 *	The containing bindings component.
	 */
	IPF2AbilityBindingsInterface* GetBindingsOwner() const;

	/**
	 * Notifies the specified binding instance that the input action it corresponds to has been activated.
	 */
	UFUNCTION()
	void LocalInputPressed();

	/**
	 * Notifies the specified binding instance that the input action it corresponds to has been released.
	 */
	UFUNCTION()
	void LocalInputReleased();

	/**
	 * Assembles and executes a command that invokes the ability associated with this binding.
	 *
	 * The ability will be wrapped in a character command for execution. The resulting command may be queued if the
	 * active MoPRS is requiring abilities to be queued (e.g., during encounters).
	 */
	virtual void ActivateAbility();

	/**
	 * Notifies the ability that the player no longer wishes to invoke the ability associated with this binding.
	 *
	 * The default implementation does nothing.
	 */
	virtual void DeactivateAbility();
};
