// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayEffect.h>

#include <Templates/SubclassOf.h>

#include "PF2ConditionalGameplayEffect.generated.h"

/**
 * Struct for a Gameplay Effect (GE) that can be chained onto the application of another GE.
 *
 * This is modeled after the stock "FConditionalGameplayEffect" struct in the engine, but is more flexible. With this
 * implementation, game designers have greater control over when conditional GE are applied. Designers can specify:
 * 1. What tags the source MUST NOT have.
 * 2. What tags the TARGET MUST have.
 * 3. What tags the TARGET MUST NOT have.
 * 4. What tag query the source must satisfy.
 * 5. What tag query the target must satisfy.
 */
USTRUCT(BlueprintType)
struct OPENPF2CORE_API FPF2ConditionalGameplayEffect
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Public Fields
	// =================================================================================================================
	/**
	 * The conditional Gameplay Effect (GE) to apply to the target if the owning GE applies successfully to the target.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gameplay Effect")
	TSubclassOf<UGameplayEffect> EffectClass;

	/**
	 * All tags the source must have in order for the conditional Gameplay Effect (GE) to apply.
	 *
	 * If this is blank, the source is not required to have any tags in order for the conditional GE to apply. If the
	 * source has some but not all of these tags, the conditional GE is not applied.
	 *
	 * The source tags are snapshotted at the time that the owning GE gets applied. Therefore, they are not affected by
	 * other conditional GEs on the same owning GE.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Gameplay Effect|Source Requirements",
		meta=(DisplayName="Source Must Have Tags")
	)
	FGameplayTagContainer SourceRequiredTags;

	/**
	 * All tags the source must NOT have in order for the conditional Gameplay Effect (GE) to apply.
	 *
	 * None of these tags may be present. If the source has at least one of the listed tags, the conditional GE is not
	 * applied.
	 *
	 * The source tags are snapshotted at the time that the owning GE gets applied. Therefore, they are not affected by
	 * other conditional GEs on the same owning GE.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Gameplay Effect|Source Requirements",
		meta=(DisplayName="Source Must Not Have Tags")
	)
	FGameplayTagContainer SourceIgnoredTags;

	/**
	 * An optional, more complex query for controlling when the conditional Gameplay Effect (GE) can be applied.
	 *
	 * This enables more advanced logic than can be expressed with "Required Source Tags" / "Ignored Source Tags"
	 * alone.
	 *
	 * The source tags are snapshotted at the time that the owning GE gets applied. Therefore, they are not affected by
	 * other conditional GEs on the same owning GE.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Gameplay Effect|Source Requirements",
		meta=(DisplayName="Source Must Match Query")
	)
	FGameplayTagQuery SourceTagQuery;

	/**
	 * All tags the target must have in order for the conditional Gameplay Effect (GE) to apply.
	 *
	 * If this is blank, the target is not required to have any tags in order for the conditional GE to apply. If the
	 * target has some but not all of these tags, the conditional GE is not applied.
	 *
	 * The target tags are snapshotted at the time that the owning GE gets applied. Therefore, they are not affected by
	 * other conditional GEs on the same owning GE.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Gameplay Effect|Target Requirements",
		meta=(DisplayName="Target Must Have Tags")
	)
	FGameplayTagContainer TargetRequiredTags;

	/**
	 * All tags the target must NOT have in order for the conditional Gameplay Effect (GE) to apply.
	 *
	 * None of these tags may be present. If the target has at least one of the listed tags, the conditional GE is not
	 * applied.
	 *
	 * The target tags are snapshotted at the time that the owning GE gets applied. Therefore, they are not affected by
	 * other conditional GEs on the same owning GE.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Gameplay Effect|Target Requirements",
		meta=(DisplayName="Target Must Not Have Tags")
	)
	FGameplayTagContainer TargetIgnoredTags;

	/**
	 * An optional, more complex query for controlling when the conditional Gameplay Effect (GE) can be applied.
	 *
	 * This enables more advanced logic than can be expressed with "Required Target Tags" / "Ignored Target Tags"
	 * alone.
	 *
	 * The target tags are snapshotted at the time that the owning GE gets applied. Therefore, they are not affected by
	 * other conditional GEs on the same owning GE.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Gameplay Effect|Target Requirements",
		meta=(DisplayName="Target Must Match Query")
	)
	FGameplayTagQuery TargetTagQuery;

public:
	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets the conditional Gameplay Effect (GE) to apply to the target if the owning GE applies successfully.
	 *
	 * @return
	 *	The conditional Gameplay Effect (GE) to apply to the target if the owning GE applies successfully to the target.
	 */
	FORCEINLINE const TSubclassOf<UGameplayEffect>& GetEffectClass() const
	{
		return this->EffectClass;
	}

	/**
	 * Checks whether tags on the source and target meet all requirements, enabling the conditional GE to be applied.
	 *
	 * The source and target must:
	 * - Have all required tags (if any).
	 * - Must have none of the "ignored" tags (if any).
	 * - Must satisfy the tag query (if any).
	 *
	 * Tags on both the source and target are snapshotted at the time that the owning GE gets applied. Therefore, they
	 * are not affected by other conditional GEs on the same owning GE.
	 *
	 * @param EffectLevel
	 *	The level of the owning GE that is attempting to invoke the conditional GE.
	 * @param SourceTags
	 *	All of the tags on the source.
	 * @param TargetTags
	 *	All of the tags on the source.
	 *
	 * @return
	 *	true if the conditional GEs can be invoked because all source and target tags satisfy requirements; or, false,
	 *	otherwise.
	 */
	bool CanApply(const float                  EffectLevel,
	              const FGameplayTagContainer& SourceTags,
	              const FGameplayTagContainer& TargetTags) const;

	/**
	 * Creates a new Gameplay Effect (GE) spec for applying the conditional GE.
	 *
	 * @param EffectContext
	 *	The context of the owning GE that was applied and is the one activating the conditional GE.
	 * @param SourceLevel
	 *	The level of the owning GE that is invoking the conditional GE.
	 *
	 * @return
	 *	A new spec for the conditional GE.
	 */
	FGameplayEffectSpecHandle CreateSpec(const FGameplayEffectContextHandle& EffectContext,
	                                     const float                         SourceLevel) const;

	/**
	 * Overloads equality operator to ensure all fields are equal.
	 *
	 * @param Other
	 *	The other instance to which to compare.
	 *
	 * @return
	 *	true if the two instances have the same values in all fields; or, false, otherwise.
	 */
	bool operator==(const FPF2ConditionalGameplayEffect& Other) const;

	/**
	 * Overloads inequality operator to ensure at least one field is not equal.
	 *
	 * @param Other
	 *	The other instance to which to compare.
	 *
	 * @return
	 *	true if at least one field of the two instances do not have the same values; or, false, otherwise.
	 */
	bool operator!=(const FPF2ConditionalGameplayEffect& Other) const;
};
