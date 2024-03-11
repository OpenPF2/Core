// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Items/PF2InventoryComponent.h"

#include <Engine/AssetManager.h>

#include <GameFramework/Actor.h>

#include <Net/UnrealNetwork.h>

#include <UObject/WeakInterfacePtr.h>

#include "Items/PF2ItemInterface.h"

#include "Utilities/PF2ArrayUtilities.h"
#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

UPF2InventoryComponent::UPF2InventoryComponent() : Events(nullptr)
{
	this->SetIsReplicatedByDefault(true);
}

void UPF2InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPF2InventoryComponent, InventoryItemIds);
}

UObject* UPF2InventoryComponent::GetGenericEventsObject() const
{
	return this->GetEvents();
}

UPF2InventoryInterfaceEvents* UPF2InventoryComponent::GetEvents() const
{
	if (this->Events == nullptr)
	{
		// BUGBUG: This has to be instantiated here rather than via CreateDefaultSubobject() in the constructor, or it
		// breaks multiplayer. It seems that when created in the constructor, this component ends up as part of the CDO
		// and then all instances of this component share *one* events object, leading to all game clients being
		// notified about every multicast event broadcast for all instances. This typically results in a crash since the
		// addresses of callbacks aren't valid for clients who don't own the component handling the event.
		this->Events = NewObject<UPF2InventoryInterfaceEvents>(
			const_cast<UPF2InventoryComponent*>(this),
			FName(TEXT("InterfaceEvents"))
		);
	}

	return this->Events;
}

TArray<TScriptInterface<IPF2ItemInterface>> UPF2InventoryComponent::GetContents() const
{
	return PF2InterfaceUtilities::ToScriptInterfaces(this->InventoryItemsLoaded);
}

void UPF2InventoryComponent::AddItem(const TScriptInterface<IPF2ItemInterface>& ItemToAdd)
{
	this->InventoryItemIds.AddUnique(ItemToAdd->GetPrimaryAssetId());

	this->Native_OnItemAddedToInventory(ItemToAdd);
	this->Native_OnInventoryChanged();
}

bool UPF2InventoryComponent::RemoveItem(const TScriptInterface<IPF2ItemInterface>& ItemToRemove)
{
	const int32 CountOfRemoved  = this->InventoryItemIds.Remove(ItemToRemove->GetPrimaryAssetId());
	const bool  bWasItemRemoved = (CountOfRemoved > 0);

	if (bWasItemRemoved)
	{
		this->Native_OnItemRemovedFromInventory(ItemToRemove);
		this->Native_OnInventoryChanged();
	}

	return bWasItemRemoved;
}

void UPF2InventoryComponent::ClearItems()
{
	this->InventoryItemIds.Empty();
}

UActorComponent* UPF2InventoryComponent::ToActorComponent()
{
	return this;
}

UAssetManager* UPF2InventoryComponent::GetAssetManager()
{
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized();

	if (AssetManager == nullptr)
	{
		UE_LOG(
			LogPf2CoreInventory,
			Error,
			TEXT("Inventory cannot be loaded because asset manager is not available.")
		);
	}

	return AssetManager;
}

void UPF2InventoryComponent::LoadItemsById(const TArray<FPrimaryAssetId>& ItemAssetIds,
                                           const FStreamableDelegate&     CompletionDelegate,
                                           const TArray<FName>&           BundlesToLoad)
{
	UAssetManager* AssetManager = GetAssetManager();

	if (AssetManager != nullptr)
	{
		AssetManager->LoadPrimaryAssets(
			ItemAssetIds,
			BundlesToLoad,
			CompletionDelegate
		);
	}
}

void UPF2InventoryComponent::OnRep_InventoryItemIds()
{
	this->LoadItemsById(this->InventoryItemIds, [this](const TArray<IPF2ItemInterface*>& LoadedItems)
	{
		this->Native_OnInventoryItemsLoaded(LoadedItems);
	});
}

