// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Engine/DataAsset.h>

#include <GameFramework/PlayerState.h>

#include <Kismet/BlueprintFunctionLibrary.h>

#include "PF2ConversionsLibrary.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class APF2EffectCauseWrapper;
class IPF2AbilitySystemInterface;
class IPF2ActorComponentInterface;
class IPF2CharacterInterface;
class IPF2ItemInterface;
class IPF2LogIdentifiableInterface;
class IPF2PlayerControllerInterface;
class IPF2PlayerStateInterface;
class UAbilitySystemComponent;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Function library for automatic conversions that improve Game Designer quality of life when working with OpenPF2.
 */
UCLASS()
class UPF2ConversionsLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Converts a log-identifiable object to a string.
	 *
	 * @param Object
	 *	The log-identifiable object to convert.
	 *
	 * @return
	 *	The resulting string.
	 */
	UFUNCTION(
		BlueprintPure,
		meta=(DisplayName = "To log ID string", CompactNodeTitle = "->", BlueprintAutocast),
		Category="OpenPF2|Conversions"
	)
	static FString Conv_LogIdentifiableIntfToString(const TScriptInterface<IPF2LogIdentifiableInterface> Object);

	/**
	 * Converts a log-identifiable object to text.
	 *
	 * @param Object
	 *	The log-identifiable object to convert.
	 *
	 * @return
	 *	The resulting text.
	 */
	UFUNCTION(
		BlueprintPure,
		meta=(DisplayName = "To log ID text", CompactNodeTitle = "->", BlueprintAutocast),
		Category="OpenPF2|Conversions"
	)
	static FText Conv_LogIdentifiableIntfToText(const TScriptInterface<IPF2LogIdentifiableInterface> Object);

	/**
	 * Converts a character to an actor.
	 *
	 * @param Character
	 *	The character interface instance to convert.
	 *
	 * @return
	 *	The resulting actor.
	 */
	UFUNCTION(
		BlueprintPure,
		meta=(DisplayName = "To Actor", CompactNodeTitle = "->", BlueprintAutocast),
		Category="OpenPF2|Conversions"
	)
	static AActor* Conv_CharacterIntfToActor(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Converts an ability system component interface to a standard ability system component.
	 *
	 * @param AscIntf
	 *	The ability system interface instance to convert.
	 *
	 * @return
	 *	The resulting ASC.
	 */
	UFUNCTION(
		BlueprintPure,
		meta=(DisplayName = "To Ability System Component", CompactNodeTitle = "->", BlueprintAutocast),
		Category="OpenPF2|Conversions"
	)
	static UAbilitySystemComponent* Conv_AscIntfToAsc(const TScriptInterface<IPF2AbilitySystemInterface>& AscIntf);

	/**
	 * Converts a character to a pawn.
	 *
	 * @param Character
	 *	The character interface instance to convert.
	 *
	 * @return
	 *	The resulting pawn.
	 */
	UFUNCTION(
		BlueprintPure,
		meta=(DisplayName = "To Pawn", CompactNodeTitle = "->", BlueprintAutocast),
		Category="OpenPF2|Conversions"
	)
	static APawn* Conv_CharacterIntfToPawn(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Converts a player controller interface to a standard controller actor.
	 *
	 * @param PlayerController
	 *	The player controller interface instance to convert.
	 *
	 * @return
	 *	The resulting controller.
	 */
	UFUNCTION(
		BlueprintPure,
		meta=(DisplayName = "To Controller", CompactNodeTitle = "->", BlueprintAutocast),
		Category="OpenPF2|Conversions"
	)
	static AController* Conv_PlayerControllerIntfToController(
		const TScriptInterface<IPF2PlayerControllerInterface>& PlayerController
	);

	/**
	 * Converts a player controller interface to a standard player controller actor.
	 *
	 * @param PlayerController
	 *	The player controller interface instance to convert.
	 *
	 * @return
	 *	The resulting player controller.
	 */
	UFUNCTION(
		BlueprintPure,
		meta=(DisplayName = "To Player Controller", CompactNodeTitle = "->", BlueprintAutocast),
		Category="OpenPF2|Conversions"
	)
	static APlayerController* Conv_PlayerControllerIntfToPlayerController(
		const TScriptInterface<IPF2PlayerControllerInterface>& PlayerController
	);

	/**
	 * Converts a player state interface to a standard player state actor.
	 *
	 * @param PlayerState
	 *	The player state interface instance to convert.
	 *
	 * @return
	 *	The resulting player state.
	 */
	UFUNCTION(
		BlueprintPure,
		meta=(DisplayName = "To Player State", CompactNodeTitle = "->", BlueprintAutocast),
		Category="OpenPF2|Conversions"
	)
	static APlayerState* Conv_PlayerStateIntfToPlayerState(
		const TScriptInterface<IPF2PlayerStateInterface>& PlayerState
	);

	/**
	 * Converts an actor component interface to a standard actor component.
	 *
	 * @param ActorComponent
	 *	The actor component interface instance to convert.
	 *
	 * @return
	 *	The resulting actor component.
	 */
	UFUNCTION(
		BlueprintPure,
		meta=(DisplayName = "To Actor Component", CompactNodeTitle = "->", BlueprintAutocast),
		Category="OpenPF2|Conversions"
	)
	static UActorComponent* Conv_ActorComponentIntfToActorComponent(
		const TScriptInterface<IPF2ActorComponentInterface>& ActorComponent
	);

	/**
	 * Converts an actor component interface to a standard actor component.
	 *
	 * @param Item
	 *	The item interface instance to convert.
	 *
	 * @return
	 *	The resulting data asset.
	 */
	UFUNCTION(
		BlueprintPure,
		meta=(DisplayName = "To Data Asset", CompactNodeTitle = "->", BlueprintAutocast),
		Category="OpenPF2|Conversions"
	)
	static UDataAsset* Conv_ItemIntfToDataAsset(const TScriptInterface<IPF2ItemInterface>& Item);
};
