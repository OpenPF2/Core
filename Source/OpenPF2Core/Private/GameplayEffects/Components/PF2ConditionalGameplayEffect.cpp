// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "GameplayEffects/Components/PF2ConditionalGameplayEffect.h"

bool FPF2ConditionalGameplayEffect::CanApply(const float                  EffectLevel,
                                             const FGameplayTagContainer& SourceTags,
                                             const FGameplayTagContainer& TargetTags) const
{
	const bool bSourceMatchesAllRequired = SourceTags.HasAll(this->SourceRequiredTags),
	           bTargetMatchesAllRequired = TargetTags.HasAll(this->TargetRequiredTags),
	           bSourceHasNoIgnored       = !SourceTags.HasAny(this->SourceIgnoredTags),
	           bTargetHasNoIgnored       = !TargetTags.HasAny(this->TargetIgnoredTags),
	           bSourceSatisfiesQuery     = this->SourceTagQuery.IsEmpty() || this->SourceTagQuery.Matches(SourceTags),
	           bTargetSatisfiesQuery     = this->TargetTagQuery.IsEmpty() || this->TargetTagQuery.Matches(TargetTags);

	return (
		bSourceMatchesAllRequired && bTargetMatchesAllRequired &&
		bSourceHasNoIgnored && bTargetHasNoIgnored &&
		bSourceSatisfiesQuery && bTargetSatisfiesQuery
	);
}

FGameplayEffectSpecHandle FPF2ConditionalGameplayEffect::CreateSpec(
	const FGameplayEffectContextHandle& EffectContext,
	const float                         SourceLevel) const
{
	if (this->EffectClass != nullptr)
	{
		const UGameplayEffect* EffectCDO = this->EffectClass->GetDefaultObject<UGameplayEffect>();

		return FGameplayEffectSpecHandle(new FGameplayEffectSpec(EffectCDO, EffectContext, SourceLevel));
	}
	else
	{
		return FGameplayEffectSpecHandle();
	}
}

bool FPF2ConditionalGameplayEffect::operator==(const FPF2ConditionalGameplayEffect& Other) const
{
	return (
		(this->EffectClass == Other.EffectClass) &&
		(this->SourceRequiredTags == Other.SourceRequiredTags) &&
		(this->SourceIgnoredTags == Other.SourceIgnoredTags) &&
		(this->SourceTagQuery == Other.SourceTagQuery)
	);
}

bool FPF2ConditionalGameplayEffect::operator!=(const FPF2ConditionalGameplayEffect& Other) const
{
	return (
		(this->EffectClass != Other.EffectClass) ||
		(this->SourceRequiredTags != Other.SourceRequiredTags) ||
		(this->SourceIgnoredTags != Other.SourceIgnoredTags) ||
		(this->SourceTagQuery != Other.SourceTagQuery)
	);
}
