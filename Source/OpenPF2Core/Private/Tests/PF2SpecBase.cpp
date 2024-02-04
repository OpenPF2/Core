// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Tests/PF2SpecBase.h"

#include <AbilitySystemBlueprintLibrary.h>

#include <Engine/Engine.h>

#include "Abilities/PF2CharacterAttributeSet.h"

#include "Libraries/PF2AbilitySystemLibrary.h"
#include "Libraries/PF2TagLibrary.h"

#include "Tests/PF2TestAbility.h"
#include "Tests/PF2TestCharacter.h"

#include "Utilities/PF2GameplayAbilityUtilities.h"
#include "Utilities/PF2InterfaceUtilities.h"

FPF2SpecBase::FPF2SpecBase(const FString& InName) :
	FPF2AutomationSpecBase(InName, false),
	World(nullptr),
	TestPawn(nullptr),
	TestPawnAsc(nullptr),
	TestCharacter(nullptr),
	TestCharacterAsc(nullptr)
{
}

FAttributeCapture FPF2SpecBase::CaptureAttributes(const UPF2CharacterAttributeSet* AttributeSet)
{
	FAttributeCapture Capture =
	{
		{ TEXT("Experience"),           const_cast<FGameplayAttributeData *>(&AttributeSet->Experience)           },
		{ TEXT("AbBoostCount"),         const_cast<FGameplayAttributeData *>(&AttributeSet->AbBoostCount)         },
		{ TEXT("AbBoostLimit"),         const_cast<FGameplayAttributeData *>(&AttributeSet->AbBoostLimit)         },
		{ TEXT("ClassDifficultyClass"), const_cast<FGameplayAttributeData *>(&AttributeSet->ClassDifficultyClass) },
		{ TEXT("Speed"),                const_cast<FGameplayAttributeData *>(&AttributeSet->Speed)                },
		{ TEXT("MaxSpeed"),             const_cast<FGameplayAttributeData *>(&AttributeSet->MaxSpeed)             },
		{ TEXT("Reach"),                const_cast<FGameplayAttributeData *>(&AttributeSet->Reach)                },
		{ TEXT("ArmorClass"),           const_cast<FGameplayAttributeData *>(&AttributeSet->ArmorClass)           },
		{ TEXT("HitPoints"),            const_cast<FGameplayAttributeData *>(&AttributeSet->HitPoints)            },
		{ TEXT("MaxHitPoints"),         const_cast<FGameplayAttributeData *>(&AttributeSet->MaxHitPoints)         },
		{ TEXT("PerceptionModifier"),   const_cast<FGameplayAttributeData *>(&AttributeSet->PerceptionModifier)   },
		{ TEXT("FeAncestryFeatCount"),  const_cast<FGameplayAttributeData *>(&AttributeSet->FeAncestryFeatCount)  },
		{ TEXT("FeAncestryFeatLimit"),  const_cast<FGameplayAttributeData *>(&AttributeSet->FeAncestryFeatLimit)  },
		{ TEXT("TmpDamageIncoming"),    const_cast<FGameplayAttributeData *>(&AttributeSet->TmpDamageIncoming)    },
	};

	Capture.Append(CaptureAbilityAttributes(AttributeSet));
	Capture.Append(CaptureAbilityModifierAttributes(AttributeSet));
	Capture.Append(CaptureSavingThrowModifierAttributes(AttributeSet));
	Capture.Append(CaptureSkillModifierAttributes(AttributeSet));
	Capture.Append(CaptureSpellAttributes(AttributeSet));

	return Capture;
}

FAttributeCapture FPF2SpecBase::CaptureAbilityAttributes(const UPF2CharacterAttributeSet* AttributeSet)
{
	FAttributeCapture Capture =
	{
		{ TEXT("AbCharisma"),     const_cast<FGameplayAttributeData *>(&AttributeSet->AbCharisma) },
		{ TEXT("AbConstitution"), const_cast<FGameplayAttributeData *>(&AttributeSet->AbConstitution) },
		{ TEXT("AbDexterity"),    const_cast<FGameplayAttributeData *>(&AttributeSet->AbDexterity) },
		{ TEXT("AbIntelligence"), const_cast<FGameplayAttributeData *>(&AttributeSet->AbIntelligence) },
		{ TEXT("AbStrength"),     const_cast<FGameplayAttributeData *>(&AttributeSet->AbStrength) },
		{ TEXT("AbWisdom"),       const_cast<FGameplayAttributeData *>(&AttributeSet->AbWisdom) },
	};

	return Capture;
}

