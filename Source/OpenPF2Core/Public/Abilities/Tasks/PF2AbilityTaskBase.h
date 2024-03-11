// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Abilities/Tasks/AbilityTask.h>

#include "PF2AbilityTaskBase.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2AbilitySystemInterface;
class IPF2CharacterAbilitySystemInterface;
class IPF2InteractableAbilityInterface;
class UAbilitySystemComponent;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Common base class for OpenPF2 Ability Tasks.
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2AbilityTaskBase : public UAbilityTask
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets whether this ability task has a valid ability.
	 *
	 * @return
	 *	true if the ability in this task is properly set; or false if it is null.
	 */
	FORCEINLINE bool HasAbility() const
	{
		return this->Ability != nullptr;
	}

	/**
	 * Gets the ability that is executing this ability task.
	 *
	 * @return
	 *	Either the owning ability; or null if the ability of this ability task
	 *	has not yet been initialized.
	 */
	FORCEINLINE UGameplayAbility* GetNativeAbility() const
	{
		check(this->HasAbility());

		return this->Ability;
	}

	/**
	 * Gets the OpenPF2-compatible interface for the ability that is invoking this ability task.
	 *
	 * @return
	 *	The ability as a PF2 ability script interface instance. If the ability is unset or not OpenPF2-compatible, the
	 *	script interface wraps nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	TScriptInterface<IPF2InteractableAbilityInterface> GetAbility() const;

	/**
	 * Gets whether this ability task has a valid Ability System Component.
	 *
	 * @return
	 *	true if the ASC in this task is properly set; or false if it is null.
	 */
	FORCEINLINE bool HasAbilitySystemComponent() const
	{
		return this->AbilitySystemComponent != nullptr;
	}

	/**
	 * Gets the Ability System Component of this ability task.
	 *
	 * @return
	 *	The ASC in this task.
	 */
	FORCEINLINE UAbilitySystemComponent* GetNativeAbilitySystemComponent() const
	{
		check(this->HasAbilitySystemComponent());

		return this->AbilitySystemComponent.Get();
	}

	/**
	 * Gets the OpenPF2-compatible interface for the ability system component invoking this ability task.
	 *
	 * @return
	 *	The ASC as a PF2 ASC script interface instance. If the ASC is unset or not OpenPF2-compatible, the script
	 *	interface wraps nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	TScriptInterface<IPF2AbilitySystemInterface> GetAbilitySystemComponent() const;

	/**
	 * Gets the OpenPF2-compatible interface for the character ability system component invoking this ability task.
	 *
	 * @return
	 *	The ASC as a PF2 character ASC script interface instance. If the ASC is unset or not an OpenPF2 character ASC,
	 *	the script interface wraps nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	TScriptInterface<IPF2CharacterAbilitySystemInterface> GetCharacterAbilitySystemComponent() const;
};
