// OpenPF2 for UE Game Logic, Copyright 2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/Async/PF2AbilityAsync_WaitCharacterConditionAdded.h"

#include "PF2CharacterInterface.h"

#include "Libraries/PF2TagLibrary.h"

UPF2AbilityAsync_WaitCharacterConditionAdded* UPF2AbilityAsync_WaitCharacterConditionAdded::WaitConditionAddedToCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	const FGameplayTag                              ConditionParentTag,
	const bool                                      bFireImmediatelyIfAlreadySatisfied,
	const bool                                      bOnlyTriggerOnce)
{
	UPF2AbilityAsync_WaitCharacterConditionAdded* Task = NewObject<UPF2AbilityAsync_WaitCharacterConditionAdded>();

	if (Character == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Character passed into 'Wait for Condition Added to Character' must be set to a non-null OpenPF2 character.")
		);
	}
	else
	{
		Task->SetAbilityActor(Character->ToActor());
	}

	Task->ConditionParentTag                 = ConditionParentTag;
	Task->CriterionToSatisfy                 = TagAdded;
	Task->bFireImmediatelyIfAlreadySatisfied = bFireImmediatelyIfAlreadySatisfied;
	Task->bOnlyTriggerOnce                   = bOnlyTriggerOnce;

	return Task;
}

void UPF2AbilityAsync_WaitCharacterConditionAdded::OnTagCriterionSatisfied(const FGameplayTag& ConditionTag)
{
	uint8 ConditionLevel;

	// Only try parsing condition levels for conditions that support them. Otherwise, it's just wasted cycles.
	if (this->DoesConditionSupportLevels())
	{
		ConditionLevel = UPF2TagLibrary::ParseConditionLevel(ConditionTag, this->ConditionParentTag);
	}
	else
	{
		ConditionLevel = INDEX_NONE;
	}

	this->OnConditionAdded.Broadcast(ConditionTag, ConditionLevel);
}
