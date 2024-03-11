// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#pragma once

#include <EnhancedInputComponent.h>

#include <GameFramework/PlayerController.h>

#include "PF2PlayerControllerInterface.h"
#include "PF2PlayerControllerBase.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterCommandInterface;
class IPF2CharacterInterface;
class IPF2CharacterQueueInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Default base class for OpenPF2 Player Controllers.
 *
 * @see IPF2PlayerControllerInterface
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2PlayerControllerBase : public APlayerController, public IPF2PlayerControllerInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The sub-component that tracks which characters are controllable by this player.
	 *
	 * The characters in this queue must be affiliated with the same party as this player.
	 *
	 * This is declared as an actor component instead of an interface so that UE interacts with it properly for
	 * replication and in the editor. UE will not do this if this component were declared/referenced through an
	 * interface field.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UActorComponent* ControllableCharacterQueue;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	APF2PlayerControllerBase();

	// =================================================================================================================
	// Public Methods - AController Overrides
	// =================================================================================================================
	virtual void InitPlayerState() override;

	virtual void OnRep_PlayerState() override;

	// =================================================================================================================
	// Public Methods - APlayerController Overrides
	// =================================================================================================================
	virtual void PostInitializeComponents() override;

	virtual void SetPawn(APawn* InPawn) override;

	// =================================================================================================================
	// Public Methods - IPF2PlayerControllerInterface Implementation
	// =================================================================================================================
	virtual TScriptInterface<IPF2PlayerStateInterface> GetPlayerState() const override;

	virtual APlayerController* ToPlayerController() override;

	virtual FHitResult GetTargetSelection() const override;

	virtual void ClearTargetSelection() override;

	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetControllableCharacters() const override;

	virtual TScriptInterface<IPF2CharacterInterface> GetControlledCharacter() const override;

	virtual void Native_OnPlayableCharactersStarting(TScriptInterface<IPF2ModeOfPlayRuleSetInterface> RuleSet) override;

	virtual void Native_OnModeOfPlayChanged(EPF2ModeOfPlayType NewMode) override;

	virtual void GiveCharacter(const TScriptInterface<IPF2CharacterInterface>& GivenCharacter) override;

	virtual void ReleaseCharacter(const TScriptInterface<IPF2CharacterInterface>& ReleasedCharacter) override;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_ExecuteAbilityAsCharacterCommand(
		const TScriptInterface<IPF2InteractableAbilityInterface>& Ability,
		AActor*                                                   CharacterActor) override;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_ExecuteAbilitySpecAsCharacterCommand(const FGameplayAbilitySpecHandle AbilitySpecHandle,
	                                                         AActor*                          CharacterActor) override;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_ExecuteAbilitySpecAsCharacterCommandWithPayload(
		const FGameplayAbilitySpecHandle AbilitySpecHandle,
		AActor*                          CharacterActor,
		const FGameplayEventData&        AbilityPayload) override;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_CancelCharacterCommand(AInfo* Command) override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_OnEncounterTurnStarted() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_OnEncounterTurnEnded() override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	virtual FString GetIdForLogs() const override;

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the sub-component that keeps track of which character(s) this player can control and in what order.
	 *
	 * @return
	 *	The character queue sub-component.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Player Controllers")
	TScriptInterface<IPF2CharacterQueueInterface> GetCharacterQueue() const;

	/**
	 * Validates whether the given character is a controllable, OpenPF2-compatible pawn.
	 *
	 * @param CharacterActor
	 *	The character to validate.
	 *
	 * @return
	 *	- true if the character is a pawn, OpenPF2-compatible, and controlled by this player controller.
	 *	- false, otherwise.
	 */
	bool IsControllableCharacterPawn(AActor* CharacterActor) const;

	/**
	 * Gets the enhanced input component of this player controller.
	 *
	 * @return
	 *	The enhanced input component.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Player Controllers")
	UEnhancedInputComponent* GetEnhancedInputComponent() const;

	// =================================================================================================================
	// Protected Native Event Callbacks
	// =================================================================================================================
	/**
	 * Callback invoked in C++ code when the player state is available for use.
	 *
	 * - On the server: This happens just after the player state has been initialized.
	 * - On clients: This happens just after the player state has been replicated.
	 *
	 * @param NewPlayerState
	 *	The player state that was just made available.
	 */
	UFUNCTION()
	virtual void Native_OnPlayerStateAvailable(const TScriptInterface<IPF2PlayerStateInterface>& NewPlayerState);

	/**
	 * Callback invoked in C++ code when the player has taken ownership of a character.
	 *
	 * @param CharacterQueueComponent
	 *	The component maintaining the list of characters in this player controller.
	 * @param GivenCharacter
	 *	The character that is now controllable by the player.
	 */
	UFUNCTION()
	virtual void Native_OnCharacterGiven(
		const TScriptInterface<IPF2CharacterQueueInterface>& CharacterQueueComponent,
		const TScriptInterface<IPF2CharacterInterface>&      GivenCharacter
	);

	/**
	 * Callback invoked in C++ code when the player has been released of ownership of a character.
	 *
	* @param CharacterQueueComponent
	 *	The component maintaining the list of characters in this player controller.
	 * @param ReleasedCharacter
	 *	The character that is no longer controllable by the player.
	 */
	UFUNCTION()
	virtual void Native_OnCharacterReleased(
		const TScriptInterface<IPF2CharacterQueueInterface>& CharacterQueueComponent,
		const TScriptInterface<IPF2CharacterInterface>&      ReleasedCharacter
	);

	// =================================================================================================================
	// Blueprint Implementable Functions
	// =================================================================================================================
	/**
	 * Gets the last target location that the player has chosen through the UI.
	 *
	 * @return
	 *	The location in the map that the player has chosen for a target.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="Get Target Location")
	)
	FHitResult BP_GetTargetSelection() const;

	// =================================================================================================================
	// Blueprint Implementable Events
	// =================================================================================================================
	/**
	 * Blueprint event callback invoked in Blueprint when the player state is available for use.
	 *
	 * This is invoked on both the owning client and the server:
	 * - On the server: This happens just after the player state has been initialized.
	 * - On clients: This happens just after the player state has been replicated.
	 *
	 * @param NewPlayerState
	 *	The player state that was just made available.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Player State Available")
	)
	void BP_OnPlayerStateAvailable(const TScriptInterface<IPF2PlayerStateInterface>& NewPlayerState);

	/**
	 * Blueprint event callback invoked when the player has taken ownership of a character.
	 *
	 * This is invoked on both the owning client and the server.
	 *
	 * @param GivenCharacter
	 *	The character that is now controllable by the player.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Character Given")
	)
	void BP_OnCharacterGiven(const TScriptInterface<IPF2CharacterInterface>& GivenCharacter);

	/**
	 * Blueprint event invoke callback when the player has been released from ownership of a character.
	 *
	 * This is invoked on both the owning client and the server.
	 *
	 * @param ReleasedCharacter
	 *	The character that is no longer controllable by the player.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Character Released")
	)
	void BP_OnCharacterReleased(const TScriptInterface<IPF2CharacterInterface>& ReleasedCharacter);

	/**
	 * Blueprint event callback invoked when the mode of play has changed.
	 *
	 * This is invoked on both the owning client and the server.
	 *
	 * @param NewMode
	 *	The new mode of play.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Mode of Play Changed")
	)
	void BP_OnModeOfPlayChanged(EPF2ModeOfPlayType NewMode);

	/**
	 * BP event invoked when the pawn's turn during an encounter has started.
	 *
	 * This is invoked on both the owning client and the server.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Encounter Turn Started")
	)
	void BP_OnEncounterTurnStarted();

	/**
	 * BP event invoked when the pawn's turn during an encounter has ended.
	 *
	 * This is invoked on both the owning client and the server.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Encounter Turn Ended")
	)
	void BP_OnEncounterTurnEnded();

	/**
	 * BP event invoked when the last target location the player has chosen through the UI should be cleared.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Player Controllers",
		meta=(DisplayName="On Clear Target Location")
	)
	void BP_OnClearTargetSelection();
};
