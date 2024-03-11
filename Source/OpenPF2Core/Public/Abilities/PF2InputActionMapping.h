// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Templates/SubclassOf.h>

#include "PF2InputActionMapping.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class UGameplayAbility;
class UInputAction;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * A mapping between an input action and a character ability.
 *
 * If the input action is invoked but the character does not have the ability
 * specified, nothing will happen.
 */
USTRUCT(BlueprintType)
struct OPENPF2CORE_API FPF2InputActionMapping
{
	GENERATED_BODY()

protected:
	/**
	 * The input action that will trigger the ability.
	 */
	UPROPERTY(EditDefaultsOnly)
	UInputAction* InputAction;

	/**
	 * The ability to invoke when the input action is pressed.
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> Ability;

public:
	/**
	 * Constructs a new instance.
	 */
	explicit FPF2InputActionMapping() : InputAction(nullptr), Ability(nullptr)
	{
	}

	/**
	 * Gets the input action that will trigger the ability.
	 *
	 * @return
	 *	The input action.
	 */
	UInputAction* GetInputAction() const
	{
		return this->InputAction;
	};

	/**
	 * Gets the CDO of the ability to invoke when the input action is pressed.
	 *
	 * @return
	 *	The ability.
	 */
	UGameplayAbility* GetAbility() const
	{
		UGameplayAbility* AbilityCDO;

		if (this->Ability == nullptr)
		{
			AbilityCDO = nullptr;
		}
		else
		{
			AbilityCDO = this->Ability.GetDefaultObject();
		}

		return AbilityCDO;
	};
};
