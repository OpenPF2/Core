// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Libraries/PF2AbilitySystemLibrary.h"

#include <AbilitySystemGlobals.h>

#include "PF2CharacterInterface.h"
#include "PF2EffectCauseWrapper.h"
#include "PF2GameplayEffectContainerSpec.h"

#include "Items/Weapons/PF2WeaponInterface.h"

TScriptInterface<IPF2CharacterInterface> UPF2AbilitySystemLibrary::GetCausingCharacterFromGameplayEventPayload(
	const FGameplayEventData& EventData)
{
	TScriptInterface<IPF2CharacterInterface> Result;
	const UObject*                           Object        = EventData.OptionalObject.Get();
	IPF2CharacterInterface*                  CharacterIntf = Cast<IPF2CharacterInterface>(const_cast<UObject*>(Object));

	if (CharacterIntf == nullptr)
	{
		Result = TScriptInterface<IPF2CharacterInterface>(nullptr);
	}
	else
	{
		Result = PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);
	}

	return Result;
}

const UGameplayAbility* UPF2AbilitySystemLibrary::GetAbilityInstanceFromGameplayEventPayload(
	const FGameplayEventData& EventData)
{
	const UGameplayAbility*            Result        = nullptr;
	const FGameplayEffectContextHandle ContextHandle = EventData.ContextHandle;

	if (ContextHandle.IsValid())
	{
		Result = ContextHandle.GetAbilityInstance_NotReplicated();
	}

	return Result;
}

void UPF2AbilitySystemLibrary::DetermineDamageInstigatorAndSource(
	const FGameplayEffectSpec&                EffectSpec,
	TScriptInterface<IPF2CharacterInterface>& Instigator,
	AActor*&                                  DamageSource)
{
	const FGameplayEffectContextHandle EffectContext   = EffectSpec.GetContext();
	AActor*                            InstigatorActor = EffectContext.GetInstigator();

	// Initialize both to NULL to start.
	Instigator   = TScriptInterface<IPF2CharacterInterface>(nullptr);
	DamageSource = nullptr;

	// Initially, assume that the damage source is the instigator.
	if (IsValid(InstigatorActor))
	{
		Instigator   = InstigatorActor;
		DamageSource = InstigatorActor;
	}

	// If we have been given an explicit GE "causer", use that as the damage source instead of the instigator.
	if (EffectContext.GetEffectCauser() != nullptr)
	{
		DamageSource = EffectContext.GetEffectCauser();
	}
}

FGameplayEffectSpecHandle UPF2AbilitySystemLibrary::MakeGameplayEffectSpecFromGameplayEventContext(
	const TSubclassOf<UGameplayEffect> GameplayEffectClass,
	const FGameplayEventData&          EventData)
{
	FGameplayEffectSpecHandle Result          = FGameplayEffectSpecHandle();
	const UGameplayAbility*   AbilityInstance = GetAbilityInstanceFromGameplayEventPayload(EventData);

	if (AbilityInstance == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Warning,
			TEXT("MakeGameplayEffectSpecFromGameplayEventContext() invoked with insufficient context or a non-instanced GA.")
		);
	}
	else
	{
		// This is the ability from the source ASC (i.e., the ability from the source character that resulted in an
		// effect on the source. We are forwarding details about this ability activation into a new gameplay effect that
		// is being applied to the target as if it originated from the source.
		FGameplayAbilitySpec* AbilitySpec = AbilityInstance->GetCurrentAbilitySpec();

		if (AbilitySpec == nullptr)
		{
			UE_LOG(
				LogPf2CoreAbilities,
				Warning,
				TEXT("MakeGameplayEffectSpecFromGameplayEventContext() invoked a GA that has an invalid spec handle.")
			);
		}
		else
		{
			const FGameplayEffectContextHandle   SourceGeContextHandle = EventData.ContextHandle;
			const FGameplayAbilityActorInfo&     SourceActorInfo       = AbilityInstance->GetActorInfo();
			const UAbilitySystemComponent* const SourceAsc             = SourceActorInfo.AbilitySystemComponent.Get();

			// Effect context handle should be valid by now since it's a pre-req for getting the ability spec handle.
			check(SourceGeContextHandle.IsValid());

			const FGameplayEffectContextHandle NewEffectContext =
				MakeEffectContextForInstigatorAndCauser(
					AbilitySpec->Handle,
					SourceActorInfo,
					SourceGeContextHandle.GetInstigator(),
					SourceGeContextHandle.GetEffectCauser()
				);

			const FGameplayEffectSpecHandle NewEffectHandle =
				SourceAsc->MakeOutgoingSpec(
					GameplayEffectClass,
					SourceGeContextHandle.GetAbilityLevel(),
					NewEffectContext
				);

			if (NewEffectHandle.IsValid())
			{
				// Copy over the all-important source tags from the GA and its source.
				AbilitySpec->Ability->ApplyAbilityTagsToGameplayEffectSpec(
					*NewEffectHandle.Data.Get(),
					AbilitySpec
				);

				// Copy over set by caller magnitudes.
				NewEffectHandle.Data->SetByCallerTagMagnitudes = AbilitySpec->SetByCallerTagMagnitudes;

				Result = NewEffectHandle;
			}
			else
			{
				UE_LOG(
					LogPf2CoreAbilities,
					Error,
					TEXT("Failed to obtain handle for gameplay effect ('%s')."),
					*(GameplayEffectClass->GetName())
				);
			}
		}
	}

	return Result;
}

