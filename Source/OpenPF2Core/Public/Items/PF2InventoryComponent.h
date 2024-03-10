// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#pragma once

#include <Components/ActorComponent.h>

#include <Engine/AssetManager.h>

#include "PF2EventEmitterInterface.h"

#include "Actors/Components/PF2ActorComponentBase.h"

#include "Items/PF2InventoryInterface.h"
#include "Items/PF2ItemInterface.h"

#include "Utilities/PF2ArrayUtilities.h"

#include "PF2InventoryComponent.generated.h"

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * An actor component for managing the inventory of items that a character is carrying with them.
 */
UCLASS(ClassGroup="OpenPF2-Characters", meta=(BlueprintSpawnableComponent))
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2InventoryComponent :
	public UPF2ActorComponentBase,
	public IPF2EventEmitterInterface,
	public IPF2InventoryInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The events object used for binding Blueprint callbacks to events from this component.
	 */
	UPROPERTY(Transient)
	mutable UPF2InventoryInterfaceEvents* Events;

private:
	// =================================================================================================================
	// Private Fields
	// =================================================================================================================
	/**
	 * The IDs of items this character currently has in their inventory.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_InventoryItemIds)
	TArray<FPrimaryAssetId> InventoryItemIds;

	/**
	 * The locally cached, loaded copy of items this character currently has in their inventory.
	 *
	 * This is not replicated. It gets repopulated when the InventoryItems field replicates.
	 */
	UPROPERTY()
	TArray<IPF2ItemInterface*> InventoryItemsLoaded;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2InventoryComponent.
	 */
	explicit UPF2InventoryComponent();

	// =================================================================================================================
	// Public Methods - UActorComponent Overrides
	// =================================================================================================================
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods - IPF2EventEmitterInterface Implementation
	// =================================================================================================================
	virtual UObject* GetGenericEventsObject() const override;

	// =================================================================================================================
	// Public Methods - IPF2InventoryInterface Implementation
	// =================================================================================================================
	virtual UPF2InventoryInterfaceEvents* GetEvents() const override;
	virtual TArray<TScriptInterface<IPF2ItemInterface>> GetContents() const override;
	virtual void AddItem(const TScriptInterface<IPF2ItemInterface>& ItemToAdd) override;
	virtual bool RemoveItem(const TScriptInterface<IPF2ItemInterface>& ItemToRemove) override;
	virtual void ClearItems() override;

	// =================================================================================================================
	// Public Methods - IPF2ActorComponentInterface Implementation
	// =================================================================================================================
	virtual UActorComponent* ToActorComponent() override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	virtual FString GetIdForLogs() const override
	{
		return Super::GetIdForLogs();
	}

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	static UAssetManager* GetAssetManager();

	/**
	 * Loads multiple inventory item assets by their IDs, then invokes a lambda with the result.
	 *
	 * @param ItemAssetIds
	 *	The IDs of the inventory assets to load.
	 * @param CompletionCallback
	 *	Callback lambda to invoke once the items have loaded.
	 * @param BundlesToLoad
	 *	An optional restriction on the bundle of assets from which to load the items.
	 */
	template<typename FunctorType>
	void LoadItemsById(const TArray<FPrimaryAssetId>& ItemAssetIds,
	                   const FunctorType              CompletionCallback,
	                   const TArray<FName>&           BundlesToLoad = TArray<FName>())
	{
		FStreamableDelegate CompletionDelegate;

		CompletionDelegate.BindLambda([this, ItemAssetIds, CompletionCallback]()
		{
			const UAssetManager* AssetManager = GetAssetManager();

			check(AssetManager != nullptr);

			TArray<IPF2ItemInterface*> LoadedItems = PF2ArrayUtilities::ReduceToArray<IPF2ItemInterface*>(
				ItemAssetIds,
				[AssetManager](TArray<IPF2ItemInterface*> ResultArray, const FPrimaryAssetId& LoadedAssetId)
				{
					UObject*           AssetObject = AssetManager->GetPrimaryAssetObject(LoadedAssetId);
					IPF2ItemInterface* Item        = Cast<IPF2ItemInterface>(AssetObject);

					if (Item != nullptr)
					{
						ResultArray.Add(Item);
					}
				}
			);

			CompletionCallback(LoadedItems);
		});

		LoadItemsById(ItemAssetIds, CompletionDelegate, BundlesToLoad);
	}

	/**
	 * Loads multiple inventory item assets by their IDs, then invokes a completion delegate with the result.
	 *
	 * @param ItemAssetIds
	 *	The IDs of the inventory assets to load.
	 * @param CompletionDelegate
	 *	Callback to invoke once the items have loaded.
	 * @param BundlesToLoad
	 *	An optional restriction on the bundle of assets from which to load the items.
	 */
	static void LoadItemsById(const TArray<FPrimaryAssetId>& ItemAssetIds,
	                          const FStreamableDelegate&     CompletionDelegate,
	                          const TArray<FName>&           BundlesToLoad = TArray<FName>());

	// =================================================================================================================
	// Protected Replication Callbacks
	// =================================================================================================================
	/**
	 * Notifies this component that the contents of inventory have been replicated.
	 */
	UFUNCTION()
	void OnRep_InventoryItemIds();

	// =================================================================================================================
	// Protected Native Event Notifications
	// =================================================================================================================
	/**
	 * Callback invoked when inventory items have been loaded asynchronously by the asset manager.
	 *
	 * If replication is enabled for this component, this is invoked on both the owning client and the server.
	 *
	 * This dispatches appropriate inventory change, add, and remove callbacks.
	 *
	 * @param NewInventory
	 *	The items that were loaded.
	 */
	void Native_OnInventoryItemsLoaded(const TArray<IPF2ItemInterface*>& NewInventory);

	/**
	 * Callback invoked when inventory contents have changed (items added or removed, or inventory cleared).
	 *
	 * If replication is enabled for this component, this is invoked on both the owning client and the server.
	 */
	void Native_OnInventoryChanged();

	/**
	 * Callback invoked when an item has been added to inventory.
	 *
	 * If replication is enabled for this component, this is invoked on both the owning client and the server.
	 *
	 * @param AddedItem
	 *	The item that is now in inventory.
	 */
	void Native_OnItemAddedToInventory(const TScriptInterface<IPF2ItemInterface>& AddedItem);

	/**
	 * Callback invoked when an item has been removed from inventory.
	 *
	 * If replication is enabled for this component, this is invoked on both the owning client and the server.
	 *
	 * @param RemovedItem
	 *	The item that is no longer in inventory.
	 */
	void Native_OnItemRemovedFromInventory(const TScriptInterface<IPF2ItemInterface>& RemovedItem);

};
