// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Tests/PF2SpecBase.h"

#include <AbilitySystemBlueprintLibrary.h>

#include <Engine/Engine.h>

#include "Abilities/PF2CharacterAttributeSet.h"

#include "Libraries/PF2AbilitySystemLibrary.h"

#include "Tests/PF2TestAbility.h"
#include "Tests/PF2TestCharacter.h"

#include "Utilities/PF2GameplayAbilityUtilities.h"
#include "Utilities/PF2InterfaceUtilities.h"

bool FPF2SpecBase::RunTest(const FString& InParameters)
{
	FAutomationTestFramework& AutomationTestFramework = FAutomationTestFramework::GetInstance();

	this->EnsureDefinitionsEx();

	if (InParameters.IsEmpty())
	{
		TArray<TSharedRef<FSpec>> Specs;
		IdToSpecMap.GenerateValueArray(Specs);

		// Run all tests.
		for (int32 SpecIndex = 0; SpecIndex < Specs.Num(); SpecIndex++)
		{
			for (int32 CommandIndex = 0; CommandIndex < Specs[SpecIndex]->Commands.Num(); ++CommandIndex)
			{
				AutomationTestFramework.EnqueueLatentCommand(Specs[SpecIndex]->Commands[CommandIndex]);
			}
		}
	}
	else
	{
		const TSharedRef<FSpec>* SpecToRun = IdToSpecMap.Find(InParameters);

		// Run specific tests.
		if (SpecToRun != nullptr)
		{
			for (int32 Index = 0; Index < (*SpecToRun)->Commands.Num(); ++Index)
			{
				AutomationTestFramework.EnqueueLatentCommand((*SpecToRun)->Commands[Index]);
			}
		}
	}

	return true;
}

void FPF2SpecBase::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	this->EnsureDefinitionsEx();

	FAutomationSpecBase::GetTests(OutBeautifiedNames, OutTestCommands);
}

void FPF2SpecBase::Describe(const FString& InDescription, const uint8 SpecVersion, const TFunction<void()>& DoWork)
{
	LLM_SCOPE_BYNAME(TEXT("AutomationTest/Framework"));
	const TSharedRef<FSpecDefinitionScope> ParentScope = DefinitionScopeStack.Last();
	// --- Start OpenPF2 Differences from <Misc/AutomationTest.h>
	const TSharedRef<FSpecDefinitionScope> NewScope =
		MakeShareable(
			(SpecVersion == 2) ? new FSpecDefinitionScopeEx() : new FSpecDefinitionScope()
		);
	// --- End OpenPF2 Differences from <Misc/AutomationTest.h>
	NewScope->Description = InDescription;
	ParentScope->Children.Push(NewScope);

	DefinitionScopeStack.Push(NewScope);
	PushDescription(InDescription);
	DoWork();
	PopDescription(InDescription);
	DefinitionScopeStack.Pop();

	if (NewScope->It.Num() == 0 && NewScope->Children.Num() == 0)
	{
		ParentScope->Children.Remove(NewScope);
	}
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
	const TScriptInterface<IPF2CharacterInterface> Character)
{
	return GrantCharacterAbility(Character, TSubclassOf<UPF2TestAbility>(UPF2TestAbility::StaticClass()));
}

FGameplayAbilitySpecHandle FPF2SpecBase::GrantCharacterAbility(
	const TScriptInterface<IPF2CharacterInterface> Character,
	const TSubclassOf<UGameplayAbility>            AbilityClass)
{
	UAbilitySystemComponent*   CharacterAsc = Character->GetAbilitySystemComponent();
	const FGameplayAbilitySpec AbilitySpec  = FGameplayAbilitySpec(AbilityClass);

	return CharacterAsc->GiveAbility(AbilitySpec);
}

FGameplayEffectSpecHandle FPF2SpecBase::BuildEffectSpec(
	const TSubclassOf<UGameplayEffect> EffectClass,
	TMap<FName, float>                 SetByCallerMagnitudesMap) const
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
	const FGameplayAbilitySpecHandle               InvokingAbilityHandle,
	const TMap<FName, float>&                      SetByCallerMagnitudesMap)
{
	return BuildEffectSpec(
		EffectClass,
		Instigator,
		InvokingAbilityHandle,
		Instigator->ToActor(),
		SetByCallerMagnitudesMap
	);
}