FAttributeCapture FPF2SpecBase::CaptureAbilityModifierAttributes(const UPF2CharacterAttributeSet* AttributeSet)
{
	FAttributeCapture Capture =
	{
		{ TEXT("AbCharismaModifier"),     const_cast<FGameplayAttributeData *>(&AttributeSet->AbCharismaModifier) },
		{ TEXT("AbConstitutionModifier"), const_cast<FGameplayAttributeData *>(&AttributeSet->AbConstitutionModifier) },
		{ TEXT("AbDexterityModifier"),    const_cast<FGameplayAttributeData *>(&AttributeSet->AbDexterityModifier) },
		{ TEXT("AbIntelligenceModifier"), const_cast<FGameplayAttributeData *>(&AttributeSet->AbIntelligenceModifier) },
		{ TEXT("AbStrengthModifier"),     const_cast<FGameplayAttributeData *>(&AttributeSet->AbStrengthModifier) },
		{ TEXT("AbWisdomModifier"),       const_cast<FGameplayAttributeData *>(&AttributeSet->AbWisdomModifier) },
	};
	return Capture;
}

FAttributeCapture FPF2SpecBase::CaptureSavingThrowModifierAttributes(const UPF2CharacterAttributeSet* AttributeSet)
{
	FAttributeCapture Capture =
	{
		{ TEXT("StFortitudeModifier"), const_cast<FGameplayAttributeData *>(&AttributeSet->StFortitudeModifier) },
		{ TEXT("StReflexModifier"),    const_cast<FGameplayAttributeData *>(&AttributeSet->StReflexModifier) },
		{ TEXT("StWillModifier"),      const_cast<FGameplayAttributeData *>(&AttributeSet->StWillModifier) },
	};

	return Capture;
}

FAttributeCapture FPF2SpecBase::CaptureSkillModifierAttributes(const UPF2CharacterAttributeSet* AttributeSet)
{
	FAttributeCapture Capture =
	{
		{ TEXT("SkAcrobaticsModifier"),   const_cast<FGameplayAttributeData *>(&AttributeSet->SkAcrobaticsModifier) },
		{ TEXT("SkArcanaModifier"),       const_cast<FGameplayAttributeData *>(&AttributeSet->SkArcanaModifier) },
		{ TEXT("SkAthleticsModifier"),    const_cast<FGameplayAttributeData *>(&AttributeSet->SkAthleticsModifier) },
		{ TEXT("SkCraftingModifier"),     const_cast<FGameplayAttributeData *>(&AttributeSet->SkCraftingModifier) },
		{ TEXT("SkDeceptionModifier"),    const_cast<FGameplayAttributeData *>(&AttributeSet->SkDeceptionModifier) },
		{ TEXT("SkDiplomacyModifier"),    const_cast<FGameplayAttributeData *>(&AttributeSet->SkDiplomacyModifier) },
		{ TEXT("SkIntimidationModifier"), const_cast<FGameplayAttributeData *>(&AttributeSet->SkIntimidationModifier) },
		{ TEXT("SkLore1Modifier"),        const_cast<FGameplayAttributeData *>(&AttributeSet->SkLore1Modifier) },
		{ TEXT("SkLore2Modifier"),        const_cast<FGameplayAttributeData *>(&AttributeSet->SkLore2Modifier) },
		{ TEXT("SkMedicineModifier"),     const_cast<FGameplayAttributeData *>(&AttributeSet->SkMedicineModifier) },
		{ TEXT("SkNatureModifier"),       const_cast<FGameplayAttributeData *>(&AttributeSet->SkNatureModifier) },
		{ TEXT("SkOccultismModifier"),    const_cast<FGameplayAttributeData *>(&AttributeSet->SkOccultismModifier) },
		{ TEXT("SkPerformanceModifier"),  const_cast<FGameplayAttributeData *>(&AttributeSet->SkPerformanceModifier) },
		{ TEXT("SkReligionModifier"),     const_cast<FGameplayAttributeData *>(&AttributeSet->SkReligionModifier) },
		{ TEXT("SkSocietyModifier"),      const_cast<FGameplayAttributeData *>(&AttributeSet->SkSocietyModifier) },
		{ TEXT("SkStealthModifier"),      const_cast<FGameplayAttributeData *>(&AttributeSet->SkStealthModifier) },
		{ TEXT("SkSurvivalModifier"),     const_cast<FGameplayAttributeData *>(&AttributeSet->SkSurvivalModifier) },
		{ TEXT("SkThieveryModifier"),     const_cast<FGameplayAttributeData *>(&AttributeSet->SkThieveryModifier) },
	};

	return Capture;
}

