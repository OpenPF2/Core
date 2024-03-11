// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <AbilitySystemInterface.h>
#include <GameplayCueInterface.h>

#include <GameFramework/DefaultPawn.h>

#include "PF2TestPawn.generated.h"

class UAbilitySystemComponent;

/**
 * A pawn that holds an ability system component, for use in testing logic that relies on Attribute Sets and other
 * GAS-related functionality.
 */
UCLASS(Blueprintable, BlueprintType, notplaceable)
class OPENPF2CORE_API APF2TestPawn : public ADefaultPawn, public IGameplayCueInterface, public IAbilitySystemInterface
{
	GENERATED_UCLASS_BODY()

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	virtual void PostInitializeComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	/**
	 * The ability system component of this pawn (manipulated by tests).
	 */
	UPROPERTY(Category = AbilitySystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

public:
	/**
	 * The name of the ASC so that the sub-component can be looked-up by name in tests.
	 */
	static FName AbilitySystemComponentName;

	/**
	 * Gets the ASC of this pawn.
	 *
	 * @return
	 *	The Ability System Component this pawn manages.
	 */
	UAbilitySystemComponent* GetAbilitySystemComponent()
	{
		return this->AbilitySystemComponent;
	}
};
