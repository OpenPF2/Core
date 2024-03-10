// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayEffect.h>
#include <GameplayEffectComponent.h>

#include "PF2AdvancedAdditionalEffectsGameplayEffectComponent.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
struct FPF2ConditionalGameplayEffect;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * A Gameplay Effects (GE) component that specifies additional GEs that can be activated under certain conditions.
 *
 * This is modeled after the stock "Additional Effects Gameplay Effect Component" provided by the engine, but with
 * support for both: 1) tags that the source MUST NOT have and/or 2) a tag query for more complex business requirements.
 * In contrast, the stock component provides only the ability to specify tags that the source MUST have for a
 * conditional GE to be applied, whereas this component supports three different levels of requirements (required,
 * ignored, and tag query) that all must be satisfied for a conditional GE to apply.
 *
 * There is an additional difference between this component and the stock component. Since the stock component does not
 * support any tags being specified for "On Complete" GEs, this version of the component does not support "On Complete"
 * GEs at all, since it would not be improving on the original component; if you need "On Complete" GEs, use the stock
 * component for that instead. The two components are not mutually exclusive, so both components can be used in the same
 * GE.
 */
UCLASS(CollapseCategories, DisplayName="Advanced Additional Effects on Application Gameplay Effect Component")
class OPENPF2CORE_API UPF2AdvancedAdditionalEffectsGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

protected:
	/**
	 * Whether to pass along data from the Gameplay Effect (GE) spec that applied the owning GE.
	 *
	 * When true, this copies data like "Transient Aggregators" (a.k.a., "Temporary Variables" or "Set-by-caller
	 * magnitudes") from an invoking GE spec into each new spec created for each GE specified specified under
	 * "OnApplicationGameplayEffect" that gets applied.
	 */
	UPROPERTY(EditDefaultsOnly, Category="On Application")
	bool bOnApplicationCopyDataFromOriginalSpec;

	/**
	 * What Gameplay Effects (GE) to consider for application to a target when the owning GE gets applied to the target.
	 *
	 * Each GE is only applied if the source has all the required tags and has none of the disallowed tags.
	 */
	UPROPERTY(EditDefaultsOnly, Category="On Application")
	TArray<FPF2ConditionalGameplayEffect> OnApplicationGameplayEffects;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2AdvancedAdditionalEffectsGameplayEffectComponent.
	 */
	explicit UPF2AdvancedAdditionalEffectsGameplayEffectComponent();

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
#if WITH_EDITOR
	/**
	 * Validates that the configuration of this component is compatible with its duration policy.
	 *
	 * @param [out] Context
	 *	A reference to warnings and errors arising from validation.
	 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

	// =================================================================================================================
	// Public Native Event Callbacks
	// =================================================================================================================
	/**
	 * Native event callback invoked after the owning Gameplay Effect (GE) gets applied to a target.
	 *
	 * This is invoked for GEs of all duration policies, including instant GEs.
	 *
	 * This evaluates the conditions of "OnApplication" GEs and applies all conditional GEs for which conditions have
	 * been met.
	 *
     * @param ActiveGEContainer
     *	The GE container of the target ASC. This holds all of the GEs that are currently active on the target.
     * @param GESpec
     *	The specification for the GE instance that has been applied.
     * @param PredictionKey
     *	The prediction key being used for the GE application and activation.
	 */
	virtual void OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer,
	                                     FGameplayEffectSpec&             GESpec,
	                                     FPredictionKey&                  PredictionKey) const override;
};
