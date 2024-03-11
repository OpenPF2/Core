// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Items/PF2EquippedItemsComponent.h"

#include <Misc/DataValidation.h>

#include <Net/UnrealNetwork.h>

#include "Items/PF2EquipableItemSlot.h"

#include "Utilities/PF2ArrayUtilities.h"
#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

#define LOCTEXT_NAMESPACE "PF2EquipableItemSlot"

TArray<const UPF2EquipableItemSlot*> UPF2EquippedItemsComponent::GetTargetSlotsForSlotAndItem(
	const UPF2EquipableItemSlot*               Slot,
	const TScriptInterface<IPF2ItemInterface>& Item)
{
	TArray<const UPF2EquipableItemSlot*>              TargetSlots;
	const TArray<TSubclassOf<UPF2EquipableItemSlot>>& LinkedSlots = Slot->GetLinkedSlots();

	TargetSlots.Reserve(1 + LinkedSlots.Num());
	TargetSlots.Add(Slot);

	if (Item->ShouldBeEquippedInAllLinkedSlots() && !LinkedSlots.IsEmpty())
	{
		TargetSlots.Append(
			PF2ArrayUtilities::Map<UPF2EquipableItemSlot*>(
				LinkedSlots,
				[](const TSubclassOf<UPF2EquipableItemSlot> SlotType)
				{
					return SlotType.GetDefaultObject();
				}
			)
		);
	}

	return TargetSlots;
}

UPF2EquippedItemsComponent::UPF2EquippedItemsComponent() : Events(nullptr)
{
}

EDataValidationResult UPF2EquippedItemsComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result =
		CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	Result = CombineDataValidationResults(Result, ValidateSlots(Context));
	Result = CombineDataValidationResults(Result, ValidateEquippedItems(Context));

	return Result;
}

void UPF2EquippedItemsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPF2EquippedItemsComponent, SupportedSlots);
	DOREPLIFETIME(UPF2EquippedItemsComponent, EquippedItems);
}

UObject* UPF2EquippedItemsComponent::GetGenericEventsObject() const
{
	return this->GetEvents();
}

UPF2EquippedItemsInterfaceEvents* UPF2EquippedItemsComponent::GetEvents() const
{
	if (this->Events == nullptr)
	{
		// BUGBUG: This has to be instantiated here rather than via CreateDefaultSubobject() in the constructor, or it
		// breaks multiplayer. It seems that when created in the constructor, this component ends up as part of the CDO
		// and then all instances of this component share *one* events object, leading to all game clients being
		// notified about every multicast event broadcast for all instances. This typically results in a crash since the
		// addresses of callbacks aren't valid for clients who don't own the component handling the event.
		this->Events = NewObject<UPF2EquippedItemsInterfaceEvents>(
			const_cast<UPF2EquippedItemsComponent*>(this),
			FName(TEXT("InterfaceEvents"))
		);
	}

	return this->Events;
}

const TArray<UPF2EquipableItemSlot*> UPF2EquippedItemsComponent::GetSlots() const
{
	return PF2ArrayUtilities::Map<UPF2EquipableItemSlot*>(
		this->SupportedSlots,
		[](const TSubclassOf<UPF2EquipableItemSlot>& Slot)
		{
			return Slot.GetDefaultObject();
		}
	);
}

TScriptInterface<IPF2ItemInterface> UPF2EquippedItemsComponent::GetItemEquippedInSlot(
	const UPF2EquipableItemSlot* Slot) const
{
	TScriptInterface<IPF2ItemInterface> Result = TScriptInterface<IPF2ItemInterface>(nullptr);

	for (const auto& [CurrentSlotType, CurrentItemType] : this->EquippedItems)
	{
		if (CurrentSlotType.GetDefaultObject()->GetClass() == Slot->GetClass())
		{
			Result = CurrentItemType;
			break;
		}
	}

	return Result;
}

bool UPF2EquippedItemsComponent::IsItemOfTypeEquipped(const TSubclassOf<UDataAsset> ItemType) const
{
	bool Result = false;

	for (const auto& [_, CurrentItemType] : this->EquippedItems)
	{
		if (CurrentItemType->IsChildOf(ItemType.Get()))
		{
			Result = true;
			break;
		}
	}

	return Result;
}

void UPF2EquippedItemsComponent::GetAllEquippedItemsOfType(const TSubclassOf<UDataAsset>                ItemType,
                                                           TArray<TScriptInterface<IPF2ItemInterface>>& Items) const
{
	// Reserve slack for the worst-case scenario.
	Items.Empty(this->EquippedItems.Num());

	for (const auto& [_, CurrentItemType] : this->EquippedItems)
	{
		if (CurrentItemType->IsChildOf(ItemType.Get()))
		{
			// Add unique in case this is a two-handed item equipped in both hands.
			Items.AddUnique(CurrentItemType.GetDefaultObject());
		}
	}
}

