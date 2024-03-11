// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Engine/DataAsset.h>
#include <Engine/World.h>

#include <GameFramework/Info.h>

#include "Items/Weapons/PF2WeaponInterface.h"

#include "PF2EffectCauseWrapper.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class APF2EffectCauseWrapper;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * An actor to wrap weapons for replication as the "effect causer" of a gameplay effect.
 */
UCLASS(BlueprintType)
class APF2EffectCauseWrapper : public AInfo
{
	GENERATED_BODY()

	// =================================================================================================================
	// Private Fields
	// =================================================================================================================
	/**
	 * The weapon that this instance is wrapping.
	 *
	 * This is declared as a data asset (instead of an interface) for replication. UE will not replicate objects if they
	 * are declared/referenced through an interface property. The value of this property MUST implement
	 * IPF2WeaponInterface.
	 */
	UPROPERTY(Replicated)
	UDataAsset* Weapon;

public:
	// =================================================================================================================
	// Public Static Methods
	// =================================================================================================================
	/**
	 * Creates a new APF2EffectCauseWrapper for the given weapon.
	 *
	 * The given weapon instance must be a data asset that implements IPF2WeaponInterface.
	 *
	 * This method is necessary because actors cannot be passed parameters through a constructor at spawn time.
     *
     * @param OwningActor
     *	The actor who owns or possesses this weapon.
	 * @param Weapon
	 *	The weapon that the new instance will wrap.
	 *
	 * @return
	 *	The new damage cause wrapper.
	 */
	static APF2EffectCauseWrapper* Create(AActor* OwningActor, IPF2WeaponInterface* Weapon);

	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor.
	 */
	explicit APF2EffectCauseWrapper(): Weapon(nullptr)
	{
	}

	// =================================================================================================================
	// Public Methods - AActor Overrides
	// =================================================================================================================
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets the weapon that this instance is wrapping.
	 *
	 * @return
	 *	The weapon instance.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Effect Causers")
	FORCEINLINE TScriptInterface<IPF2WeaponInterface> GetWeapon()
	{
		return TScriptInterface<IPF2WeaponInterface>(this->Weapon);
	}

protected:
	/**
	 * Initializes the state of this actor to what has been provided and then finishes spawning the actor.
	 *
	 * This can only be called during spawning and must not be called after the actor has already been spawned.
	 *
	 * @param InWeapon
	 *	The weapon instance to wrap. This must be a data asset that implements the OpenPF2 weapon interface.
	 */
	void FinalizeConstruction(IPF2WeaponInterface* const InWeapon);
};
