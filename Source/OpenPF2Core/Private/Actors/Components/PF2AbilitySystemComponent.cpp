// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Actors/Components/PF2AbilitySystemComponent.h"

#include <UObject/ConstructorHelpers.h>

#include "PF2CharacterConstants.h"
#include "PF2CharacterInterface.h"

#include "Abilities/PF2InteractableAbilityInterface.h"

#include "Utilities/PF2ArrayUtilities.h"
#include "Utilities/PF2EnumUtilities.h"
#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

const FName UPF2AbilitySystemComponent::DefaultMovementAbilityTagName   = FName(TEXT("GameplayAbility.Type.DefaultMovement"));
const FName UPF2AbilitySystemComponent::DefaultFaceTargetAbilityTagName = FName(TEXT("GameplayAbility.Type.DefaultFaceTarget"));

UPF2AbilitySystemComponent::UPF2AbilitySystemComponent() : Events(nullptr), bAreAbilitiesAvailable(false)
{
	const FString DynamicTagsGeFilename =
		PF2CharacterConstants::GetBlueprintPath(
			*PF2CharacterConstants::GeDynamicTagsName,
			*PF2CharacterConstants::BlueprintSubfolderCharacterStats
		);

	const ConstructorHelpers::FObjectFinder<UClass> DynamicTagsGeFinder(*DynamicTagsGeFilename);

	this->DynamicTagsEffect = DynamicTagsGeFinder.Object;

	for (const auto& Ability : TEnumRange<EPF2CharacterAbilityScoreType>())
	{
		const FString AbilityName = PF2EnumUtilities::ToString(Ability);

		const FString Filename =
			PF2CharacterConstants::GetBlueprintPath(
				FName(FString::Format(*PF2CharacterConstants::GeBlueprintBoostNameFormat, {AbilityName})),
				PF2CharacterConstants::BlueprintSubfolderAbilityBoosts
			);

		const ConstructorHelpers::FObjectFinder<UClass> BoostGeFinder(*Filename);

		// Allow boost effects to be looked-up by ability name later.
		this->AbilityBoostEffects.Add(Ability, BoostGeFinder.Object);
	}
}

UObject* UPF2AbilitySystemComponent::GetGenericEventsObject() const
{
	return this->GetEvents();
}

UPF2AbilitySystemInterfaceEvents* UPF2AbilitySystemComponent::GetEvents() const
{
	if (this->Events == nullptr)
	{
		// BUGBUG: This has to be instantiated here rather than via CreateDefaultSubobject() in the constructor, or it
		// breaks multiplayer. It seems that when created in the constructor, this component ends up as part of the CDO
		// and then all instances of this component share *one* events object, leading to all game clients being
		// notified about every multicast event broadcast for all instances. This typically results in a crash since the
		// addresses of callbacks aren't valid for clients who don't own the component handling the event.
		this->Events = NewObject<UPF2AbilitySystemInterfaceEvents>(
			const_cast<UPF2AbilitySystemComponent*>(this),
			FName(TEXT("InterfaceEvents"))
		);
	}

	return this->Events;
}

TScriptInterface<IPF2InteractableAbilityInterface> UPF2AbilitySystemComponent::GetAbilityInstanceFromSpec(
	const FGameplayAbilitySpec& AbilitySpec) const
{
	TScriptInterface<IPF2InteractableAbilityInterface> AbilitySIntf;
	UGameplayAbility*                                  AbilityInstance;
	IPF2InteractableAbilityInterface*                  AbilityIntf = nullptr;

	// First, look for an instanced ability.
	AbilityInstance = AbilitySpec.GetPrimaryInstance();

	// If not instanced, get the CDO.
	if (AbilityInstance == nullptr)
	{
		AbilityInstance = AbilitySpec.Ability;
	}

	// Ensure we don't return an object that's marked for kill.
	if (IsValid(AbilityInstance))
	{
		AbilityIntf = Cast<IPF2InteractableAbilityInterface>(AbilityInstance);
	}

	if (AbilityIntf == nullptr)
	{
		AbilitySIntf = TScriptInterface<IPF2InteractableAbilityInterface>(nullptr);
	}
	else
	{
		AbilitySIntf = PF2InterfaceUtilities::ToScriptInterface(AbilityIntf);
	}

	return AbilitySIntf;
}

