// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Items/Weapons/PF2WeaponInterface.h"

#include "Abilities/PF2EffectCauseWrapper.h"

TScriptInterface<IPF2WeaponInterface> IPF2WeaponInterface::EffectCauserToWeapon(UObject* Object)
{
	if (Object != nullptr)
	{
		if (APF2EffectCauseWrapper* WeaponWrapper = Cast<APF2EffectCauseWrapper>(Object); WeaponWrapper != nullptr)
		{
			return WeaponWrapper->GetWeapon();
		}

		if (Object->Implements<UPF2WeaponInterface>())
		{
			return TScriptInterface<IPF2WeaponInterface>(Object);
		}
	}

	return TScriptInterface<IPF2WeaponInterface>(nullptr);
}