FAttributeCapture FPF2SpecBase::CaptureSpellAttributes(const UPF2CharacterAttributeSet* AttributeSet)
{
	FAttributeCapture Capture =
	{
		{ TEXT("SpellAttackRoll"),      const_cast<FGameplayAttributeData *>(&AttributeSet->SpellAttackRoll) },
		{ TEXT("SpellDifficultyClass"), const_cast<FGameplayAttributeData *>(&AttributeSet->SpellDifficultyClass) },
	};

	return Capture;
}

FGameplayAbilitySpecHandle FPF2SpecBase::GrantCharacterFakeAbility(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	const FGameplayTagContainer&                    DynamicAbilityTags)
{
	return GrantCharacterAbility(Character, UPF2TestAbility::StaticClass(), DynamicAbilityTags);
}

FGameplayAbilitySpecHandle FPF2SpecBase::GrantCharacterAbility(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	const TSubclassOf<UGameplayAbility>&            AbilityType,
	const FGameplayTagContainer&                    DynamicAbilityTags)
{
	UAbilitySystemComponent* CharacterAsc    = Character->GetAbilitySystemComponent();
	UGameplayAbility *       AbilityCDO      = AbilityType->GetDefaultObject<UGameplayAbility>(),
	                 *       AbilityInstance;
	FGameplayAbilitySpec     AbilitySpec;

	if (AbilityCDO->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerExecution)
	{
		// Instantiate the GA before granting it, to simulate the logic that would normally happen in
		// UAbilitySystemComponent::InternalTryActivateAbility() so that UGameplayAbility::IsInstantiated() returns
		// "true" and the ability gets assigned a handle when it is granted to the actor. This allows the spec to be
		// retrieved by handle from the ASC during processing (e.g., for dynamic tags).
		AbilityInstance = NewObject<UGameplayAbility>(Character->ToActor(), AbilityType);
	}
	else
	{
		AbilityInstance = AbilityCDO;
	}

	AbilitySpec = FGameplayAbilitySpec(AbilityInstance);
	AbilitySpec.DynamicAbilityTags.AppendTags(DynamicAbilityTags);

	return CharacterAsc->GiveAbility(AbilitySpec);
}

FActiveGameplayEffectHandle FPF2SpecBase::ApplyGameplayEffectToTestCharacter(
	const TSubclassOf<UGameplayEffect>& EffectType) const
{
	FGameplayEffectContext* GameplayEffectContext = new FGameplayEffectContext();

	return this->TestCharacterAsc->BP_ApplyGameplayEffectToSelf(
		EffectType,
		1.0,
		FGameplayEffectContextHandle(GameplayEffectContext)
	);
}

