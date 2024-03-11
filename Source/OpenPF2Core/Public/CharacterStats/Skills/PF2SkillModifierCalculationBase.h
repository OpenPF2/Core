// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "CharacterStats/AbilityModifiers/PF2SimpleTemlModifierCalculationBase.h"

#include "PF2SkillModifierCalculationBase.generated.h"

/**
 * Base class for Skill modifier MMCs.
 *
 * There is expected to be one blueprint sub-class of this base class for each skill (Acrobatics, Arcana, etc.). The
 * ability upon which the skill modifier is based *must* be the one and ONLY attribute being captured.
 */
UCLASS(Abstract)
class OPENPF2CORE_API UPF2SkillModifierCalculationBase : public UPF2SimpleTemlModifierCalculationBase
{
	GENERATED_BODY()
};
