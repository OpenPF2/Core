// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/Interface.h>

#include "CharacterStats/AbilityBoosts/PF2AbilityBoostRuleOption.h"

#include "PF2AbilityBoostInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class UGameplayAbility;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2AbilityBoostInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible Ability Boost GAs.
 */
class OPENPF2CORE_API IPF2AbilityBoostInterface
{
	GENERATED_BODY()

public:
	/**
	 * Gets the description of this boost, as shown to the player when they are being asked to make a selection.
	 *
	 * @return
	 *	The description of this ability boost.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Ability Boosts")
	virtual FText GetDescription() const = 0;

	/**
	 * Gets the boost(s) this ability applies to the target.
	 *
	 * Each element represents a single rule option for an ability score boost. Each can either be a free ability boost;
	 * or the option to choose between one or more ability options.
	 *
	 * @return
	 *	The rules for how this boost must be applied.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Ability Boosts")
	virtual TArray<FPF2AbilityBoostRuleOption> GetBoostRuleOptions() const = 0;

	/**
	 * Gets the gameplay ability that is implementing this interface.
	 *
	 * @return
	 *	This object, as a UE-native Gameplay Ability.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Ability Boosts")
	virtual UGameplayAbility* ToGameplayAbility() = 0;
};
