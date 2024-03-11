// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "PF2CharacterBase.h"

#include "PF2TestCharacter.generated.h"

/**
 * A non-abstract character for use in testing logic that relies on interactions with OpenPF2-compatible characters.
 */
UCLASS()
class OPENPF2CORE_API APF2TestCharacter : public APF2CharacterBase
{
	GENERATED_BODY()
};