TArray<TScriptInterface<IPF2InteractableAbilityInterface>> UPF2AbilitySystemComponent::GetAbilities() const
{
	return PF2ArrayUtilities::ReduceToArray<TScriptInterface<IPF2InteractableAbilityInterface>>(
		this->GetActivatableAbilities(),
		[this](
			TArray<TScriptInterface<IPF2InteractableAbilityInterface>>& Abilities,
			const FGameplayAbilitySpec&                                 CurrentAbilitySpec)
		{
			const TScriptInterface<IPF2InteractableAbilityInterface> AbilityIntf =
				this->GetAbilityInstanceFromSpec(CurrentAbilitySpec);

			if (AbilityIntf != nullptr)
			{
				Abilities.Add(AbilityIntf);
			}
		});
}

TArray<TScriptInterface<IPF2InteractableAbilityInterface>> UPF2AbilitySystemComponent::GetAbilitiesByTags(
	const FGameplayTagContainer& Tags,
	bool bExactMatch) const
{
	return PF2ArrayUtilities::ReduceToArray<TScriptInterface<IPF2InteractableAbilityInterface>>(
		this->GetActivatableAbilities(),
		[this, &Tags, bExactMatch](
			TArray<TScriptInterface<IPF2InteractableAbilityInterface>>& Abilities,
			const FGameplayAbilitySpec&                                 CurrentAbilitySpec)
		{
			const TScriptInterface<IPF2InteractableAbilityInterface> AbilityIntf =
				this->GetAbilityInstanceFromSpec(CurrentAbilitySpec);

			if (AbilityIntf != nullptr)
			{
				const UGameplayAbility* Ability = AbilityIntf->ToGameplayAbility();

				if ((bExactMatch && Ability->AbilityTags.HasAll(Tags)) ||
					(!bExactMatch && Ability->AbilityTags.HasAny(Tags)))
				{
					Abilities.Add(AbilityIntf);
				}
			}
		});
}

bool UPF2AbilitySystemComponent::HasActiveAbilityWithTags(const FGameplayTagContainer& Tags)
{
	bool bResult = false;

	// We don't care if the matching ability is blocked; we only care if it's active.
	for (const FGameplayAbilitySpec& AbilitySpec : this->FindAbilitySpecsByTags(Tags, false))
	{
		if (AbilitySpec.IsActive())
		{
			// We only need a single match to know if an ability with those tags in active.
			bResult = true;
			break;
		}
	}

	return bResult;
}

FGameplayTagContainer UPF2AbilitySystemComponent::GetActiveGameplayTags() const
{
	FGameplayTagContainer Tags;

	Tags.Reset();

	this->GetOwnedGameplayTags(Tags);

	return Tags;
}

UAbilitySystemComponent* UPF2AbilitySystemComponent::ToAbilitySystemComponent()
{
	return Cast<UAbilitySystemComponent>(this);
}

TArray<FGameplayAbilitySpec> UPF2AbilitySystemComponent::FindAbilitySpecsByTags(
	const FGameplayTagContainer& Tags,
	const bool                   bOnlyAbilitiesThatSatisfyTagRequirements) const
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;

	this->GetActivatableGameplayAbilitySpecsByAllMatchingTags(
		Tags,
		AbilitiesToActivate,
		bOnlyAbilitiesThatSatisfyTagRequirements
	);

	return PF2ArrayUtilities::Map<FGameplayAbilitySpec>(
		AbilitiesToActivate,
		[](const FGameplayAbilitySpec* AbilityPtr)
		{
			return *AbilityPtr;
		}
	);
}

FGameplayAbilitySpec UPF2AbilitySystemComponent::FindAbilitySpecByTags(
	const FGameplayTagContainer& InTags,
	bool&                        bOutMatchFound,
	const bool                   bInOnlyAbilitiesThatSatisfyTagRequirements) const
{
	FGameplayAbilitySpec         MatchingAbility;
	TArray<FGameplayAbilitySpec> MatchingAbilities =
		this->FindAbilitySpecsByTags(InTags, bInOnlyAbilitiesThatSatisfyTagRequirements);

	if (MatchingAbilities.Num() == 0)
	{
		bOutMatchFound  = false;
		MatchingAbility = FGameplayAbilitySpec();
	}
	else
	{
		bOutMatchFound  = true;
		MatchingAbility = MatchingAbilities[0];
	}

	return MatchingAbility;
}

