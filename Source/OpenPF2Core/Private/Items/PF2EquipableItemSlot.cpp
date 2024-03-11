// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Items/PF2EquipableItemSlot.h"

#include <Misc/DataValidation.h>

#define LOCTEXT_NAMESPACE "PF2EquipableItemSlot"

#if WITH_EDITOR
EDataValidationResult UPF2EquipableItemSlot::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result =
		CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	// Validate the slot name.
	{
		if (this->SlotName.IsEmptyOrWhitespace())
		{
			Result = EDataValidationResult::Invalid;

			Context.AddError(
				FText::Format(
					LOCTEXT("NullSlotName", "{0}: Slot name cannot be an empty string."),
					FText::FromString(this->GetName())
				)
			);
		}
	}

	// Validate the item type.
	{
		if (this->ItemType == nullptr)
		{
			Result = EDataValidationResult::Invalid;

			Context.AddError(
				FText::Format(
					LOCTEXT("NullItemType", "{0}: Slot type is required."),
					FText::FromString(this->GetName())
				)
			);
		}
	}

	// Validate the linked slots.
	{
		for (const auto& LinkedSlot : this->LinkedSlots)
		{
			if (LinkedSlot == this->GetClass())
			{
				Result = EDataValidationResult::Invalid;

				Context.AddError(
					FText::Format(
						LOCTEXT("SelfReferentialLinkedSlot", "{0}: A slot cannot have itself as a linked slot."),
						FText::FromString(this->GetName())
					)
				);
			}
		}
	}

	return Result;
}

FString UPF2EquipableItemSlot::GetIdForLogs() const
{
	return FString::Format(
		TEXT("{0}: Only {1} ({2})"),
		{
			*this->GetSlotName().ToString(),
			*IPF2LogIdentifiableInterface::GetIdForLogs(this->GetItemType()->GetDefaultObject()),
			*(this->GetName())
		}
	);
}
#endif

#undef LOCTEXT_NAMESPACE