void UPF2EquippedItemsComponent::GetAllSlotsThatAcceptType(const TSubclassOf<UDataAsset>   ItemType,
                                                           TArray<UPF2EquipableItemSlot*>& OutSlots) const
{
	// Reserve slack for the worst-case scenario.
	OutSlots.Empty(this->SupportedSlots.Num());

	for (TSubclassOf<UPF2EquipableItemSlot> Slot : this->SupportedSlots)
	{
		UPF2EquipableItemSlot* SlotCDO = Slot.GetDefaultObject();

		if (SlotCDO->WouldAcceptItemOfType(ItemType))
		{
			OutSlots.Add(SlotCDO);
		}
	}
}

void UPF2EquippedItemsComponent::EquipItemInSlot(const UPF2EquipableItemSlot*               Slot,
                                                 const TScriptInterface<IPF2ItemInterface>& Item)
{
	TArray<const UPF2EquipableItemSlot*> TargetSlots = GetTargetSlotsForSlotAndItem(Slot, Item);

	for (const auto& CurrentSlot : TargetSlots)
	{
		const FPF2EquippedItem EquippedItem(CurrentSlot->GetClass(), Item.GetObject()->GetClass());

		// Unequip any existing item in the slot.
		this->UnequipItemInSpecificSlot(CurrentSlot);
		this->EquippedItems.Add(EquippedItem);

		this->Native_OnItemEquipped(CurrentSlot, Item);
	}
}

void UPF2EquippedItemsComponent::UnequipItemInSlot(const UPF2EquipableItemSlot* Slot)
{
	// Create a copy of the equipped items before we modify them.
	const TArray<FPF2EquippedItem> OriginalEquippedItems = this->EquippedItems;

	for (auto EquippedItemIt = OriginalEquippedItems.CreateConstIterator(); EquippedItemIt; ++EquippedItemIt)
	{
		const auto& [CurrentSlotType, CurrentItemType] = *EquippedItemIt;

		if (CurrentSlotType == Slot->GetClass())
		{
			const UPF2EquipableItemSlot* CurrentSlot = Cast<UPF2EquipableItemSlot>(CurrentItemType.GetDefaultObject());

			// Update both the target slot and any linked slots, if the item is multi-slot and the slot has linked
			// slots.
			for (const auto& TargetSlot : GetTargetSlotsForSlotAndItem(CurrentSlot, CurrentItemType))
			{
				this->UnequipItemInSpecificSlot(TargetSlot);
			}
		}
	}
}

UActorComponent* UPF2EquippedItemsComponent::ToActorComponent()
{
	return this;
}

void UPF2EquippedItemsComponent::UnequipItemInSpecificSlot(const UPF2EquipableItemSlot* Slot)
{
	for (auto EquippedItemIt = this->EquippedItems.CreateConstIterator(); EquippedItemIt; ++EquippedItemIt)
	{
		const auto& [CurrentSlotType, CurrentItemType] = *EquippedItemIt;

		if (CurrentSlotType == Slot->GetClass())
		{
			// Though we are modifying the array while we're iterating over it, this is safe because we stop iterating
			// as soon as we have done the removal.
			this->EquippedItems.RemoveAt(EquippedItemIt.GetIndex());

			this->Native_OnItemUnequipped(CurrentSlotType.GetDefaultObject(), CurrentItemType.GetDefaultObject());
			break;
		}
	}
}

EDataValidationResult UPF2EquippedItemsComponent::ValidateSlots(FDataValidationContext& Context) const
{
	EDataValidationResult                      Result = EDataValidationResult::Valid;
	TArray<TSubclassOf<UPF2EquipableItemSlot>> UsedSlots;

	for (const TSubclassOf<UPF2EquipableItemSlot>& Slot : this->SupportedSlots)
	{
		if (!IsValid(Slot.Get()))
		{
			Result = EDataValidationResult::Invalid;

			Context.AddError(
				FText::Format(
					LOCTEXT("EmptySlot", "{0}: All slot types must be non-empty."),
					FText::FromString(this->GetName())
				)
			);
		}
		else if (UsedSlots.Contains(Slot))
		{
			Result = EDataValidationResult::Invalid;

			Context.AddError(
				FText::Format(
					LOCTEXT("DuplicateSlot", "{0}: Slot '{1}' has been specified multiple times."),
					FText::FromString(this->GetName()),
					Slot.GetDefaultObject()->GetSlotName()
				)
			);
		}
		else
		{
			UsedSlots.Add(Slot);
		}
	}

	return Result;
}

