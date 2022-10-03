﻿// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Libraries/PF2ConversionsLibrary.h"

#include <Engine/World.h>

#include <GameFramework/PlayerController.h>

#include "PF2CharacterInterface.h"
#include "PF2PlayerStateInterface.h"

AActor* UPF2ConversionsLibrary::Conv_CharacterToActor(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	if (Character == nullptr)
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

APawn* UPF2ConversionsLibrary::Conv_CharacterToPawn(const TScriptInterface<IPF2CharacterInterface>& Character)
{
	if (Character == nullptr)
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

AController* UPF2ConversionsLibrary::Conv_InterfaceToController(
	const TScriptInterface<IPF2PlayerControllerInterface>& PlayerController)
{
	if (PlayerController == nullptr)
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

APlayerController* UPF2ConversionsLibrary::Conv_InterfaceToPlayerController(
	const TScriptInterface<IPF2PlayerControllerInterface>& PlayerController)
{
	if (PlayerController == nullptr)
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

APlayerState* UPF2ConversionsLibrary::Conv_InterfaceToPlayerState(
	const TScriptInterface<IPF2PlayerStateInterface>& PlayerState)
{
	if (PlayerState == nullptr)
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