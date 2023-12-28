// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#pragma once

#include <Components/ActorComponent.h>

#include <Engine/AssetManager.h>

#include "PF2ActorComponentBase.h"
#include "PF2EventEmitterInterface.h"
#include "PF2EquippedItemsInterface.h"
#include "PF2ItemInterface.h"

#include "PF2EquippedItemsComponent.generated.h"

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * The binding/association between an equipment slot and an equipable item.
 */
USTRUCT(BlueprintType)
struct FPF2EquippedItem
{
	GENERATED_BODY()

	// =================================================================================================================
	// Public Fields
	// =================================================================================================================
	/**
	 * The slot into which the equipment has been equipped.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="OpenPF2 - Equipped Item")
	TSubclassOf<UPF2EquipableItemSlot> Slot;

	/**
	 * The item that has been equipped.
	 */
	UPROPERTY(
		BlueprintReadWrite,
		EditAnywhere,
		meta=(MustImplement="/Script/OpenPF2Core.PF2ItemInterface"),
		Category="OpenPF2 - Equipped Item"
	)
	TSubclassOf<UDataAsset> Item;

	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor.
	 */
	explicit FPF2EquippedItem(): Slot(nullptr), Item(nullptr)
	{
	}

	/**
	 * Creates an equipped item association between the given item slot and item.
	 *
	 * @param Slot
	 *	The slot into which the equipment is being equipped.
	 * @param Item
	 *	The item that is being equipped.
	 */
	explicit FPF2EquippedItem(const TSubclassOf<UPF2EquipableItemSlot> Slot, const TSubclassOf<UDataAsset> Item) :
		Slot(Slot),
		Item(Item)
	{
	}
};

/**
 * An actor component for managing the items that a character is holding in their hands or wearing on their person.
 */
