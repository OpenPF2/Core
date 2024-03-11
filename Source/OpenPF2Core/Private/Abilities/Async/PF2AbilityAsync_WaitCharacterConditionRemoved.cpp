// OpenPF2 for UE Game Logic, Copyright 2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/Async/PF2AbilityAsync_WaitCharacterConditionRemoved.h"

#include "PF2CharacterInterface.h"

UPF2AbilityAsync_WaitCharacterConditionRemoved* UPF2AbilityAsync_WaitCharacterConditionRemoved::WaitConditionRemovedFromCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	const FGameplayTag                              ConditionParentTag,
	const bool                                      bFireImmediatelyIfAlreadySatisfied,
	const bool                                      bOnlyTriggerOnce)
{
	UPF2AbilityAsync_WaitCharacterConditionRemoved* Task = NewObject<UPF2AbilityAsync_WaitCharacterConditionRemoved>();

	if (Character == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Character passed into 'Wait for Condition Removed from Character' must be set to a non-null OpenPF2 character.")
		);
	}
	else
	{
		Task->SetAbilityActor(Character->ToActor());
	}

	Task->ConditionParentTag                 = ConditionParentTag;
	Task->CriterionToSatisfy                 = TagRemoved;
	Task->bFireImmediatelyIfAlreadySatisfied = bFireImmediatelyIfAlreadySatisfied;
	Task->bOnlyTriggerOnce                   = bOnlyTriggerOnce;

	return Task;
}

void UPF2AbilityAsync_WaitCharacterConditionRemoved::OnTagCriterionSatisfied(const FGameplayTag& ConditionTag)
{
	this->OnConditionRemoved.Broadcast(ConditionTag);
}
