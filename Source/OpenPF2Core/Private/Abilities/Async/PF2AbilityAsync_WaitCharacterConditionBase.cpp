// OpenPF2 for UE Game Logic, Copyright 2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <AbilitySystemComponent.h>
#include <GameplayTagsManager.h>

#include "Abilities/Async/PF2AbilityAsync_WaitConditionBase.h"

void UPF2AbilityAsync_WaitCharacterConditionBase::Activate()
{
	const UAbilitySystemComponent* Asc = this->GetAbilitySystemComponent();

	Super::Activate();

	check(this->CriterionToSatisfy != EWaitCriterion::None);

	if (Asc == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Warning,
			TEXT("Async task ('%s') cannot register event for gameplay tag '%s' because ASC is null."),
			*(this->GetIdForLogs()),
			*(ConditionParentTag.ToString())
		);

		this->EndAction();
	}
	else if (!this->ShouldBroadcastDelegates())
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Warning,
			TEXT("Async task ('%s') cannot register events for condition tag '%s' because ShouldBroadcastDelegates() is 'false'."),
			*(this->GetIdForLogs()),
			*(ConditionParentTag.ToString())
		);

		this->EndAction();
	}
	else
	{
		const UGameplayTagsManager&  TagsManager = UGameplayTagsManager::Get();
		const FGameplayTagContainer& ChildTags   = TagsManager.RequestGameplayTagChildren(this->ConditionParentTag);

		if (ChildTags.Num() == 0)
		{
			// Handle conditions that do not support levels.
			this->bConditionSupportsLevels = false;

			this->SetupCallbackForConditionTag(this->ConditionParentTag);

			if (this->bFireImmediatelyIfAlreadySatisfied)
			{
				this->NotifyIfCriterionSatisfied(this->ConditionParentTag);
			}
		}
		else
		{
			// Handle conditions that *do* support levels.
			this->bConditionSupportsLevels = true;

			// For these, we listen for changes to all the child tags rather than just the parent tag so that we can
			// detect when the level of a tag changes.
			for (const FGameplayTag& ChildTag : ChildTags)
			{
				this->SetupCallbackForConditionTag(ChildTag);

				if (this->bFireImmediatelyIfAlreadySatisfied)
				{
					this->NotifyIfCriterionSatisfied(ChildTag);
				}
			}
		}
	}
}

void UPF2AbilityAsync_WaitCharacterConditionBase::SetupCallbackForConditionTag(const FGameplayTag& ConditionTag)
{
	UAbilitySystemComponent* Asc = this->GetAbilitySystemComponent();
	FDelegateHandle          CallbackHandle;

	// By now, this must be valid.
	ensure(Asc != nullptr);

	CallbackHandle =
		Asc->RegisterGameplayTagEvent(ConditionTag).AddUObject(
			this,
			&UPF2AbilityAsync_WaitCharacterConditionBase::OnConditionTagCountChanged
		);

	this->CallbackHandles.Add(ConditionTag, CallbackHandle);
}

void UPF2AbilityAsync_WaitCharacterConditionBase::NotifyIfCriterionSatisfied(const FGameplayTag& ConditionTag)
{
	const UAbilitySystemComponent* Asc             = this->GetAbilitySystemComponent();
	int32                          CurrentTagCount;

	ensure(Asc != nullptr);

	CurrentTagCount = Asc->GetTagCount(ConditionTag);

	this->NotifyIfCriterionSatisfied(ConditionTag, CurrentTagCount);
}

void UPF2AbilityAsync_WaitCharacterConditionBase::OnConditionTagCountChanged(const FGameplayTag ConditionTag,
                                                                             const int32        NewCount)
{
	if (this->ShouldBroadcastDelegates())
	{
		this->NotifyIfCriterionSatisfied(ConditionTag, NewCount);
	}
	else
	{
		this->EndAction();
	}
}

void UPF2AbilityAsync_WaitCharacterConditionBase::EndAction()
{
	UAbilitySystemComponent* Asc = this->GetAbilitySystemComponent();

	if (Asc != nullptr)
	{
		for (const auto& [Tag, CallbackHandle] : this->CallbackHandles)
		{
			if (CallbackHandle.IsValid())
			{
				Asc->UnregisterGameplayTagEvent(CallbackHandle, Tag);
			}
		}
	}

	this->CallbackHandles.Empty();

	Super::EndAction();
}

FString UPF2AbilityAsync_WaitCharacterConditionBase::GetIdForLogs() const
{
	return this->GetName();
}


void UPF2AbilityAsync_WaitCharacterConditionBase::NotifyIfCriterionSatisfied(const FGameplayTag& ConditionTag,
                                                                             const int32         NewCount)
{
	const UAbilitySystemComponent* Asc = this->GetAbilitySystemComponent();

	ensure(Asc != nullptr);

	if (((this->CriterionToSatisfy == TagRemoved) && (NewCount == 0) && (Asc->GetTagCount(this->ConditionParentTag) == 0)) ||
		((this->CriterionToSatisfy == TagAdded) && (NewCount >= 1)))
	{
		this->OnTagCriterionSatisfied(ConditionTag);

		if (this->bOnlyTriggerOnce)
		{
			this->EndAction();
		}
	}
}
