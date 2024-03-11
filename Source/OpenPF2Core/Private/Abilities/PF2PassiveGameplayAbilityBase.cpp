// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2PassiveGameplayAbilityBase.h"

#include <AbilitySystemComponent.h>

void UPF2PassiveGameplayAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilitySpec&      Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// Attempt to activate this ability immediately.
	ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
}