FGameplayEffectSpecHandle FPF2SpecBase::BuildEffectSpec(
	const TSubclassOf<UGameplayEffect>& EffectClass,
	const TMap<FName, float>&           SetByCallerMagnitudesMap) const
{
	FGameplayEffectSpecHandle EffectSpec;

	EffectSpec = this->TestCharacterAsc->MakeOutgoingSpec(EffectClass, 1.0, FGameplayEffectContextHandle());

	for (const auto& [ParameterTag, ParameterValue] : SetByCallerMagnitudesMap)
	{
		EffectSpec = UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			EffectSpec,
			FGameplayTag::RequestGameplayTag(ParameterTag),
			ParameterValue
		);
	}

	return EffectSpec;
}

FGameplayEffectSpecHandle FPF2SpecBase::BuildEffectSpec(
	const TSubclassOf<UGameplayEffect>             EffectClass,
	const TScriptInterface<IPF2CharacterInterface> Instigator,
	const UGameplayAbility*                        InvokingAbility,
	const TMap<FName, float>&                      SetByCallerMagnitudesMap)
{
	return BuildEffectSpec(
		EffectClass,
		Instigator,
		InvokingAbility,
		Instigator->ToActor(),
		SetByCallerMagnitudesMap
	);
}

FGameplayEffectSpecHandle FPF2SpecBase::BuildEffectSpec(
	const TSubclassOf<UGameplayEffect>             EffectClass,
	const TScriptInterface<IPF2CharacterInterface> Instigator,
	const UGameplayAbility*                        InvokingAbility,
	AActor*                                        EffectCauser,
	const TMap<FName, float>&                      SetByCallerMagnitudesMap)
{
	FGameplayEffectSpecHandle EffectSpec;
	AActor*                   InstigatorActor = Instigator->ToActor();

	EffectSpec =
		UPF2AbilitySystemLibrary::MakeGameplayEffectSpecFromAbilityForInstigatorAndCauser(
			InvokingAbility,
			EffectClass,
			InstigatorActor,
			EffectCauser,
			1.0
		);

	for (const auto& [ParameterTag, ParameterValue] : SetByCallerMagnitudesMap)
	{
		EffectSpec = UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			EffectSpec,
			FGameplayTag::RequestGameplayTag(ParameterTag),
			ParameterValue
		);
	}

	return EffectSpec;
}

void FPF2SpecBase::SetupWorld()
{
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);

	this->World = UWorld::CreateWorld(EWorldType::Game, false);

	WorldContext.SetCurrentWorld(this->World);
}

void FPF2SpecBase::BeginPlay() const
{
	const FURL CommandLineURL;

	this->World->InitializeActorsForPlay(CommandLineURL);
	this->World->BeginPlay();
}

void FPF2SpecBase::DestroyWorld() const
{
	GEngine->DestroyWorldContext(this->World);

	this->World->DestroyWorld(false);

	// Workaround for:
	// https://forums.unrealengine.com/t/when-running-505-tests-physx-einvalid-operation-pxscene-unlockread-called-without-matching-call-to-pxscene-lockread/488533
	GEngine->ForceGarbageCollection(true);
}

void FPF2SpecBase::SetupTestPawn()
{
	this->TestPawn    = this->SpawnPawn();
	this->TestPawnAsc = this->TestPawn->GetAbilitySystemComponent();
}

void FPF2SpecBase::DestroyTestPawn()
{
	if (this->TestPawn != nullptr)
	{
		this->World->EditorDestroyActor(this->TestPawn, false);
	}

	this->TestPawn = nullptr;
}

void FPF2SpecBase::SetupTestCharacter()
{
	this->TestCharacter    = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
	this->TestCharacterAsc = this->TestCharacter->GetAbilitySystemComponent();
}

void FPF2SpecBase::DestroyTestCharacter()
{
	if (this->TestCharacter != nullptr)
	{
		this->World->EditorDestroyActor(this->TestCharacter->ToActor(), false);
	}

	this->TestCharacter = TScriptInterface<IPF2CharacterInterface>(nullptr);
}

APF2TestPawn* FPF2SpecBase::SpawnPawn() const
{
	return this->World->SpawnActor<APF2TestPawn>();
}

IPF2CharacterInterface* FPF2SpecBase::SpawnCharacter() const
{
	return this->World->SpawnActor<APF2TestCharacter>();
}