TArray<FGameplayAbilitySpecHandle> UPF2AbilitySystemComponent::FindAbilityHandlesByTags(
	const FGameplayTagContainer& Tags,
	const bool                   bOnlyAbilitiesThatSatisfyTagRequirements) const
{
	return PF2ArrayUtilities::Map<FGameplayAbilitySpecHandle>(
		this->FindAbilitySpecsByTags(Tags, bOnlyAbilitiesThatSatisfyTagRequirements),
		[](const FGameplayAbilitySpec& AbilitySpec)
		{
			return AbilitySpec.Handle;
		}
	);
}

FGameplayAbilitySpecHandle UPF2AbilitySystemComponent::FindAbilityHandleByTags(
	const FGameplayTagContainer& InTags,
	bool&                        bOutMatchFound,
	const bool                   bInOnlyAbilitiesThatSatisfyTagRequirements) const
{
	FGameplayAbilitySpecHandle Handle;
	const FGameplayAbilitySpec AbilitySpec =
		this->FindAbilitySpecByTags(InTags, bOutMatchFound, bInOnlyAbilitiesThatSatisfyTagRequirements);

	if (bOutMatchFound)
	{
		Handle = AbilitySpec.Handle;
	}
	else
	{
		Handle = FGameplayAbilitySpecHandle();
	}

	return Handle;
}

bool UPF2AbilitySystemComponent::TriggerAbilityWithPayload(const FGameplayAbilitySpecHandle AbilityHandle,
                                                           const FGameplayEventData         Payload)
{
	return this->TriggerAbilityFromGameplayEvent(
		AbilityHandle,
		this->AbilityActorInfo.Get(),
		FGameplayTag(),
		&Payload,
		*this
	);
}

void UPF2AbilitySystemComponent::AddPassiveGameplayEffect(const TSubclassOf<UGameplayEffect> Effect)
{
	const FName WeightGroup = PF2GameplayAbilityUtilities::GetWeightGroupOfGameplayEffect(Effect);

	this->AddPassiveGameplayEffectWithWeight(WeightGroup, Effect);
}

void UPF2AbilitySystemComponent::AddPassiveGameplayEffectWithWeight(
	const FName                        WeightGroup,
	const TSubclassOf<UGameplayEffect> Effect)
{
	// Special case: If this is the first time a GE from this weight group is being added, and other weight groups are
	// active, let's assume that we want to enable the new weight group.
	if ((this->PassiveGameplayEffects.Num(WeightGroup) == 0) && this->ArePassiveGameplayEffectsActive())
	{
		this->ActivatedWeightGroups.Add(WeightGroup);
	}

	this->InvokeAndReapplyPassiveGEsInSubsequentWeightGroups(WeightGroup, [this, WeightGroup, Effect]
	{
		this->PassiveGameplayEffects.Add(WeightGroup, Effect);
		this->CachedPassiveGameplayEffectsToApply.Empty();

		if (this->ActivatedWeightGroups.Contains(WeightGroup))
		{
			// Activate the new passive GE since it's being put into an active group.
			this->ActivatePassiveGameplayEffect(WeightGroup, Effect);
		}
	});
}

void UPF2AbilitySystemComponent::SetPassiveGameplayEffects(const TMultiMap<FName, TSubclassOf<UGameplayEffect>> Effects)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Effects]
	{
		this->PassiveGameplayEffects = Effects;
		this->CachedPassiveGameplayEffectsToApply.Empty();
	});
}

void UPF2AbilitySystemComponent::RemoveAllPassiveGameplayEffects()
{
	this->DeactivateAllPassiveGameplayEffects();

	this->PassiveGameplayEffects.Empty();
	this->CachedPassiveGameplayEffectsToApply.Empty();
}

void UPF2AbilitySystemComponent::ActivateAllPassiveGameplayEffects()
{
	const TMultiMap<FName, TSubclassOf<UGameplayEffect>> EffectsToApply = this->GetPassiveGameplayEffectsToApply();

	TSet<FName> AllWeightGroups,
	            InactiveGroups;

	EffectsToApply.GetKeys(AllWeightGroups);

	InactiveGroups = AllWeightGroups.Difference(this->ActivatedWeightGroups);

	for (const auto& InactiveGroup : InactiveGroups)
	{
		this->ActivatePassiveGameplayEffects(InactiveGroup);
	}
}