FGameplayEffectSpecHandle FPF2SpecBase::BuildEffectSpec(
	const TSubclassOf<UGameplayEffect>             EffectClass,
	const TScriptInterface<IPF2CharacterInterface> Instigator,
	const FGameplayAbilitySpecHandle               InvokingAbilityHandle,
	AActor*                                        EffectCauser,
	const TMap<FName, float>&                      SetByCallerMagnitudesMap)
{
	FGameplayEffectSpecHandle      EffectSpec;
	AActor*                        InstigatorActor = Instigator->ToActor();
	const UAbilitySystemComponent* InstigatorAsc   = Instigator->GetAbilitySystemComponent();

	EffectSpec =
		UPF2AbilitySystemLibrary::MakeGameplayEffectSpecForInstigatorAndCauser(
			InvokingAbilityHandle,
			*InstigatorAsc->AbilityActorInfo.Get(),
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

void FPF2SpecBase::EnsureDefinitionsEx() const
{
	if (!bHasBeenDefined)
	{
		const_cast<FPF2SpecBase*>(this)->Define();
		const_cast<FPF2SpecBase*>(this)->PostDefineEx();
	}
}

void FPF2SpecBase::PostDefineEx()
{
	TArray<TSharedRef<FSpecDefinitionScope>> Stack;
	Stack.Push(RootDefinitionScope.ToSharedRef());

	TArray<TSharedRef<IAutomationLatentCommand>> BeforeAll,
	                                             BeforeEach,
	                                             AfterEach,
	                                             AfterAll;

	while (Stack.Num() > 0)
	{
		const TSharedRef<FSpecDefinitionScope> Scope = Stack.Last();

		if (Scope->SpecDefinitionVersion == 2)
		{
			const TSharedRef<FSpecDefinitionScopeEx> ScopeV2 = this->GetCurrentV2Scope();

			BeforeAll.Append(ScopeV2->BeforeAll);
			BeforeAll.Append(ScopeV2->AfterAll);
		}

		BeforeEach.Append(Scope->BeforeEach);
		AfterEach.Append(Scope->AfterEach);

		for (int32 ItIndex = 0; ItIndex < Scope->It.Num(); ItIndex++)
		{
			const TSharedRef<FSpecIt> It   = Scope->It[ItIndex];
			TSharedRef<FSpec>         Spec = MakeShareable(new FSpec());

			Spec->Id               = It->Id;
			Spec->Description      = It->Description;
			Spec->Filename         = It->Filename;
			Spec->LineNumber       = It->LineNumber;

			Spec->Commands.Append(BeforeAll);
			Spec->Commands.Append(BeforeEach);
			Spec->Commands.Add(It->Command);

			// Iterate in reverse to evaluate AfterEach() from the inner-most scope outwards.
			for (int32 AfterEachIndex = AfterEach.Num() - 1; AfterEachIndex >= 0; AfterEachIndex--)
			{
				Spec->Commands.Add(AfterEach[AfterEachIndex]);
			}

			for (int32 AfterAllIndex = AfterAll.Num() - 1; AfterAllIndex >= 0; AfterAllIndex--)
			{
				Spec->Commands.Add(AfterAll[AfterAllIndex]);
			}

			check(!IdToSpecMap.Contains(Spec->Id));
			IdToSpecMap.Add(Spec->Id, Spec);
		}

		Scope->It.Empty();

		if (Scope->Children.Num() > 0)
		{
			Stack.Append(Scope->Children);
			Scope->Children.Empty();
		}
		else
		{
			while (Stack.Num() > 0 && Stack.Last()->Children.Num() == 0 && Stack.Last()->It.Num() == 0)
			{
				const TSharedRef<FSpecDefinitionScope> PoppedScope        = Stack.Pop();
				const int32                            NumBeforeEachAdded = PoppedScope->BeforeEach.Num(),
				                                       NumAfterEachAdded  = PoppedScope->AfterEach.Num();

				if (NumBeforeEachAdded > 0)
				{
					BeforeEach.RemoveAt(BeforeEach.Num() - NumBeforeEachAdded, NumBeforeEachAdded);
				}

				if (NumAfterEachAdded > 0)
				{
					AfterEach.RemoveAt(AfterEach.Num() - NumAfterEachAdded, NumAfterEachAdded);
				}

				if (PoppedScope->SpecDefinitionVersion == 2)
				{
					const TSharedRef<FSpecDefinitionScopeEx> PoppedScopeV2 =
						StaticCastSharedRef<FSpecDefinitionScopeEx>(PoppedScope);

					const int32 NumBeforeAllAdded = PoppedScopeV2->BeforeAll.Num(),
					            NumAfterAllAdded  = PoppedScopeV2->AfterAll.Num();

					if (NumBeforeAllAdded > 0)
					{
						BeforeAll.RemoveAt(BeforeAll.Num() - NumBeforeAllAdded, NumBeforeAllAdded);
					}

					if (NumAfterAllAdded > 0)
					{
						AfterAll.RemoveAt(AfterAll.Num() - NumAfterAllAdded, NumAfterAllAdded);
					}
				}
			}
		}
	}

	RootDefinitionScope.Reset();
	DefinitionScopeStack.Reset();
	bHasBeenDefined = true;
}
