#pragma once

#include <Engine/DataAsset.h>

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2EquipableItemSlot.generated.h"

class IPF2ItemInterface;

/**
 * A representation of a single slot/spot on a character that can accept equipment (weapons, armor, etc.).
 *
 * Equipment slots will vary depending upon the physical layout of the character. For example, a humanoid character will
 * typically have armor slots for the torso, chest, neck, hips, and lower legs; weapon slots for the left and right
 * arms; multiple ring slots for each finger; and a slot for a cloak.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class UPF2EquipableItemSlot : public UDataAsset, public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The human-friendly name of this slot.
	 *
	 * For example:
	 * - "Armor - Torso"
	 * - "Armor - Chest"
	 * - "Hand - Left"
	 * - "Hand - Right"
	 * - "Ring - Left Hand - Finger 1"
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="OpenPF2 - Equippable Item Slot")
	FText SlotName;

	/**
	 * The optional human-friendly description of this slot.
	 *
	 * For example:
	 * - "Armor equipped to cover the torso of the character."
	 * - "Armor equipped to cover the chest of the character."
	 * - "The weapon equipped in the character's left hand (or both hands, if the weapon is two-handed)."
	 * - "The weapon equipped in the character's right hand (or both hands, if the weapon is two-handed)."
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="OpenPF2 - Equippable Item Slot")
	FText SlotDescription;

	/**
	 * The type of items that this slot can hold (e.g., weapons, armor, etc.).
	 */
	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere,
		meta=(MustImplement="/Script/OpenPF2Core.PF2ItemInterface", AllowAbstract),
		Category="OpenPF2 - Equippable Item Slot"
	)
	// ReSharper disable once CppRedundantTemplateArguments
	TSoftClassPtr<UDataAsset> ItemType;

	/**
	 * The optional item slot(s) (if any) to which this slot is linked.
	 *
	 * For example, humanoid characters can carry a two-handed weapon using both their left and right hands, so the
	 * two slots are linked to accommodate this use case.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="OpenPF2 - Equippable Item Slot")
	TArray<TSubclassOf<UPF2EquipableItemSlot>> LinkedSlots;

public:
	// =================================================================================================================
	// Public Methods - UObject Overrides
	// =================================================================================================================
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	virtual FString GetIdForLogs() const override;

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets the human-friendly name of this slot.
	 *
	 * @return
	 *	The name of this slot.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Item Slots")
	FORCEINLINE FText GetSlotName() const
	{
		return this->SlotName;
	}

	/**
	 * Gets the optional human-friendly description of this slot.
	 *
	 * @return
	 *	The description of this slot.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Item Slots")
	FORCEINLINE FText GetSlotDescription() const
	{
		return this->SlotDescription;
	}

	/**
	 * Gets the type of items that this slot can hold (e.g., weapons, armor, etc.).
	 *
	 * @return
	 *	The type of item allowed in this slot.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Item Slots")
	// ReSharper disable once CppRedundantTemplateArguments
	FORCEINLINE TSoftClassPtr<UObject> GetItemType() const
	{
		return this->ItemType;
	}

	/**
	 * Gets the optional item slot(s) to which this slot is linked (if any).
	 *
	 * For example, humanoid characters can carry a two-handed weapon using both their left and right hands, so the
	 * two slots are linked to accommodate this use case.
	 *
	 * @return
	 *	The linked slot(s), if any.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Item Slots")
	FORCEINLINE TArray<TSubclassOf<UPF2EquipableItemSlot>> GetLinkedSlots() const
	{
		return this->LinkedSlots;
	}

	/**
	 * Determines if this item slot would accept an item of the specified type.
	 *
	 * @param OtherItemType
	 *	The type to check.
	 *
	 * @return
	 *	- true if this slot accepts items of the specified type (or its sub-classes).
	 *	- false if this slot does not accept items of the specified type.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Item Slots")
	bool WouldAcceptItemOfType(
		UPARAM(meta=(MustImplement="/Script/OpenPF2Core.PF2ItemInterface", AllowAbstract))
		const TSubclassOf<UDataAsset> OtherItemType
	) const
	{
		return OtherItemType->IsChildOf(this->ItemType.Get());
	}
};
