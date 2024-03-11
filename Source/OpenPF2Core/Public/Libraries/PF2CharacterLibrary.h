// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>
#include <Templates/SubclassOf.h>

#include "PF2CharacterLibrary.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IInterface;
class IPF2CharacterInterface;
class IPF2PlayerControllerInterface;
class UInterface;
class UWorld;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Function library for interacting with OpenPF2 characters and character controllers.
 */
UCLASS()
class UPF2CharacterLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Gets controllers for all the characters in the world.
	 *
	 * @param World
	 *	A reference to the loaded, running world that contains the characters.
	 *
	 * @return
	 *	An array of all OpenPF2-specific controllers for all players in the world.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Characters")
	static TArray<TScriptInterface<IPF2PlayerControllerInterface>> GetPlayerControllers(const UWorld* const World);

	/**
	 * Gets all of the player-controlled OpenPF2 characters in the world.
	 *
	 * If called on the client and the player state has not yet been replicated, this will return an empty array.
	 *
	 * @param World
	 *	A reference to the loaded, running world that contains the characters.
	 *
	 * @return
	 *	An array of all player-controllable characters in the world.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Characters")
	static TArray<TScriptInterface<IPF2CharacterInterface>> GetPlayerControlledCharacters(const UWorld* const World);

	/**
	 * Locates the actor component in this character that implements the specified interface.
	 *
	 * In development builds, an assertion error is raised if more than one component matches the given interface type.
	 * In shipping builds, only the first matching component is returned.
	 *
	 * @param [in]  Character
	 *	The character for which a component is desired.
	 * @param [in]  Interface
	 *	The type of interface to locate.
	 * @param [out] bWasFound
	 *	A reference to an output variable to receive the status of whether the component was found.
	 *
	 * @return
	 *	Either the component that matches the given interface; or, nullptr if there is no such component.
	 */
	UFUNCTION(BlueprintPure, Category = "OpenPF2|Characters|Components", meta = (DeterminesOutputType = "Interface"))
	static TScriptInterface<IInterface> GetComponentByInterface(
		const TScriptInterface<IPF2CharacterInterface>& Character,
		const TSubclassOf<UInterface>                   Interface,
		bool&                                           bWasFound);
};