FGameplayEffectSpecHandle UPF2AbilitySystemLibrary::MakeGameplayEffectSpecForWeapon(
	const FGameplayAbilitySpecHandle&            AbilityHandle,
	const FGameplayAbilityActorInfo&             AbilityOwnerInfo,
	const TSubclassOf<UGameplayEffect>           GameplayEffectClass,
	const TScriptInterface<IPF2WeaponInterface>& Weapon,
	const float                                  Level)
{
	return MakeGameplayEffectSpecForCauser(
		AbilityHandle,
		AbilityOwnerInfo,
		GameplayEffectClass,
		Weapon->ToEffectCauser(AbilityOwnerInfo.OwnerActor.Get()),
		Level
	);
}

FGameplayEffectSpecHandle UPF2AbilitySystemLibrary::MakeGameplayEffectSpecForInstigatorAndCauser(
	const FGameplayAbilitySpecHandle&  AbilityHandle,
	const FGameplayAbilityActorInfo&   AbilityOwnerInfo,
	const TSubclassOf<UGameplayEffect> GameplayEffectClass,
	AActor*                            Instigator,
	AActor*                            EffectCauser,
	const float                        Level)
{
	const UAbilitySystemComponent* const SourceAsc = AbilityOwnerInfo.AbilitySystemComponent.Get();

	const FGameplayEffectContextHandle EffectContext =
		MakeEffectContextForInstigatorAndCauser(AbilityHandle, AbilityOwnerInfo, Instigator, EffectCauser);

	FGameplayEffectSpecHandle EffectHandle = SourceAsc->MakeOutgoingSpec(GameplayEffectClass, Level, EffectContext);

	if (EffectHandle.IsValid())
	{
		FGameplayAbilitySpec* AbilitySpec = SourceAsc->FindAbilitySpecFromHandle(AbilityHandle);

		if (AbilitySpec == nullptr)
		{
			UE_LOG(
				LogPf2CoreAbilities,
				Error,
				TEXT("Gameplay ability specification not found for handle ('%s')."),
				*(AbilityHandle.ToString())
			);
		}
		else
		{
			AbilitySpec->Ability->ApplyAbilityTagsToGameplayEffectSpec(*EffectHandle.Data.Get(), AbilitySpec);

			// Copy over set by caller magnitudes.
			EffectHandle.Data->SetByCallerTagMagnitudes = AbilitySpec->SetByCallerTagMagnitudes;
		}
	}
	else
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Failed to obtain handle for gameplay effect ('%s')."),
			*(GameplayEffectClass->GetName())
		);
	}

	return EffectHandle;
}

FGameplayEffectContextHandle UPF2AbilitySystemLibrary::MakeEffectContextForInstigatorAndCauser(
	const FGameplayAbilitySpecHandle& AbilityHandle,
	const FGameplayAbilityActorInfo&  AbilityOwnerInfo,
	AActor*                           Instigator,
	AActor*                           EffectCauser)
{
	FGameplayEffectContextHandle Context =
		FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());

	Context.AddInstigator(Instigator, EffectCauser);

	// Pass along the source ability object to the effect, as long as it is available.
	{
		const FGameplayAbilitySpec* AbilitySpec =
			AbilityOwnerInfo.AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);

		if (AbilitySpec == nullptr)
		{
			UE_LOG(
				LogPf2CoreAbilities,
				Error,
				TEXT("Gameplay ability specification not found for handle ('%s')."),
				*(AbilityHandle.ToString())
			);
		}
		else
		{
			// Track which ability is initiating the GE.
			Context.SetAbility(AbilitySpec->Ability);

			// Copy the ability source over to the GE.
			Context.AddSourceObject(AbilitySpec->SourceObject.Get());
		}
	}

	return Context;
}

