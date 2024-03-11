// OpenPF2 for UE Game Logic, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2InteractableAbilityBase.h"

#include "Abilities/PF2EffectCauseWrapper.h"

#include "Utilities/PF2InterfaceUtilities.h"

FString UPF2InteractableAbilityBase::GetIdForLogs() const
{
	return FString::Format(
		TEXT("{0} ({1})"),
		{
			this->GetAbilityLabel().ToString(),
			this->GetName()
		}
	);
}

UGameplayAbility* UPF2InteractableAbilityBase::ToGameplayAbility()
{
	return Super::ToGameplayAbility();
}

FGameplayAbilitySpec UPF2InteractableAbilityBase::ToGameplayAbilitySpecForCharacter(
	const TScriptInterface<IPF2CharacterInterface> Character,
	bool&                                          bOutSpecFound) const
{
	return Super::ToGameplayAbilitySpecForCharacter(Character, bOutSpecFound);
}

FGameplayAbilitySpecHandle UPF2InteractableAbilityBase::ToGameplayAbilitySpecHandleForCharacter(
	const TScriptInterface<IPF2CharacterInterface> Character,
	bool&                                          bOutSpecHandleFound) const
{
	return Super::ToGameplayAbilitySpecHandleForCharacter(Character, bOutSpecHandleFound);
}

UTexture2D* UPF2InteractableAbilityBase::GetAbilityIcon() const
{
	return this->Icon;
}

FText UPF2InteractableAbilityBase::GetAbilityLabel() const
{
	return this->Label;
}

FText UPF2InteractableAbilityBase::GetAbilityDescription() const
{
	return this->Description;
}