void UPF2AbilitySystemComponent::DeactivateAllPassiveGameplayEffects()
{
	FGameplayEffectQuery Query;

	Query.EffectSource = this;

	this->RemoveActiveEffects(Query);
	this->ActivatedWeightGroups.Empty();
}

TSet<FName> UPF2AbilitySystemComponent::ActivatePassiveGameplayEffectsAfter(const FName StartingWeightGroup)
{
	const TMultiMap<FName, TSubclassOf<UGameplayEffect>> EffectsToApply = this->GetPassiveGameplayEffectsToApply();

	TSet<FName> AllWeightGroups,
	            InactiveGroups,
	            ActivatedGroups;

	EffectsToApply.GetKeys(AllWeightGroups);

	InactiveGroups = AllWeightGroups.Difference(this->ActivatedWeightGroups);

	for (const auto& InactiveGroup : InactiveGroups)
	{
		if (StartingWeightGroup.LexicalLess(InactiveGroup) && this->ActivatePassiveGameplayEffects(InactiveGroup))
		{
			ActivatedGroups.Add(InactiveGroup);
		}
	}

	return ActivatedGroups;
}

TSet<FName> UPF2AbilitySystemComponent::DeactivatePassiveGameplayEffectsAfter(const FName StartingWeightGroup)
{
	TSet<FName> DeactivatedGroups;

	// We have to make a copy of the set because we'll be modifying it in the loop.
	TSet<FName> WeightGroups = this->ActivatedWeightGroups;

	for (const FName& ActiveGroup : WeightGroups)
	{
		if (StartingWeightGroup.LexicalLess(ActiveGroup) && this->DeactivatePassiveGameplayEffects(ActiveGroup))
		{
			DeactivatedGroups.Add(ActiveGroup);
		}
	}

	return DeactivatedGroups;
}

bool UPF2AbilitySystemComponent::ActivatePassiveGameplayEffects(const FName WeightGroup)
{
	if (this->ActivatedWeightGroups.Contains(WeightGroup))
	{
		return false;
	}
	else
	{
		const TMultiMap<FName, TSubclassOf<UGameplayEffect>> AllEffects   = this->GetPassiveGameplayEffectsToApply();
		TArray<TSubclassOf<UGameplayEffect>>                 GroupEffects;

		AllEffects.MultiFind(WeightGroup, GroupEffects, true);

		for (const auto& GameplayEffect : GroupEffects)
		{
			this->ActivatePassiveGameplayEffect(WeightGroup, GameplayEffect);
		}

		this->ActivatedWeightGroups.Add(WeightGroup);

		return true;
	}
}

bool UPF2AbilitySystemComponent::DeactivatePassiveGameplayEffects(const FName WeightGroup)
{
	if (!this->ActivatedWeightGroups.Contains(WeightGroup))
	{
		return false;
	}
	else
	{
		FGameplayEffectQuery Query;
		int32                NumRemoved;

		Query.EffectSource = this;

		Query.EffectTagQuery =
			FGameplayTagQuery::MakeQuery_MatchAnyTags(
				FGameplayTagContainer(PF2GameplayAbilityUtilities::GetTag(WeightGroup))
			);

		NumRemoved = this->RemoveActiveEffects(Query);

		this->ActivatedWeightGroups.Remove(WeightGroup);

		return (NumRemoved > 0);
	}
}

void UPF2AbilitySystemComponent::AddDynamicTag(const FGameplayTag Tag)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Tag]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Adding a dynamic tag ('%s') to ASC on character ('%s')."),
			*(Tag.ToString()),
			*(GetNameSafe(this->GetOwnerActor()))
		);

		this->DynamicTags.AddTag(Tag);
	});
}

void UPF2AbilitySystemComponent::AppendDynamicTags(const FGameplayTagContainer Tags)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Tags]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Adding dynamic tags ('%s') to ASC on character ('%s')."),
			*(Tags.ToString()),
			*(GetNameSafe(this->GetOwnerActor()))
		);

		this->DynamicTags.AppendTags(Tags);
	});
}

void UPF2AbilitySystemComponent::SetDynamicTags(const FGameplayTagContainer Tags)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Tags]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Setting all dynamic tags ('%s') in ASC on character ('%s')."),
			*(Tags.ToString()),
			*(GetNameSafe(this->GetOwnerActor()))
		);

		this->DynamicTags = Tags;
	});
}

