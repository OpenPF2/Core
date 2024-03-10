// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/Tasks/PF2AbilityTaskBase.h"

#include "Abilities/PF2InteractableAbilityInterface.h"

#include "Actors/Components/PF2AbilitySystemInterface.h"

#include "CharacterStats/PF2CharacterAbilitySystemInterface.h"

#include "Utilities/PF2InterfaceUtilities.h"

TScriptInterface<IPF2InteractableAbilityInterface> UPF2AbilityTaskBase::GetAbility() const
{
	TScriptInterface<IPF2InteractableAbilityInterface> Result;

	IPF2InteractableAbilityInterface* AbilityIntf = Cast<IPF2InteractableAbilityInterface>(this->GetNativeAbility());

	if (AbilityIntf == nullptr)
	{
		Result = TScriptInterface<IPF2InteractableAbilityInterface>(nullptr);
	}
	else
	{
		Result = PF2InterfaceUtilities::ToScriptInterface(AbilityIntf);
	}

	return Result;
}

TScriptInterface<IPF2AbilitySystemInterface> UPF2AbilityTaskBase::GetAbilitySystemComponent() const
{
	TScriptInterface<IPF2AbilitySystemInterface> Result;

	IPF2AbilitySystemInterface* AscIntf = Cast<IPF2AbilitySystemInterface>(this->GetNativeAbilitySystemComponent());

	if (AscIntf == nullptr)
	{
		Result = TScriptInterface<IPF2AbilitySystemInterface>(nullptr);
	}
	else
	{
		Result = PF2InterfaceUtilities::ToScriptInterface(AscIntf);
	}

	return Result;
}

TScriptInterface<IPF2CharacterAbilitySystemInterface> UPF2AbilityTaskBase::GetCharacterAbilitySystemComponent() const
{
	TScriptInterface<IPF2CharacterAbilitySystemInterface> Result;

	IPF2CharacterAbilitySystemInterface* AscIntf =
		Cast<IPF2CharacterAbilitySystemInterface>(this->GetNativeAbilitySystemComponent());

	if (AscIntf == nullptr)
	{
		Result = TScriptInterface<IPF2CharacterAbilitySystemInterface>(nullptr);
	}
	else
	{
		Result = PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterAbilitySystemInterface>(AscIntf);
	}

	return Result;
}
