// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "GameplayEffects/Components/PF2AdvancedAdditionalEffectsGameplayEffectComponent.h"

#include <AbilitySystemComponent.h>

#include <Misc/DataValidation.h>

#include "GameplayEffects/Components/PF2ConditionalGameplayEffect.h"

#define LOCTEXT_NAMESPACE "PF2AdditionalEffectsGameplayEffectComponent"

UPF2AdvancedAdditionalEffectsGameplayEffectComponent::UPF2AdvancedAdditionalEffectsGameplayEffectComponent() :
	bOnApplicationCopyDataFromOriginalSpec(true)
{
}

#if WITH_EDITOR
EDataValidationResult UPF2AdvancedAdditionalEffectsGameplayEffectComponent::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult Result   = Super::IsDataValid(Context);
	const UGameplayEffect*      OwningGE = this->GetOwner();

	if ((OwningGE->DurationPolicy != EGameplayEffectDurationType::Instant) &&
		(OwningGE->Period.Value > 0.0f) &&
		(this->OnApplicationGameplayEffects.Num() > 0))
	{
		Context.AddWarning(
			LOCTEXT(
				"IsPeriodicAndHasOnApplication",
				"GE is periodic, but a component has 'On Application' GEs which will only be applied once."
			)
		);
	}

	return Result;
}
#endif

void UPF2AdvancedAdditionalEffectsGameplayEffectComponent::OnGameplayEffectApplied(
	FActiveGameplayEffectsContainer& ActiveGEContainer,
	FGameplayEffectSpec&             GESpec,
	FPredictionKey&                  PredictionKey) const
{
	const float                         EffectLevel         = GESpec.GetLevel();
	const FGameplayEffectContextHandle& EffectContextHandle = GESpec.GetEffectContext();
	UAbilitySystemComponent*            TargetAsc           = ActiveGEContainer.Owner;
	TArray<FGameplayEffectSpecHandle>   TargetEffectSpecs;

	if (!ensureMsgf(TargetAsc, TEXT("OnGameplayEffectApplied was passed an ActiveGEContainer that has a NULL ASC.")))
	{
		return;
	}

	for (const FPF2ConditionalGameplayEffect& ConditionalEffect : this->OnApplicationGameplayEffects)
	{
		const UGameplayEffect* GameplayEffectDef = ConditionalEffect.GetEffectClass().GetDefaultObject();

		if (GameplayEffectDef != nullptr)
		{
			const FGameplayTagContainer& SourceTags = GESpec.CapturedSourceTags.GetActorTags();
			const FGameplayTagContainer& TargetTags = GESpec.CapturedTargetTags.GetActorTags();

			if (ConditionalEffect.CanApply(EffectLevel, SourceTags, TargetTags))
			{
				FGameplayEffectSpecHandle SpecHandle;

				if (this->bOnApplicationCopyDataFromOriginalSpec)
				{
					SpecHandle = FGameplayEffectSpecHandle(new FGameplayEffectSpec());

					SpecHandle.Data->InitializeFromLinkedSpec(GameplayEffectDef, GESpec);
				}
				else
				{
					SpecHandle = ConditionalEffect.CreateSpec(EffectContextHandle, EffectLevel);
				}

				if (ensure(SpecHandle.IsValid()))
				{
					TargetEffectSpecs.Add(SpecHandle);
				}
			}
		}
	}

	for (const FGameplayEffectSpecHandle& TargetSpec : TargetEffectSpecs)
	{
		if (TargetSpec.IsValid())
		{
			TargetAsc->ApplyGameplayEffectSpecToSelf(*TargetSpec.Data.Get(), PredictionKey);
		}
	}
}

#undef LOCTEXT_NAMESPACE