FGameplayAbilityTargetDataHandle UPF2AbilitySystemLibrary::CreateAbilityTargetDataFromPlayerControllerTargetSelection(
	const TScriptInterface<IPF2PlayerControllerInterface> PlayerController)
{
	FGameplayAbilityTargetDataHandle Handle;

	if (PlayerController == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Error,
			TEXT("Cannot create ability target data from null player controller.")
		);
	}
	else
	{
		const FHitResult TargetSelection = PlayerController->GetTargetSelection();

		FGameplayAbilityTargetData_SingleTargetHit* TargetData =
			new FGameplayAbilityTargetData_SingleTargetHit(TargetSelection);

		Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(TargetData));
	}

	return Handle;
}

EPF2TargetSelectionType UPF2AbilitySystemLibrary::GetTargetSelectionType(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	const int32                             Index)
{
	EPF2TargetSelectionType           Result = EPF2TargetSelectionType::None;
	const FGameplayAbilityTargetData* Data   = TargetDataHandle.Get(Index);

	if (Data == nullptr)
	{
		// This is not logged as an error because it's perfectly reasonable for ability activations to have no target
		// data and we handle that gracefully by just returning "None".
		UE_LOG(
			LogPf2CoreAbilities,
			Verbose,
			TEXT("No data exists at the given index (%d) under GA target data handle (%d)."),
			Index,
			TargetDataHandle.UniqueId
		);
	}
	else
	{
		const FHitResult*             HitResult       = Data->GetHitResult();
		const AActor*                 TargetActor     = HitResult->GetActor();
		const IPF2CharacterInterface* TargetCharacter = Cast<IPF2CharacterInterface>(TargetActor);

		if (TargetCharacter != nullptr)
		{
			Result = EPF2TargetSelectionType::Character;
		}
		else if (HitResult->Location.IsNearlyZero())
		{
			// Ignore hit results with a location at the world origin (0,0,0), since that signals no hit.
			Result = EPF2TargetSelectionType::None;
		}
		else
		{
			Result = EPF2TargetSelectionType::Location;
		}
	}

	return Result;
}

FPF2GameplayEffectContainerSpec UPF2AbilitySystemLibrary::AddGameplayEffectSpecToEffectContainerSpec(
	const FPF2GameplayEffectContainerSpec& ContainerSpec,
	const FGameplayEffectSpecHandle& GameplayEffectSpec)
{
	FPF2GameplayEffectContainerSpec NewSpec = ContainerSpec;

	NewSpec.GameplayEffectSpecsToApply.Add(GameplayEffectSpec);

	return NewSpec;
}

FPF2GameplayEffectContainerSpec UPF2AbilitySystemLibrary::AddHitTargetsToEffectContainerSpec(
	const FPF2GameplayEffectContainerSpec& ContainerSpec,
	const TArray<FHitResult>& HitResults)
{
	FPF2GameplayEffectContainerSpec NewSpec = ContainerSpec;

	NewSpec.AddHitTargets(HitResults);

	return NewSpec;
}

FPF2GameplayEffectContainerSpec UPF2AbilitySystemLibrary::AddActorTargetsToEffectContainerSpec(
	const FPF2GameplayEffectContainerSpec& ContainerSpec,
	const TArray<AActor*>& TargetActors)
{
	FPF2GameplayEffectContainerSpec NewSpec = ContainerSpec;

	NewSpec.AddActorTargets(TargetActors);

	return NewSpec;
}

FPF2GameplayEffectContainerSpec UPF2AbilitySystemLibrary::AppendTargetsToEffectContainerSpec(
	const FPF2GameplayEffectContainerSpec&  ContainerSpec,
	const FGameplayAbilityTargetDataHandle& ExistingTargetDataHandle)
{
	FPF2GameplayEffectContainerSpec NewSpec = ContainerSpec;

	NewSpec.AppendTargets(ExistingTargetDataHandle);

	return NewSpec;
}

FAggregatorEvaluateParameters UPF2AbilitySystemLibrary::BuildEvaluationParameters(const FGameplayEffectSpec& ActivatingSpec)
{
	const FGameplayTagContainer * SourceTags           = ActivatingSpec.CapturedSourceTags.GetAggregatedTags(),
	                            * TargetTags           = ActivatingSpec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;

	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	return EvaluationParameters;
}
