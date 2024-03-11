// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "Actors/Components/PF2ActorComponentInterface.h"

#include "PF2InventoryInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2InventoryInterface;
class IPF2CharacterInterface;
class IPF2ItemInterface;

// =====================================================================================================================
// Normal Declarations - Delegates
// =====================================================================================================================
/**
 * Delegate for Blueprints to react to a change in the contents of a character's inventory.
 *
 * @param InventoryComponent
 *	The component broadcasting this event.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FPF2InventoryComponentInventoryChangedDelegate,
	const TScriptInterface<IPF2InventoryInterface>&, InventoryComponent
);

/**
 * Delegate for Blueprints to react to an item being added to, or removed from, a character's inventory.
 *
 * @param InventoryComponent
 *	The component broadcasting this event.
 * @param InventoryItem
 *	The item being added to inventory or removed from inventory.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPF2InventoryComponentItemAddedOrRemovedDelegate,
	const TScriptInterface<IPF2InventoryInterface>&, InventoryComponent,
	const TScriptInterface<IPF2ItemInterface>&,      InventoryItem
);


// =====================================================================================================================
// Normal Declarations - Types
// =====================================================================================================================
/**
 * The "Events" object for PF2InventoryInterface.
 *
 * This is a concrete UObject that contains only the dynamic multicast delegates that instances of the interface expose
 * to consumers for binding.
 *
 * @see IPF2EventEmitterInterface
 */
UCLASS()
class OPENPF2CORE_API UPF2InventoryInterfaceEvents : public UObject
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Fields - Multicast Delegates
	// =================================================================================================================
	/**
	 * Event fired when the contents of a character's inventory have changed.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Inventory")
	FPF2InventoryComponentInventoryChangedDelegate OnInventoryChanged;

	/**
	 * Event fired when an item has been added to a character's inventory.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Inventory")
	FPF2InventoryComponentItemAddedOrRemovedDelegate OnItemAddedToInventory;

	/**
	 * Event fired when an item has been removed from a character's inventory.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Inventory")
	FPF2InventoryComponentItemAddedOrRemovedDelegate OnItemRemovedFromInventory;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2InventoryInterface : public UPF2ActorComponentInterface
{
	GENERATED_BODY()
};

/**
 * An interface for components that maintain items that a character is carrying with them.
 */
class OPENPF2CORE_API IPF2InventoryInterface : public IPF2ActorComponentInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets the events object used for binding Blueprint callbacks to events from this component.
	 *
	 * @return
	 *	The events object for this interface.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Inventory")
	virtual UPF2InventoryInterfaceEvents* GetEvents() const = 0;

	/**
	 * Gets all of the items that are in the inventory of the owning character.
	 *
	 * @return
	 *	The contents of inventory.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Inventory")
	virtual TArray<TScriptInterface<IPF2ItemInterface>> GetContents() const = 0;

	/**
	 * Adds an item to inventory.
	 *
	 * If the item already exists in inventory, this has no effect.
	 *
	 * @param ItemToAdd
	 *	The item to add to inventory.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Inventory")
	virtual void AddItem(const TScriptInterface<IPF2ItemInterface>& ItemToAdd) = 0;

	/**
	 * Removes an item from inventory.
	 *
	 * @param ItemToRemove
	 *	The item to remove from inventory.
	 *
	 * @return
	 *	- true if the item was removed from inventory.
	 *	- false if the item was not in inventory.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Inventory")
	virtual bool RemoveItem(const TScriptInterface<IPF2ItemInterface>& ItemToRemove) = 0;

	/**
	 * Clears all items from inventory.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Inventory")
	virtual void ClearItems() = 0;
};
