// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Libraries/PF2ConversionsLibrary.h"

#include <Engine/World.h>

#include <GameFramework/PlayerController.h>

#include "PF2CharacterInterface.h"
#include "PF2PlayerStateInterface.h"

#include "Actors/Components/PF2AbilitySystemInterface.h"
#include "Actors/Components/PF2ActorComponentInterface.h"

#include "Items/PF2ItemInterface.h"

FString UPF2ConversionsLibrary::Conv_LogIdentifiableIntfToString(
	const TScriptInterface<IPF2LogIdentifiableInterface> Object)
{
	if (Object.GetInterface() == nullptr)
	{
		// This is neither an assertion error nor a logged error because Blueprints might use this conversion before a
		// call to "Is valid?" or something that can accept a nullptr.
		return FString();
	}
	else
	{
		return Object->GetIdForLogs();
	}
}

FText UPF2ConversionsLibrary::Conv_LogIdentifiableIntfToText(
	const TScriptInterface<IPF2LogIdentifiableInterface> Object)
{
	return FText::FromString(Conv_LogIdentifiableIntfToString(Object));
}

AActor* UPF2ConversionsLibrary::Conv_CharacterIntfToActor(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	if (Character.GetInterface() == nullptr)
	{
		// This is neither an assertion error nor a logged error because Blueprints might use this conversion before a
		// call to "Is valid?" or something that can accept a nullptr.
		return nullptr;
	}
	else
	{
		return Character->ToActor();
	}
}

UAbilitySystemComponent* UPF2ConversionsLibrary::Conv_AscIntfToAsc(
	const TScriptInterface<IPF2AbilitySystemInterface>& AscIntf)
{
	if (AscIntf.GetInterface() == nullptr)
	{
		// This is neither an assertion error nor a logged error because Blueprints might use this conversion before a
		// call to "Is valid?" or something that can accept a nullptr.
		return nullptr;
	}
	else
	{
		return AscIntf->ToAbilitySystemComponent();
	}
}

APawn* UPF2ConversionsLibrary::Conv_CharacterIntfToPawn(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	if (Character.GetInterface() == nullptr)
	{
		// This is neither an assertion error nor a logged error because Blueprints might use this conversion before a
		// call to "Is valid?" or something that can accept a nullptr.
		return nullptr;
	}
	else
	{
		return Character->ToPawn();
	}
}

AController* UPF2ConversionsLibrary::Conv_PlayerControllerIntfToController(
	const TScriptInterface<IPF2PlayerControllerInterface>& PlayerController)
{
	if (PlayerController.GetInterface() == nullptr)
	{
		// This is neither an assertion error nor a logged error because Blueprints might use this conversion before a
		// call to "Is valid?" or something that can accept a nullptr.
		return nullptr;
	}
	else
	{
		return PlayerController->ToPlayerController();
	}
}

APlayerController* UPF2ConversionsLibrary::Conv_PlayerControllerIntfToPlayerController(
	const TScriptInterface<IPF2PlayerControllerInterface>& PlayerController)
{
	if (PlayerController.GetInterface() == nullptr)
	{
		// This is neither an assertion error nor a logged error because Blueprints might use this conversion before a
		// call to "Is valid?" or something that can accept a nullptr.
		return nullptr;
	}
	else
	{
		return PlayerController->ToPlayerController();
	}
}

APlayerState* UPF2ConversionsLibrary::Conv_PlayerStateIntfToPlayerState(
	const TScriptInterface<IPF2PlayerStateInterface>& PlayerState)
{
	if (PlayerState.GetInterface() == nullptr)
	{
		// This is neither an assertion error nor a logged error because Blueprints might use this conversion before a
		// call to "Is valid?" or something that can accept a nullptr.
		return nullptr;
	}
	else
	{
		return PlayerState->ToPlayerState();
	}
}

UActorComponent* UPF2ConversionsLibrary::Conv_ActorComponentIntfToActorComponent(
	const TScriptInterface<IPF2ActorComponentInterface>& ActorComponent)
{
	if (ActorComponent.GetInterface() == nullptr)
	{
		// This is neither an assertion error nor a logged error because Blueprints might use this conversion before a
		// call to "Is valid?" or something that can accept a nullptr.
		return nullptr;
	}
	else
	{
		return ActorComponent->ToActorComponent();
	}
}

UDataAsset* UPF2ConversionsLibrary::Conv_ItemIntfToDataAsset(const TScriptInterface<IPF2ItemInterface>& Item)
{
	if (Item.GetInterface() == nullptr)
	{
		// This is neither an assertion error nor a logged error because Blueprints might use this conversion before a
		// call to "Is valid?" or something that can accept a nullptr.
		return nullptr;
	}
	else
	{
		return Item->ToDataAsset();
	}
}
