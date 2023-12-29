// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Abilities/GameplayAbility.h>

#include "PF2TestAbility.generated.h"

/**
 * A fake Gameplay Ability for use in testing logic that requires abilities granted to a character.
 */
UCLASS(Blueprintable, BlueprintType, notplaceable)
class OPENPF2CORE_API UPF2TestAbility : public UGameplayAbility
{
	GENERATED_BODY()
};