FActiveGameplayEffectHandle FPF2SpecBase::InitializeAttributeAndApplyEffect(
	FGameplayAttributeData&             Attribute,
	const float                         StartingValue,
	const TSubclassOf<UGameplayEffect>& EffectBP,
	const float                         CharacterLevel) const
{
	UGameplayEffect* GameplayEffect = EffectBP->GetDefaultObject<UGameplayEffect>();

	Attribute = StartingValue;

	const FActiveGameplayEffectHandle EffectHandle = this->TestPawnAsc->ApplyGameplayEffectToTarget(
		GameplayEffect,
		this->TestPawnAsc,
		CharacterLevel
	);

	return EffectHandle;
}

void FPF2SpecBase::ApplyUnreplicatedTag(const FString& TagName) const
{
	this->TestPawnAsc->AddLooseGameplayTag(PF2GameplayAbilityUtilities::GetTag(TagName));
}

void FPF2SpecBase::RemoveUnreplicatedTag(const FString& TagName) const
{
	this->TestPawnAsc->RemoveLooseGameplayTag(PF2GameplayAbilityUtilities::GetTag(TagName));
}

void FPF2SpecBase::TestCharacterHasCondition(const TScriptInterface<IPF2CharacterInterface>& Character,
                                             const FGameplayTag&                             ConditionTag)
{
	this->TestAscHasCondition(
		Character->GetIdForLogs(),
		Character->GetAbilitySystemComponent(),
		ConditionTag
	);
}

void FPF2SpecBase::TestAscHasCondition(const FString&                 WhatAsc,
                                       const UAbilitySystemComponent* AbilitySystemComponent,
                                       const FGameplayTag&            ConditionTag)
{
	TestTrue(
		FString::Format(TEXT("'{0}' has tag '{1}'"), {*WhatAsc, *ConditionTag.ToString()}),
		AbilitySystemComponent->HasMatchingGameplayTag(ConditionTag)
	);
}

void FPF2SpecBase::TestCharacterNotHaveCondition(const TScriptInterface<IPF2CharacterInterface>& Character,
                                                 const FGameplayTag&                             ConditionTag)
{
	this->TestAscNotHaveCondition(
		Character->GetIdForLogs(),
		Character->GetAbilitySystemComponent(),
		ConditionTag
	);
}

void FPF2SpecBase::TestAscNotHaveCondition(const FString&                 WhatAsc,
                                           const UAbilitySystemComponent* AbilitySystemComponent,
                                           const FGameplayTag&            ConditionTag)
{
	TestFalse(
		FString::Format(TEXT("'{0}' has tag '{1}'"), {*WhatAsc, *ConditionTag.ToString()}),
		AbilitySystemComponent->HasMatchingGameplayTag(ConditionTag)
	);
}

void FPF2SpecBase::TestCharacterHasConditionLevel(const TScriptInterface<IPF2CharacterInterface>& Character,
                                                  const FGameplayTag&                             ParentTag,
                                                  const uint8                                     ExpectedLevel)
{
	this->TestAscHasConditionLevel(
		Character->GetIdForLogs(),
		Character->GetAbilitySystemComponent(),
		ParentTag,
		ExpectedLevel
	);
}

void FPF2SpecBase::TestAscHasConditionLevel(const FString&                 WhatAsc,
                                            const UAbilitySystemComponent* AbilitySystemComponent,
                                            const FGameplayTag&            ParentTag,
                                            const uint8                    ExpectedLevel)
{
	FGameplayTagContainer CharacterTags;
	uint8                 ActualLevel;

	AbilitySystemComponent->GetOwnedGameplayTags(CharacterTags);

	ActualLevel = UPF2TagLibrary::FindAndParseConditionLevel(CharacterTags, ParentTag);

	this->TestEqual(
		FString::Format(
			TEXT("Expected '{0}' to have condition level {1} of tag '{2}', but it was level {3}."),
			{
				*WhatAsc,
				*FString::FromInt(ExpectedLevel),
				*ParentTag.ToString(),
				*FString::FromInt(ActualLevel)
			}
		),
		ActualLevel,
		ExpectedLevel
	);
}