void UPF2AbilitySystemComponent::RemoveDynamicTag(const FGameplayTag Tag)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Tag]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Removing a dynamic tag ('%s') from ASC on character ('%s')."),
			*(Tag.ToString()),
			*(GetNameSafe(this->GetOwnerActor()))
		);

		this->DynamicTags.RemoveTag(Tag);
	});
}

void UPF2AbilitySystemComponent::RemoveDynamicTags(const FGameplayTagContainer Tags)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Tags]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Removing dynamic tags ('%s') from ASC on character ('%s')."),
			*(Tags.ToString()),
			*(GetNameSafe(this->GetOwnerActor()))
		);

		this->DynamicTags.RemoveTags(Tags);
	});
}

void UPF2AbilitySystemComponent::RemoveAllDynamicTags()
{
	this->InvokeAndReapplyAllPassiveGEs([this]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Removing all dynamic tags from ASC on character ('%s')."),
			*(GetNameSafe(this->GetOwnerActor()))
		);

		this->DynamicTags.Reset();
	});
}

TScriptInterface<IPF2CharacterInterface> UPF2AbilitySystemComponent::GetCharacter() const
{
	IPF2CharacterInterface* OwningCharacter = Cast<IPF2CharacterInterface>(this->GetOwnerActor());

	if (OwningCharacter == nullptr)
	{
		return PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterInterface>(nullptr);
	}
	else
	{
		return PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterInterface>(OwningCharacter);
	}
}

FORCEINLINE int32 UPF2AbilitySystemComponent::GetCharacterLevel() const
{
	const TScriptInterface<IPF2CharacterInterface> OwningCharacter = this->GetCharacter();

	if (OwningCharacter.GetInterface() == nullptr)
	{
		return 1;
	}
	else
	{
		return OwningCharacter->GetCharacterLevel();
	}
}

TMap<EPF2CharacterAbilityScoreType, FPF2AttributeModifierSnapshot> UPF2AbilitySystemComponent::GetAbilityScoreValues() const
{
	const UPF2CharacterAttributeSet* AttributeSet =
		Cast<UPF2CharacterAttributeSet>(this->GetAttributeSubobject(UPF2CharacterAttributeSet::StaticClass()));

	const TMap<EPF2CharacterAbilityScoreType, FPF2AttributeModifierSnapshot> Values =
		{
			{
				EPF2CharacterAbilityScoreType::AbStrength,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbStrength.GetCurrentValue(),
					AttributeSet->AbStrengthModifier.GetCurrentValue()
				)
			},
			{
				EPF2CharacterAbilityScoreType::AbDexterity,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbDexterity.GetCurrentValue(),
					AttributeSet->AbDexterityModifier.GetCurrentValue()
				)
			},
			{
				EPF2CharacterAbilityScoreType::AbConstitution,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbConstitution.GetCurrentValue(),
					AttributeSet->AbConstitutionModifier.GetCurrentValue()
				)
			},
			{
				EPF2CharacterAbilityScoreType::AbIntelligence,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbIntelligence.GetCurrentValue(),
					AttributeSet->AbIntelligenceModifier.GetCurrentValue()
				)
			},
			{
				EPF2CharacterAbilityScoreType::AbWisdom,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbWisdom.GetCurrentValue(),
					AttributeSet->AbWisdomModifier.GetCurrentValue()
				)
			},
			{
				EPF2CharacterAbilityScoreType::AbCharisma,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbCharisma.GetCurrentValue(),
					AttributeSet->AbCharismaModifier.GetCurrentValue()
				)
			},
		};

	return Values;
}

