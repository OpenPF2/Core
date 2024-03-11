// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Abilities/GameplayAbilityTargetTypes.h>

#include "CharacterStats/PF2CharacterAbilityScoreType.h"

#include "PF2GameplayAbilityTargetData_BoostAbility.generated.h"

/**
 * Target data for an ability boost chosen by the player (for use by GA sub-classes of PF2GameplayAbility_BoostAbility).
 *
 * For each option provided by the GA, this target data must have a corresponding choice. This means that the number of
 * choices provided must be equal to the number of rules options in the PF2GameplayAbility_BoostAbility GA being
 * activated. In addition, the choice being provided must be compatible with the choices offered by the GA rule options,
 * as follows:
 *   - For a rule that grants the player a choice of boosting Strength or Dexterity, the corresponding choice can only
 *     be Strength or Dexterity.
 *   - For a rule that grants the player a free ability score boost, the corresponding choice can be any ability score
 *     not already selected.
 *   - No ability score can appear more than once in the target data for the same activation of the boost GA. In other
 *     words, consider a GA that provides the player with two ability boost options: 1) boost Strength or Dexterity, and
 *     2) a free ability boost; then if Strength is chosen for rule option 1, it cannot appear in the choice for rule
 *     option 2 (though Dexterity can); while if Dexterity is chosen for rule option 1, it cannot appear in the choice
 *     for rule option 2 (though Strength can).
 */
USTRUCT(BlueprintType)
struct OPENPF2CORE_API FPF2GameplayAbilityTargetData_BoostAbility : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TSet<EPF2CharacterAbilityScoreType> SelectedAbilities;

	/**
	 * Constructor for FPF2GameplayAbilityTargetData_BoostAbility.
	 */
	explicit FPF2GameplayAbilityTargetData_BoostAbility()
	{
	}

	/**
	 * Constructor for FPF2GameplayAbilityTargetData_BoostAbility.
	 *
	 * @param SelectedAbilities
	 *	The choice(s) for ability boosts. The choices must be allowed by the rule options of the GA being activated. The
	 *	choices can be provided in any order, regardless of the order of the rule options.
	 */
	explicit FPF2GameplayAbilityTargetData_BoostAbility(const TSet<EPF2CharacterAbilityScoreType>& SelectedAbilities) :
		SelectedAbilities(SelectedAbilities)
	{
	}
};
