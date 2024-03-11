// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.
//

#include "Actors/Components/PF2ActorComponentBase.h"

#include <GameFramework/Actor.h>

FString UPF2ActorComponentBase::GetIdForLogs() const
{
	FString                             OwnerName;
	AActor*                             OwningActor = this->GetOwner();
	const IPF2LogIdentifiableInterface* OwnerLogId  = Cast<IPF2LogIdentifiableInterface>(OwningActor);

	if (OwnerLogId == nullptr)
	{
		// Fallback to vanilla UE if we're not in something that implements IPF2LogIdentifiableInterface
		OwnerName = GetNameSafe(OwningActor);
	}
	else
	{
		// Use the preferred log ID of the containing actor.
		OwnerName = OwnerLogId->GetIdForLogs();
	}

	// ReSharper disable CppRedundantParentheses
	return FString::Format(
		TEXT("{0}.{1}"),
		{
			*(OwnerName),
			*(this->GetName())
		}
	);
}
