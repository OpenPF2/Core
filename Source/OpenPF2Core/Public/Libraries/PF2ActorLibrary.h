// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>

#include <UObject/Interface.h>

#include "PF2ActorLibrary.generated.h"

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Function library for interacting with actors in OpenPF2.
 */
UCLASS()
class UPF2ActorLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Locates the actor component that implements the specified interface.
	 *
	 * In development builds, an assertion error is raised if more than one component matches the given interface type.
	 * In shipping builds, only the first matching component is returned.
	 *
	 * @param [in]  Actor
	 *	The actor for which a component is desired.
	 * @param [in]  Interface
	*	The type of interface to locate.
	 * @param [out] bWasFound
	 *	A reference to an output variable to receive the status of whether the component was found.
	 *
	 * @return
	 *	Either the component that matches the given interface; or, nullptr if there is no such component.
	 */
	UFUNCTION(BlueprintPure, Category = "OpenPF2|Actors|Components", meta = (DeterminesOutputType = "Interface"))
	static TScriptInterface<IInterface> GetComponentByInterface(const AActor*                 Actor,
	                                                            const TSubclassOf<UInterface> Interface,
	                                                            bool&                         bWasFound);
};
