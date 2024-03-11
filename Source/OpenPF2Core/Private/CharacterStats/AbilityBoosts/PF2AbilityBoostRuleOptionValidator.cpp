// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "CharacterStats/AbilityBoosts/PF2AbilityBoostRuleOptionValidator.h"

#include "Utilities/PF2EnumUtilities.h"

void UPF2AbilityBoostRuleOptionValidator::AppendRuleOptions(const TArray<FPF2AbilityBoostRuleOption> NewRuleOptions)
{
	for (const auto& RuleOption : NewRuleOptions)
	{
		this->AddRuleOption(RuleOption);
	}
}

int32 UPF2AbilityBoostRuleOptionValidator::GetRemainingBoostCount() const
{
	return (this->RuleOptions.Num() - this->UsedAbilities.Num());
}

TSet<EPF2CharacterAbilityScoreType> UPF2AbilityBoostRuleOptionValidator::GetRemainingOptions()
{
	TSet<EPF2CharacterAbilityScoreType> AllRuleAbilityOptions;
	TSet<EPF2CharacterAbilityScoreType> RemainingOptions;

	for (const auto& RuleOption : this->RuleOptions)
	{
		if (RuleOption.bIsFreeBoost)
		{
			for (const auto& AbilityScoreType : TEnumRange<EPF2CharacterAbilityScoreType>())
			{
				AllRuleAbilityOptions.Add(AbilityScoreType);
			}
		}
		else
		{
			AllRuleAbilityOptions.Append(RuleOption.AbilityScoreTypes);
		}
	}

	if (this->UsedAbilities.Num() == 0)
	{
		// We don't need to search any further, since all options are still on the table.
		RemainingOptions = AllRuleAbilityOptions;
	}
	else
	{
		for (const auto& AbilityScoreType : AllRuleAbilityOptions)
		{
			if (this->CanApplyAbilityBoost(AbilityScoreType))
			{
				RemainingOptions.Add(AbilityScoreType);
			}
		}
	}

	return RemainingOptions;
}

bool UPF2AbilityBoostRuleOptionValidator::HasRemainingBoosts()
{
	return (this->GetRemainingBoostCount() != 0);
}

void UPF2AbilityBoostRuleOptionValidator::AddRuleOption(const FPF2AbilityBoostRuleOption RuleOption)
{
	checkf(this->UsedAbilities.Num() == 0, TEXT("Rule options cannot be added once an ability boost has been added."));
	this->RuleOptions.Add(RuleOption);

	// This is done for safety, but really shouldn't have much of an impact since we tend only to evaluate and cache
	// permutations after an ability boost has been applied.
	this->CachedRulePermutations.Empty();
}

bool UPF2AbilityBoostRuleOptionValidator::CanApplyAbilityBoost(const EPF2CharacterAbilityScoreType AbilityScoreType)
{
	bool                                bCanApply                = false;
	TSet<EPF2CharacterAbilityScoreType> AbilityScoreTypesToMatch = this->UsedAbilities;

	// We can't apply more boosts than we have rules.
	// Also, the same ability score type can't be targeted twice in the same boost activation.
	if ((this->GetRemainingBoostCount() > 0) && !AbilityScoreTypesToMatch.Contains(AbilityScoreType))
	{
		TArray<TArray<FPF2AbilityBoostRuleOption>> RulePermutations      = this->CalculateRulePermutations();
		TArray<TArray<FPF2AbilityBoostRuleOption>> RemainingPermutations = RulePermutations;

		AbilityScoreTypesToMatch.Add(AbilityScoreType);

		for (const TArray<FPF2AbilityBoostRuleOption>& RulePermutation : RulePermutations)
		{
			for (int PositionIndex = 0; PositionIndex < AbilityScoreTypesToMatch.Num(); ++PositionIndex)
			{
				const FSetElementId                  ElementId               = FSetElementId::FromInteger(PositionIndex);
				const EPF2CharacterAbilityScoreType& AbilityScoreTypeToMatch = AbilityScoreTypesToMatch[ElementId];
				const FPF2AbilityBoostRuleOption&    RuleOption              = RulePermutation[PositionIndex];

				if (!RuleOption.bIsFreeBoost && !RuleOption.AbilityScoreTypes.Contains(AbilityScoreTypeToMatch))
				{
					// This permutation doesn't have a rule in this position of the sequence of rules that would match
					// this ability boost, so disqualify the permutation.
					RemainingPermutations.Remove(RulePermutation);
					break;
				}
			}
		}

		bCanApply = (RemainingPermutations.Num() != 0);
	}

	return bCanApply;
}

void UPF2AbilityBoostRuleOptionValidator::ApplyAbilityBoost(const EPF2CharacterAbilityScoreType AbilityScoreType)
{
	checkf(
		this->CanApplyAbilityBoost(AbilityScoreType),
		TEXT("A boost to '%s' cannot be applied based on the unused rule options ('%d' boosts remain)."),
		*(PF2EnumUtilities::ToString(AbilityScoreType)),
		this->GetRemainingBoostCount()
	);

	this->UsedAbilities.Add(AbilityScoreType);
}

TArray<TArray<FPF2AbilityBoostRuleOption>> UPF2AbilityBoostRuleOptionValidator::CalculateRulePermutations()
{
	if (this->CachedRulePermutations.Num() == 0)
	{
		this->CachedRulePermutations = this->CalculateRulePermutations(this->RuleOptions, {});
	}

	return this->CachedRulePermutations;
}

TArray<TArray<FPF2AbilityBoostRuleOption>> UPF2AbilityBoostRuleOptionValidator::CalculateRulePermutations(
	const TArray<FPF2AbilityBoostRuleOption>& RemainingOptions,
	const TArray<FPF2AbilityBoostRuleOption>& SeenOptions) const
{
	TArray<TArray<FPF2AbilityBoostRuleOption>> Result;
	const int32                                RemainingOptionCount = RemainingOptions.Num();

	if (RemainingOptionCount == 0)
	{
		Result.Add(SeenOptions);
	}
	else
	{
		for (int OptionIndex = 0; OptionIndex < RemainingOptionCount; ++OptionIndex)
		{
			const FPF2AbilityBoostRuleOption   RuleOption          = RemainingOptions[OptionIndex];
			TArray<FPF2AbilityBoostRuleOption> NewRemainingOptions = RemainingOptions,
			                                   NewSeenOptions      = SeenOptions;

			// Skip the option we're on right now.
			NewRemainingOptions.RemoveAt(OptionIndex);

			NewSeenOptions.Add(RuleOption);
			Result.Append(this->CalculateRulePermutations(NewRemainingOptions, NewSeenOptions));
		}
	}

	return Result;
}
