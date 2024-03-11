// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Abilities/GameplayAbility.h>

#include "PF2GameplayAbilityBase.h"
#include "PF2PassiveGameplayAbilityBase.generated.h"

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Abstract base class for gameplay abilities that are passive and activated immediately after being granted.
 *
 * GAs that extend from this base class do not get activated directly by the player.
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class UPF2PassiveGameplayAbilityBase : public UPF2GameplayAbilityBase
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods - UGameplayAbility Overrides
	// =================================================================================================================
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
