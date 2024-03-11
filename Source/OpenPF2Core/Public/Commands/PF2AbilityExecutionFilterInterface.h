// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "Commands/PF2AbilityExecutionFilterContext.h"

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2AbilityExecutionFilterInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterCommandInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType)
class UPF2AbilityExecutionFilterInterface : public UPF2LogIdentifiableInterface
{
	GENERATED_BODY()
};

/**
 * An interface for classes that filter the execution of character abilities.
 *
 * Filters act like wrappers around ability invocation, affecting execution in one of three ways:
 *   1. Proceeding to activate the ability.
 *   2. Activating an alternate ability rather than the ability that would normally be invoked.
 *   3. Dropping execution of the ability all together.
 *
 * Any combination of the outcomes above can exist in the same filter, controlled by conditionals. Filters can also be
 * chained.
 *
 * Filters provide more flexibility than relying on tag-based blocking in abilities and gameplay effects alone, but they
 * are not intended to replace tag-based approaches. Rather, filters are useful when you need a straightforward way to
 * vary which gameplay ability gets executed by the same input action.
 */
class OPENPF2CORE_API IPF2AbilityExecutionFilterInterface : public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Filters activation of an ability.
	 *
	 * The filter is expected to evaluate the given context and return a (conditionally updated) context to control what
	 * ability (if any) gets executed.
	 *
	 * @param Context
	 *	Context about the ability that is being invoked.
	 *
	 * @return
	 *	The updated context.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Gameplay Ability Execution Filters")
	FPF2AbilityExecutionFilterContext FilterAbilityActivation(const FPF2AbilityExecutionFilterContext Context);
};