TArray<TScriptInterface<IPF2AbilityBoostInterface>> UPF2AbilitySystemComponent::GetPendingAbilityBoosts() const
{
	TArray<TScriptInterface<IPF2AbilityBoostInterface>> MatchingGameplayAbilities;
	TArray<FGameplayAbilitySpec*>                       MatchingGameplayAbilitySpecs;

	this->GetActivatableGameplayAbilitySpecsByAllMatchingTags(
		FGameplayTagContainer(PF2GameplayAbilityUtilities::GetTag(FName("GameplayAbility.Type.AbilityBoost"))),
		MatchingGameplayAbilitySpecs,
		false
	);

	MatchingGameplayAbilities =
		PF2ArrayUtilities::Map<TScriptInterface<IPF2AbilityBoostInterface>>(
			MatchingGameplayAbilitySpecs,
			[](const FGameplayAbilitySpec* AbilitySpec)
			{
				IPF2AbilityBoostInterface* AbilityBoostIntf =
					Cast<IPF2AbilityBoostInterface>(AbilitySpec->Ability);

				checkf(
					AbilityBoostIntf != nullptr,
					TEXT("All ability boosts must implement IPF2AbilityBoostInterface.")
				);

				return PF2InterfaceUtilities::ToScriptInterface(AbilityBoostIntf);
			}
		);

	return MatchingGameplayAbilities;
}

void UPF2AbilitySystemComponent::ApplyAbilityBoost(const EPF2CharacterAbilityScoreType TargetAbilityScore)
{
	const TSubclassOf<UGameplayEffect> BoostEffect = this->AbilityBoostEffects[TargetAbilityScore];

	// Allow boost GE to override the default weight group.
	const FName WeightGroup =
		PF2GameplayAbilityUtilities::GetWeightGroupOfGameplayEffect(
			BoostEffect,
			PF2CharacterConstants::GeWeightGroups::AbilityBoosts
		);

	UE_LOG(
		LogPf2Core,
		VeryVerbose,
		TEXT("Applying a boost to ability ('%s') through ASC for character ('%s') via GE ('%s')."),
		*(PF2EnumUtilities::ToString(TargetAbilityScore)),
		*(GetNameSafe(this->GetOwnerActor())),
		*(GetNameSafe(BoostEffect))
	);

	this->AddPassiveGameplayEffectWithWeight(WeightGroup, BoostEffect);
}

bool UPF2AbilitySystemComponent::HasDefaultMovementAbility() const
{
	bool bHaveAbility = false;

	this->FindDefaultMovementAbilityHandle(bHaveAbility);

	return bHaveAbility;
}

FGameplayAbilitySpecHandle UPF2AbilitySystemComponent::FindDefaultMovementAbilityHandle(bool& bOutMatchFound) const
{
	const FGameplayTag          MovementTag = PF2GameplayAbilityUtilities::GetTag(DefaultMovementAbilityTagName);
	const FGameplayTagContainer SearchTags  = FGameplayTagContainer(MovementTag);

	return this->FindAbilityHandleByTags(SearchTags, bOutMatchFound, false);
}

bool UPF2AbilitySystemComponent::HasDefaultFaceTargetAbility() const
{
	bool bHaveAbility = false;

	this->FindDefaultFaceTargetAbilityHandle(bHaveAbility);

	return bHaveAbility;
}

FGameplayAbilitySpecHandle UPF2AbilitySystemComponent::FindDefaultFaceTargetAbilityHandle(bool& bOutMatchFound) const
{
	const FGameplayTag          FacingTag  = PF2GameplayAbilityUtilities::GetTag(DefaultFaceTargetAbilityTagName);
	const FGameplayTagContainer SearchTags = FGameplayTagContainer(FacingTag);

	return this->FindAbilityHandleByTags(SearchTags, bOutMatchFound, false);
}

UActorComponent* UPF2AbilitySystemComponent::ToActorComponent()
{
	return this;
}

FString UPF2AbilitySystemComponent::GetIdForLogs() const
{
	return this->GetFullName();
}

void UPF2AbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (this->AreAbilitiesAvailable())
	{
		// Nothing further to do.
		return;
	}

	for (const FGameplayAbilitySpec& Spec : this->ActivatableAbilities.Items)
	{
		const UGameplayAbility* SpecAbility = Spec.Ability;

		if (SpecAbility == nullptr)
		{
			// Return early, because abilities haven't fully replicated yet. The parent class will call us again using
			// a timer.
			return;
		}
	}

	// Prevent future event notifications for this instance.
	this->bAreAbilitiesAvailable = true;

	this->Native_OnAbilitiesAvailable();
}

TMultiMap<FName, TSubclassOf<UGameplayEffect>> UPF2AbilitySystemComponent::GetPassiveGameplayEffectsToApply()
{
	if (this->CachedPassiveGameplayEffectsToApply.Num() == 0)
	{
		this->CachedPassiveGameplayEffectsToApply = this->BuildPassiveGameplayEffectsToApply();
	}

	return this->CachedPassiveGameplayEffectsToApply;
}