EDataValidationResult UPF2EquippedItemsComponent::ValidateEquippedItems(FDataValidationContext& Context) const
{
	EDataValidationResult                      Result     = EDataValidationResult::Valid;
	TArray<TSubclassOf<UPF2EquipableItemSlot>> UsedSlots;

	for (auto EquippedItemIt = this->EquippedItems.CreateConstIterator(); EquippedItemIt; ++EquippedItemIt)
	{
		const auto& [SlotType, ItemType] = *EquippedItemIt;

		if (IsValid(SlotType))
		{
			const int EntryIndex   = EquippedItemIt.GetIndex();
			bool      bIsSlotValid = false;

			if (UsedSlots.Contains(SlotType))
			{
				Result = EDataValidationResult::Invalid;

				Context.AddError(
					FText::Format(
						LOCTEXT("MultipleItemsInSameSlot", "{0}: Item equipped in index {1} references slot '{2}', which is already populated by a different item."),
						FText::FromString(this->GetName()),
						FText::AsNumber(EntryIndex),
						SlotType.GetDefaultObject()->GetSlotName()
					)
				);
			}
			else
			{
				UsedSlots.Add(SlotType);
			}

			for (const TSubclassOf<UPF2EquipableItemSlot>& CurrentSlot : this->SupportedSlots)
			{
				if (CurrentSlot == SlotType)
				{
					bIsSlotValid = true;
					break;
				}
			}

			if (!bIsSlotValid)
			{
				Result = EDataValidationResult::Invalid;

				Context.AddError(
					FText::Format(
						LOCTEXT("InvalidSlot", "{0}: Slot '{1}' referenced by the item equipped in index {2} is not listed in the 'Slots' property as a supported slot type."),
						FText::FromString(this->GetName()),
						SlotType.GetDefaultObject()->GetSlotName(),
						FText::AsNumber(EntryIndex)
					)
				);
			}
		}
		else
		{
			Result = EDataValidationResult::Invalid;

			Context.AddError(
				FText::Format(
					LOCTEXT("EmptyItemSlot", "{0}: All equipped items must specify a slot."),
					FText::FromString(this->GetName())
				)
			);
		}
	}

	return Result;
}

void UPF2EquippedItemsComponent::OnRep_EquippedItems(const TArray<FPF2EquippedItem>& PreviouslyEquippedItems)
{
	TArray<FPF2EquippedItem> NewUnequippedItems,
	                         NewEquippedItems;

	PF2ArrayUtilities::CaptureStructDeltas<FPF2EquippedItem>(
		PreviouslyEquippedItems,
		this->EquippedItems,
		[](const FPF2EquippedItem& First, const FPF2EquippedItem& Second)
		{
			return ((First.Item == Second.Item) && (First.Slot == Second.Slot));
		},
		NewUnequippedItems,
		NewEquippedItems
	);

	for (const auto& [Slot, Item] : NewUnequippedItems)
	{
		this->Native_OnItemUnequipped(Slot.GetDefaultObject(), Item.GetDefaultObject());
	}

	for (const auto& [Slot, Item] : NewEquippedItems)
	{
		this->Native_OnItemEquipped(Slot.GetDefaultObject(), Item.GetDefaultObject());
	}
}

void UPF2EquippedItemsComponent::Native_OnItemEquipped(const UPF2EquipableItemSlot*               Slot,
                                                       const TScriptInterface<IPF2ItemInterface>& EquippedItem)
{
	const FPF2EquippedItemComponentItemEquippedOrUnequippedDelegate ItemEquippedDelegate = this->GetEvents()->OnItemEquipped;

	UE_LOG(
		LogPf2CoreInventory,
		Verbose,
		TEXT("[%s] Item ('%s') equipped into slot ('%s') for character ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(EquippedItem->GetIdForLogs()),
		*(Slot->GetIdForLogs()),
		*(IPF2LogIdentifiableInterface::GetIdForLogs(this->GetOwner()))
	);

	if (ItemEquippedDelegate.IsBound())
	{
		ItemEquippedDelegate.Broadcast(this, Slot, EquippedItem);
	}
}

void UPF2EquippedItemsComponent::Native_OnItemUnequipped(const UPF2EquipableItemSlot*               Slot,
                                                         const TScriptInterface<IPF2ItemInterface>& UnequippedItem)
{
	const FPF2EquippedItemComponentItemEquippedOrUnequippedDelegate ItemUnequippedDelegate = this->GetEvents()->OnItemUnequipped;

	UE_LOG(
		LogPf2CoreInventory,
		Verbose,
		TEXT("[%s] Item ('%s') unequipped into slot ('%s') for character ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(UnequippedItem->GetIdForLogs()),
		*(Slot->GetIdForLogs()),
		*(IPF2LogIdentifiableInterface::GetIdForLogs(this->GetOwner()))
	);

	if (ItemUnequippedDelegate.IsBound())
	{
		ItemUnequippedDelegate.Broadcast(this, Slot, UnequippedItem);
	}
}

#undef LOCTEXT_NAMESPACE