UCLASS(ClassGroup="OpenPF2-Characters", Blueprintable)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2EquippedItemsComponent :
	public UPF2ActorComponentBase,
	public IPF2EventEmitterInterface,
	public IPF2EquippedItemsInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Static Methods
	// =================================================================================================================
	/**
	 * Gets all the slots that are affected when the provided item is equipped or unequipped in the specified slot.
	 *
	 * @param Slot
	 *	The slot being affected.
	 * @param Item
	 *	The item being equipped or unequipped into the slot.
	 *
	 * @return
	 *	A list of slots that should be affected by the item.
	 */
	static TArray<const UPF2EquipableItemSlot*> GetTargetSlotsForSlotAndItem(
		const UPF2EquipableItemSlot*               Slot,
		const TScriptInterface<IPF2ItemInterface>& Item);

	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The events object used for binding Blueprint callbacks to events from this component.
	 */
	UPROPERTY(Transient)
	mutable UPF2EquippedItemsInterfaceEvents* Events;

	/**
	 * All of the slots into which equipment can be equipped on the owning character.
	 *
	 * Equipment slots will vary depending upon the physical layout of the character. For example, a humanoid character
	 * will typically have armor slots for the torso, chest, neck, hips, and lower legs; weapon slots for the left and
	 * right arms; multiple ring slots for each finger; and a slot for a cloak.
	 */
	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere,
		Replicated,
		meta=(NoElementDuplicate),
		Category="OpenPF2 - Equipped Items"
	)
	TArray<TSubclassOf<UPF2EquipableItemSlot>> SupportedSlots;

	/**
	 * The equipment that the owning character has equipped within each slot.
	 *
	 * Each slot must correspond to a slot defined in the Slots property.
	 */
	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere,
		ReplicatedUsing=OnRep_EquippedItems,
		Category="OpenPF2 - Equipped Items"
	)
	TArray<FPF2EquippedItem> EquippedItems;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2EquippedItemsComponent.
	 */
	explicit UPF2EquippedItemsComponent();

	// =================================================================================================================
	// Public Methods - UObject Overrides
	// =================================================================================================================
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	// =================================================================================================================
	// Public Methods - UActorComponent Overrides
	// =================================================================================================================
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods - IPF2EventEmitterInterface Implementation
	// =================================================================================================================
	virtual UObject* GetGenericEventsObject() const override;

	// =================================================================================================================
	// Public Methods - IPF2EquippedItemsInterface Implementation
	// =================================================================================================================
	virtual UPF2EquippedItemsInterfaceEvents* GetEvents() const override;
	virtual const TArray<UPF2EquipableItemSlot*> GetSlots() const override;
	virtual TScriptInterface<IPF2ItemInterface> GetItemEquippedInSlot(const UPF2EquipableItemSlot* Slot) const override;
	virtual bool IsItemOfTypeEquipped(const TSubclassOf<UDataAsset> ItemType) const override;

	virtual void GetAllEquippedItemsOfType(const TSubclassOf<UDataAsset>                ItemType,
	                                       TArray<TScriptInterface<IPF2ItemInterface>>& Items) const override;

	virtual void GetAllSlotsThatAcceptType(const TSubclassOf<UDataAsset>   ItemType,
	                                       TArray<UPF2EquipableItemSlot*>& OutSlots) const override;

	virtual void EquipItemInSlot(const UPF2EquipableItemSlot*               Slot,
	                             const TScriptInterface<IPF2ItemInterface>& Item) override;

	virtual void UnequipItemInSlot(const UPF2EquipableItemSlot* Slot) override;

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

	/**
	 * Removes the item (if any) that's in the specified slot, without affected linked slots.
	 *
	 * @param Slot
	 *	The slot to affect.
	 */
	virtual void UnequipItemInSpecificSlot(const UPF2EquipableItemSlot* Slot);

	#if WITH_EDITOR
	/**
	 * Validates that the SupportedSlots property contains valid data from the editor.
	 *
	 * @param [out] Context
	 *	A reference to warnings and errors arising from validation.
	 *
	 * @return
	 *	Whether the slots are valid.
	 */
	EDataValidationResult ValidateSlots(FDataValidationContext& Context) const;

	/**
	 * Validates that the EquippedItems property contains valid data from the editor.
	 *
	 * The equipped items are valid as long as:
	 * 1. They only reference slots that this component has been configured to accept (e.g., armor is only equipped
	 *    if there is an armor slot for the owning character).
	 * 2. The same slot is referenced no more than once (e.g., the owning character doesn't have two pieces of armor
	 *    equipped in the same armor slot).
	 *
	 * @param [out] Context
	 *	A reference to warnings and errors arising from validation.
	 *
	 * @return
	 *	Whether the equipped items are valid.
	 */
	EDataValidationResult ValidateEquippedItems(FDataValidationContext& Context) const;
	#endif

	// =================================================================================================================
	// Protected Replication Callbacks
	// =================================================================================================================
	/**
	 * Notifies this component that the contents of inventory have been replicated.
	 *
	 * @param PreviouslyEquippedItems
	 *	The items that were previously equipped.
	 */
	UFUNCTION()
	void OnRep_EquippedItems(const TArray<FPF2EquippedItem>& PreviouslyEquippedItems);

	// =================================================================================================================
	// Protected Native Event Notifications
	// =================================================================================================================
	/**
	 * Callback invoked when the owning character equips an item.
	 *
	 * If replication is enabled for this component, this is invoked on both the owning client and the server.
	 *
	 * @param Slot
	 *	The slot into which the item is being equipped.
	 * @param EquippedItem
	 *	The item that was equipped.
	 */
	void Native_OnItemEquipped(const UPF2EquipableItemSlot*               Slot,
	                           const TScriptInterface<IPF2ItemInterface>& EquippedItem);

	/**
	 * Callback invoked when the owning character unequips an item.
	 *
	 * If replication is enabled for this component, this is invoked on both the owning client and the server.
	 *
	 * @param Slot
	 *	The slot from which the item was unequipped.
	 * @param UnequippedItem
	 *	The item that was unequipped.
	 */
	void Native_OnItemUnequipped(const UPF2EquipableItemSlot*               Slot,
	                             const TScriptInterface<IPF2ItemInterface>& UnequippedItem);
};
