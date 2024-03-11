// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Libraries/PF2ActorLibrary.h"

#include <Components/ActorComponent.h>

#include <GameFramework/Actor.h>

#include <Templates/SubclassOf.h>

TScriptInterface<IInterface> UPF2ActorLibrary::GetComponentByInterface(const AActor*                 Actor,
                                                                       const TSubclassOf<UInterface> Interface,
                                                                       bool&                         bWasFound)
{
	TScriptInterface<IInterface>   Result;
	const TArray<UActorComponent*> Components = Actor->GetComponentsByInterface(Interface);

	checkf(
		Components.Num() < 2,
		TEXT("More than one component implements the same interface: %s"),
		*(Interface->GetName())
	);

	bWasFound = false;

	for (UActorComponent* Component : Components)
	{
		bWasFound = true;
		Result    = Component;
		break;
	}

	return Result;
}
