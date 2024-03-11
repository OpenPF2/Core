// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "CharacterStats/PF2CharacterAbilityScoreType.h"

#include "PF2AbilityBoostRuleOption.generated.h"

USTRUCT(BlueprintType)
struct OPENPF2CORE_API FPF2AbilityBoostRuleOption
{
	GENERATED_BODY()

	explicit FPF2AbilityBoostRuleOption() :
		bIsFreeBoost(false)
	{
	}

	explicit FPF2AbilityBoostRuleOption(const bool bIsFreeAbilityBoost) :
		bIsFreeBoost(bIsFreeAbilityBoost)
	{

	}

	explicit FPF2AbilityBoostRuleOption(const TSet<EPF2CharacterAbilityScoreType>& AbilityScoreTypes) :
		bIsFreeBoost(false),
		AbilityScoreTypes(AbilityScoreTypes)
	{
	}

	bool operator==(const FPF2AbilityBoostRuleOption& Other) const
	{
		return ((this->bIsFreeBoost == Other.bIsFreeBoost) &&
			    (this->AbilityScoreTypes.Num() == Other.AbilityScoreTypes.Num()) &&
			    (this->AbilityScoreTypes.Intersect(Other.AbilityScoreTypes).Num() == this->AbilityScoreTypes.Num()));
	};

	bool operator!=(const FPF2AbilityBoostRuleOption& Other) const
	{
		return (!(*this == Other));
	};

	/**
	 * Whether this is a free ability boost option or not.
	 *
	 * If set, then "Ability Score Options" are ignored.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsFreeBoost;

	/**
	 * The abilities from which the player can choose, if this is not a free ability boost.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bIsFreeBoost"))
	TSet<EPF2CharacterAbilityScoreType> AbilityScoreTypes;
};
