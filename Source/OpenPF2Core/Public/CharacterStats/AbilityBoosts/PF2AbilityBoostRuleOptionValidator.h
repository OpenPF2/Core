// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "CharacterStats/PF2CharacterAbilityScoreType.h"
#include "CharacterStats/AbilityBoosts/PF2AbilityBoostRuleOption.h"

#include "PF2AbilityBoostRuleOptionValidator.generated.h"

/**
 * A service object for evaluating ability boost choices against ability boost rule options.
 *
 * For example, if a particular ancestry grants the player boosts to the following:
 * 1. Strength or Dexterity
 * 2. One free ability boost
 *
 * And the player chooses Intelligence and Strength, this class can determine that the boost to Strength goes with the
 * first rule option and the boost to Intelligence goes with the second rule option. Similarly, this class can be used
 * to determine what options to present to the player as they make choices, by eliminating options that are no longer
 * allowed by the combinations of rule options and the rule that the same ability cannot be boosted twice by the same
 * GA activation (for boosts granted "at the same time").
 */
UCLASS(BlueprintType)
class UPF2AbilityBoostRuleOptionValidator final : public UObject
{
	GENERATED_BODY()

protected:
	/**
	 * The rule options against which boosts will be checked.
	 */
	UPROPERTY(BlueprintReadOnly)
	TArray<FPF2AbilityBoostRuleOption> RuleOptions;

	/**
	 * The abilities that have already been targeted by previous boosts during this activation.
	 */
	UPROPERTY(BlueprintReadOnly)
	TSet<EPF2CharacterAbilityScoreType> UsedAbilities;

	/**
	 * An internal cache of all the possible permutations of rule option evaluation orders.
	 *
	 * @see CalculateRulePermutations()
	 */
	TArray<TArray<FPF2AbilityBoostRuleOption>> CachedRulePermutations;

public:
	/**
	 * Constructor for UPF2AbilityBoostRuleOptionValidator.
	 */
	explicit UPF2AbilityBoostRuleOptionValidator()
	{
	};

	/**
	 * Gets the count of how many additional boosts can be applied.
	 *
	 * @return
	 *	The number of boosts that can still be applied.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Ability Boosts|Rule Option Validators")
	int32 GetRemainingBoostCount() const;

	/**
	 * Gets the list of abilities that have not yet been boosted that rule options allow to be boosted.
	 *
	 * @return
	 *	The set of abilities that can be boosted, according to the rule options in this matcher.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Ability Boosts|Rule Option Validators")
	TSet<EPF2CharacterAbilityScoreType> GetRemainingOptions();

	/**
	 * Determines if there are any remaining boosts to choose based on the rule options that have been set.
	 *
	 * @return
	 *	true if the number of remaining boosts is greater than 0; or, false, otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Ability Boosts|Rule Option Validators")
	bool HasRemainingBoosts();

	/**
	 * Adds multiple rule options to be taken into consideration during validation.
	 *
	 * This cannot be called if ability boosts have already been applied to the validator.
	 *
	 * @param NewRuleOptions
	 *	The options to add to the validator.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Ability Boosts|Rule Option Validators")
	void AppendRuleOptions(const TArray<FPF2AbilityBoostRuleOption> NewRuleOptions);

	/**
	 * Adds a rule option to be taken into consideration during validation.
	 *
	 * This cannot be called if ability boosts have already been applied to the validator.
	 *
	 * @param RuleOption
	 *	The option to add to the validator.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Ability Boosts|Rule Option Validators")
	void AddRuleOption(const FPF2AbilityBoostRuleOption RuleOption);

	/**
	 * Determines if the specified ability score can be boosted based on rule options and previously-boosted abilities.
	 *
	 * @param AbilityScoreType
	 *	The type of ability score for which a boost is being considered.
	 *
	 * @return
	 *	true if a boost of the specified ability would be allowed; false if it would not be allowed.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Ability Boosts|Rule Option Validators")
	bool CanApplyAbilityBoost(EPF2CharacterAbilityScoreType AbilityScoreType);

	/**
	 * Tracks a boost of the specified ability score.
	 *
	 * The caller should verify that the boost is allowed by calling CanApplyAbilityBoost() first. This is checked in
	 * development builds.
	 *
	 * @param AbilityScoreType
	 *	The type of ability score that is being boosted.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Ability Boosts|Rule Option Validators")
	void ApplyAbilityBoost(EPF2CharacterAbilityScoreType AbilityScoreType);

protected:
	/**
	 * Calculates all possible ways/orders that the rule options could be applied by the player.
	 *
	 * To improve performance, the result is cached per instance.
	 *
	 * Normally, it would be computationally and memory intensive to maintain all possible permutations for how rule
	 * options could be evaluated. However, in OpenPF2, most game rules have a maximum of 3 rule options (2 is much more
	 * common), the largest this array will tend to be is 6 elements. If we find that this becomes a problem, we could
	 * switch to a more efficient algorithm (e.g. perhaps we could define a "natural ordering" for both the ability
	 * score types AND rule options that allows us to re-sort boost choices from the player so they are always applied
	 * to the rule options in the same order regardless of the order that the player chooses them).
	 *
	 * @return
	 *	All possible permutations of the rule options this matcher is evaluating.
	 */
	TArray<TArray<FPF2AbilityBoostRuleOption>> CalculateRulePermutations();

	/**
	 * Internal, recursive method for calculating permutations of rule options.
	 *
	 * @param RemainingOptions
	 *	The remaining rule options that can be used to construct additional permutations.
	 * @param SeenOptions
	 *	The rule options included in this permutation thus far.
	 *
	 * @return
	 *	The full list of permutation(s) that started with the given SeenOptions.
	 */
	TArray<TArray<FPF2AbilityBoostRuleOption>> CalculateRulePermutations(
		const TArray<FPF2AbilityBoostRuleOption>& RemainingOptions,
		const TArray<FPF2AbilityBoostRuleOption>& SeenOptions) const;
};
