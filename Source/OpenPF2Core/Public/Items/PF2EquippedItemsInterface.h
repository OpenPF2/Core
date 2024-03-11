// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "Actors/Components/PF2ActorComponentInterface.h"

#include "PF2EquippedItemsInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2ItemInterface;
class UDataAsset;
class UPF2EquipableItemSlot;

// =====================================================================================================================
// Normal Declarations - Delegates
// =====================================================================================================================
/**
 * Delegate for Blueprints to react to an item being equipped or unequipped.
 *
 * @param EquippedItemsComponent
 *	The component broadcasting this event.
 * @param Slot
 *	The slot into which the item is being equipped.
 * @param Item
 *	The item that was equipped or unequipped.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FPF2EquippedItemComponentItemEquippedOrUnequippedDelegate,
	const TScriptInterface<IPF2EquippedItemsInterface>&, EquippedItemsComponent,
	const UPF2EquipableItemSlot*,                        Slot,
	const TScriptInterface<IPF2ItemInterface>&,          Item
);

// =====================================================================================================================
// Normal Declarations - Types
// =====================================================================================================================
/**
 * The "Events" object for PF2EquippedItemsInterface.
 *
 * This is a concrete UObject that contains only the dynamic multicast delegates that instances of the interface expose
 * to consumers for binding.
 *
 * @see IPF2EventEmitterInterface
 */
UCLASS()
class OPENPF2CORE_API UPF2EquippedItemsInterfaceEvents : public UObject
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Fields - Multicast Delegates
	// =================================================================================================================
	/**
	 * Event fired when an item has been equipped.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Equipped Items")
	FPF2EquippedItemComponentItemEquippedOrUnequippedDelegate OnItemEquipped;

	/**
	 * Event fired when an item has been unequipped.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Equipped Items")
	FPF2EquippedItemComponentItemEquippedOrUnequippedDelegate OnItemUnequipped;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2EquippedItemsInterface : public UPF2ActorComponentInterface
{
	GENERATED_BODY()
};

/**
 * An interface for components that maintain which items a character has equipped as a weapon, armor, shield, etc.
 *
 * Equipment slots will vary depending upon the physical layout of the character. For example, a humanoid character will
 * typically have armor slots for the torso, chest, neck, hips, and lower legs; weapon slots for the left and right
 * arms; multiple ring slots for each finger; and a slot for a cloak.
 */
class OPENPF2CORE_API IPF2EquippedItemsInterface : public IPF2ActorComponentInterface
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
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Equipped Items")
	virtual UPF2EquippedItemsInterfaceEvents* GetEvents() const = 0;

	/**
	 * Gets the slots into which equipment can be equipped.
	 *
	 * @return
	 *	The equipment slots supported by this component.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Equipped Items")
	virtual const TArray<UPF2EquipableItemSlot*> GetSlots() const = 0;

	/**
	 * Gets the item (if any) that is equipped in the specified slot.
	 *
	 * @param Slot
	 *	The slot for which an equipped item is desired.
	 *
	 * @return
	 *	The item that is equipped in the specified slot. If no item is equipped, this will be a script interface wrapper
	 *	around nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Equipped Items")
	virtual TScriptInterface<IPF2ItemInterface> GetItemEquippedInSlot(const UPF2EquipableItemSlot* Slot) const = 0;

	/**
	 * Determines whether the character has an item of the specified type equipped.
	 *
	 * @param ItemType
	 *	The type of item to check for.
	 *
	 * @return
	 *	- true if an item of the specified type has been equipped.
	 *	- false if there is no item of the specified type equipped.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Equipped Items")
	virtual bool IsItemOfTypeEquipped(
		UPARAM(meta=(MustImplement="/Script/OpenPF2Core.PF2ItemInterface", AllowAbstract))
		const TSubclassOf<UDataAsset> ItemType
	) const = 0;

	/**
	 * Gets all of the equipped items of the specified type.
	 *
	 * @param [in] ItemType
	 *	The type of item for which to search.
	 * @param [out] Items
	 *	A reference to the array that will receive all of the items that were found during the search.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Equipped Items")
	virtual void GetAllEquippedItemsOfType(
		UPARAM(meta=(MustImplement="/Script/OpenPF2Core.PF2ItemInterface", AllowAbstract))
		const TSubclassOf<UDataAsset> ItemType,

		TArray<TScriptInterface<IPF2ItemInterface>>& Items
	) const = 0;

	/**
	 * Gets all of the item slots that accept items of the specified type.
	 *
	 * @param [in] ItemType
	 *	The type of item for which to search.
	 * @param [out] OutSlots
	 *	A reference to the array that will receive all of the slots that were identified during the search.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Equipped Items")
	virtual void GetAllSlotsThatAcceptType(
		UPARAM(meta=(MustImplement="/Script/OpenPF2Core.PF2ItemInterface", AllowAbstract))
		const TSubclassOf<UDataAsset> ItemType,

		UPARAM(DisplayName="Slots")
		TArray<UPF2EquipableItemSlot*>& OutSlots
	) const = 0;

	/**
	 * Equips an item in the specified slot.
	 *
	 * If the slot already has an item equipped, it will be unequipped before equipping the new item. Callbacks will be
	 * fired for any item being unequipped as well as the item being equipped.
	 *
	 * @param Slot
	 *	The slot into which to equip the item.
	 * @param Item
	 *	The item to equip.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Equipped Items")
	virtual void EquipItemInSlot(const UPF2EquipableItemSlot*               Slot,
	                             const TScriptInterface<IPF2ItemInterface>& Item) = 0;

	/**
	 * Unequips an item from the specified slot, if it is populated.
	 *
	 * - If the slot has an item equipped, it will be unequipped and unequip callbacks will be fired.
	 * - If the slot has no item equipped, this method has no effect.
	 *
	 * @param Slot
	 *	The slot into which to equip the item.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Characters|Equipped Items")
	virtual void UnequipItemInSlot(const UPF2EquipableItemSlot* Slot) = 0;
};
