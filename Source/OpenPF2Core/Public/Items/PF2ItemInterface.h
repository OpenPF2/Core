// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Engine/DataAsset.h>

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2ItemInterface.generated.h"

UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2ItemInterface : public UPF2LogIdentifiableInterface
{
    GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible items that characters can carry in inventory.
 */
class OPENPF2CORE_API IPF2ItemInterface : public IPF2LogIdentifiableInterface
{
    GENERATED_BODY()

public:
    /**
	 * Gets the type-name pair representing the primary asset ID for this item.
     *
     * @return
     *	The primary asset ID of this item.
     */
    UFUNCTION(BlueprintCallable, Category="OpenPF2|Items")
	virtual FPrimaryAssetId GetPrimaryAssetId() = 0;

	/**
	 * Gets the data asset that is implementing this interface.
	 *
	 * @return
	 *	This item, as a data asset.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Items")
	virtual UDataAsset* ToDataAsset() = 0;

    /**
     * Indicates when this item, when equipped or un-equipped, affects all slots linked with the target slot.
     *
     * For example, a two-handed weapon should affect both the left and right hand when equipped or un-equipped, whereas
     * a single-handed weapon should only affect the left or right hand (depending upon which has been target).
     *
     * @return
     *	- true if this weapon affects both the target slot and all the slots its linked to when the item is being
     *	  equipped or unequipped.
	 *	- false if this weapon only affects the target slot when it is being equipped or unequipped.
     */
    UFUNCTION(BlueprintCallable, Category="OpenPF2|Items")
	virtual bool ShouldBeEquippedInAllLinkedSlots() = 0;
};
