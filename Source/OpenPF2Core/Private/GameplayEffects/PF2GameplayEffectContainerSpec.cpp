// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "GameplayEffects/PF2GameplayEffectContainerSpec.h"

void FPF2GameplayEffectContainerSpec::AddHitTargets(const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& HitResult : HitResults)
	{
		// This heap-allocated instance becomes a shared pointer in FGameplayAbilityTargetDataHandle, which then handles
		// freeing it/cleaning it up.
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);

		TargetData.Add(NewData);
	}
}

void FPF2GameplayEffectContainerSpec::AddActorTargets(const TArray<AActor*>& TargetActors)
{
	if (TargetActors.Num() > 0)
	{
		// This heap-allocated instance becomes a shared pointer in FGameplayAbilityTargetDataHandle, which then handles
		// freeing it/cleaning it up.
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);

		TargetData.Add(NewData);
	}
}

void FPF2GameplayEffectContainerSpec::AddTargetData(FGameplayAbilityTargetData* ExistingTargetData)
{
	this->TargetData.Add(ExistingTargetData);
}

void FPF2GameplayEffectContainerSpec::AppendTargets(const FGameplayAbilityTargetDataHandle& ExistingTargetDataHandle)
{
	this->TargetData.Append(ExistingTargetDataHandle);
}