TMultiMap<FName, TSubclassOf<UGameplayEffect>> UPF2AbilitySystemComponent::BuildPassiveGameplayEffectsToApply() const
{
	TMultiMap<FName, TSubclassOf<UGameplayEffect>> EffectsToApply = this->PassiveGameplayEffects;

	// Add a pseudo-GE for the dynamic tags.
	EffectsToApply.Add(PF2CharacterConstants::GeWeightGroups::InitializeBaseStats, this->DynamicTagsEffect);

	// Ensure Passive GEs are always evaluated in weight order.
	EffectsToApply.KeyStableSort(FNameLexicalLess());

	return EffectsToApply;
}

void UPF2AbilitySystemComponent::ActivatePassiveGameplayEffect(
	const FName                        WeightGroup,
	const TSubclassOf<UGameplayEffect> GameplayEffect)
{
	FGameplayEffectContextHandle EffectContext = this->MakeEffectContext();
	FGameplayEffectSpecHandle    NewHandle;
	FGameplayEffectSpec*         GameplayEffectSpec;

	EffectContext.AddSourceObject(this);

	NewHandle = this->MakeOutgoingSpec(
		GameplayEffect,
		this->GetCharacterLevel(),
		EffectContext
	);

	GameplayEffectSpec = NewHandle.Data.Get();

	// Ensure that the GE spec is tagged with its weight no matter how the weight was set (either through API or through
	// a tag in the InheritableGameplayEffectTags field on the GE definition class itself). Without this, only the tag
	// from the GE definition spec would pass through.
	GameplayEffectSpec->AddDynamicAssetTag(PF2GameplayAbilityUtilities::GetTag(WeightGroup));

	// Special case: If the GE being activated is our "dummy" GE for dynamic tags, apply tags to it.
	//
	// TODO: Find a different way to accomplish this without a GE. This feels very much like a kludge.
	if (GameplayEffect->GetName() == PF2CharacterConstants::GeDynamicTagsClassName)
	{
		GameplayEffectSpec->DynamicGrantedTags.AppendTags(this->DynamicTags);
	}

	if (NewHandle.IsValid())
	{
		this->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpec, this);
	}
}

void UPF2AbilitySystemComponent::Native_OnAbilitiesAvailable()
{
	const FPF2ClientAbilitiesLoadedDelegate& OnAbilitiesAvailable = this->GetEvents()->OnAbilitiesLoaded;

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] Abilities have replicated from the server for ASC [%s]."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetIdForLogs())
	);

	if (OnAbilitiesAvailable.IsBound())
	{
		OnAbilitiesAvailable.Broadcast(this);
	}
}

template <typename Func>
void UPF2AbilitySystemComponent::InvokeAndReapplyAllPassiveGEs(const Func Callable)
{
	const bool bWasActive = this->ArePassiveGameplayEffectsActive();

	if (bWasActive)
	{
		this->DeactivateAllPassiveGameplayEffects();
	}

	Callable();

	if (bWasActive)
	{
		this->ActivateAllPassiveGameplayEffects();
	}
}

template<typename Func>
void UPF2AbilitySystemComponent::InvokeAndReapplyPassiveGEsInSubsequentWeightGroups(
	const TSubclassOf<UGameplayEffect> Effect,
	const Func Callable)
{
	FName WeightGroup = PF2GameplayAbilityUtilities::GetWeightGroupOfGameplayEffect(Effect);

	this->InvokeAndReapplyPassiveGEsInSubsequentWeightGroups(WeightGroup, Callable);
}


template <typename Func>
void UPF2AbilitySystemComponent::InvokeAndReapplyPassiveGEsInSubsequentWeightGroups(
	const FName WeightGroup,
	const Func Callable)
{
	// NOTE: If the group we are affecting isn't active, we don't bother to re-apply subsequent groups because they
	// won't be affected.
	const bool bSubsequentGroupsWereActive =
		this->ActivatedWeightGroups.Contains(WeightGroup) &&
		(this->DeactivatePassiveGameplayEffectsAfter(WeightGroup).Num() != 0);

	Callable();

	if (bSubsequentGroupsWereActive)
	{
		this->ActivatePassiveGameplayEffectsAfter(WeightGroup);
	}
}