void UPF2InventoryComponent::Native_OnInventoryItemsLoaded(const TArray<IPF2ItemInterface*>& NewInventory)
{
	const TArray<IPF2ItemInterface*>& OldInventory = this->InventoryItemsLoaded;
	TArray<IPF2ItemInterface*>        RemovedItems,
	                                  AddedItems;

	// BUGBUG: By the time we're here, this should definitely be an OpenPF2 item, but UE will sometimes replicate
	// entries in this->InventoryItems as NULL.
	PF2ArrayUtilities::CapturePtrDeltasWithCast(OldInventory, NewInventory, RemovedItems, AddedItems);

	// We execute this logic even if we have no registered listeners because we still need to do internal bookkeeping
	// when the queue changes.
	for (IPF2ItemInterface* const& RemovedItem : RemovedItems)
	{
		this->Native_OnItemRemovedFromInventory(PF2InterfaceUtilities::ToScriptInterface(RemovedItem));
	}

	for (IPF2ItemInterface* const& AddedItem : AddedItems)
	{
		this->Native_OnItemAddedToInventory(PF2InterfaceUtilities::ToScriptInterface(AddedItem));
	}

	if ((RemovedItems.Num() != 0) || (AddedItems.Num() != 0))
	{
		this->Native_OnInventoryChanged();
	}
}

void UPF2InventoryComponent::Native_OnInventoryChanged()
{
	const FPF2InventoryComponentInventoryChangedDelegate InventoryChangedDelegate = this->GetEvents()->OnInventoryChanged;

	if (InventoryChangedDelegate.IsBound())
	{
		TArray<TScriptInterface<IPF2ItemInterface>> InventoryItems;

		for (IPF2ItemInterface* Item : this->InventoryItemsLoaded)
		{
			// BUGBUG: By the time we're here, this should definitely be an OpenPF2 item, but UE will sometimes
			// replicate entries in this->InventoryItems as NULL.
			if (Item != nullptr)
			{
				InventoryItems.Add(PF2InterfaceUtilities::ToScriptInterface(Item));
			}
		}

		UE_LOG(
			LogPf2CoreInventory,
			VeryVerbose,
			TEXT("[%s] Character inventory changed ('%s') - %d elements."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(this->GetIdForLogs()),
			InventoryItems.Num()
		);

		InventoryChangedDelegate.Broadcast(this);
	}
	else
	{
		UE_LOG(
			LogPf2CoreInventory,
			Verbose,
			TEXT("[%s] Character inventory changed ('%s')."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(this->GetIdForLogs())
		);
	}
}

void UPF2InventoryComponent::Native_OnItemAddedToInventory(const TScriptInterface<IPF2ItemInterface>& AddedItem)
{
	const FPF2InventoryComponentItemAddedOrRemovedDelegate ItemAddedDelegate = this->GetEvents()->OnItemAddedToInventory;

	UE_LOG(
		LogPf2CoreInventory,
		Verbose,
		TEXT("[%s] Item ('%s') added to character inventory ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(AddedItem->GetIdForLogs()),
		*(this->GetIdForLogs())
	);

	if (ItemAddedDelegate.IsBound())
	{
		ItemAddedDelegate.Broadcast(this, AddedItem);
	}
}

void UPF2InventoryComponent::Native_OnItemRemovedFromInventory(const TScriptInterface<IPF2ItemInterface>& RemovedItem)
{
	const FPF2InventoryComponentItemAddedOrRemovedDelegate ItemRemovedDelegate = this->GetEvents()->OnItemRemovedFromInventory;

	UE_LOG(
		LogPf2CoreInventory,
		Verbose,
		TEXT("[%s] Item ('%s') removed from character inventory ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(RemovedItem->GetIdForLogs()),
		*(this->GetIdForLogs())
	);

	if (ItemRemovedDelegate.IsBound())
	{
		ItemRemovedDelegate.Broadcast(this, RemovedItem);
	}
}
