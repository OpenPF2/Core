// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayAbilitySpec.h>

#include <UObject/ScriptInterface.h>

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2GameplayAbilityInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2GameplayAbilityInterface : public UPF2LogIdentifiableInterface
{
	GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible Gameplay Abilities.
 */
class OPENPF2CORE_API IPF2GameplayAbilityInterface : public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets the gameplay ability that is implementing this interface.
	 *
	 * @return
	 *	This gameplay ability, as a UObject.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	virtual UGameplayAbility* ToGameplayAbility() = 0;

	/**
	 * Locates and returns the ability spec in the given character's ASC that corresponds to this ability.
	 *
	 * @param Character
	 *	The character for which an ability spec is desired.
	 * @param bOutSpecFound
	 *	Whether a spec for this ability was found in the given character's ASC.
	 *
	 * @return
	 *	The specification for this ability in the ASC of the given character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	virtual FGameplayAbilitySpec ToGameplayAbilitySpecForCharacter(
		const TScriptInterface<IPF2CharacterInterface> Character,

		UPARAM(DisplayName="Spec Found")
		bool& bOutSpecFound) const = 0;

	/**
	 * Locates and returns the handle of the ability spec in the given character's ASC that corresponds to this ability.
	 *
	 * @param Character
	 *	The character for which an ability spec handle is desired.
	 * @param bOutSpecHandleFound
	 *	Whether a spec handle for this ability was found in the given character's ASC.
	 *
	 * @return
	 *	The handle for this ability in the ASC of the given character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	virtual FGameplayAbilitySpecHandle ToGameplayAbilitySpecHandleForCharacter(
		const TScriptInterface<IPF2CharacterInterface> Character,

		UPARAM(DisplayName="Spec Handle Found")
		bool& bOutSpecHandleFound) const = 0;
};
