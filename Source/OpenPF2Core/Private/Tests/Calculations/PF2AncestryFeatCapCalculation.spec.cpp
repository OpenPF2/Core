// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//   - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//   - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//   - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
//
// Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
// as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
// file other than the material designated as Open Game Content may be reproduced in any form without written
// permission.

#include "Abilities/PF2CharacterAttributeSet.h"
#include "Calculations/Modifiers/PF2AbilityModifierCalculationBase.h"
#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2AncestryFeatCapCalculationSpec,
                     "OpenPF2.AncestryFeatCapCalculations",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	const FString BlueprintPath                 = TEXT("/OpenPF2Core/OpenPF2/Core/CharacterStats/Ancestries");
	const FString AncestryFeatCapGameEffectCalc = TEXT("GE_CalcAncestryFeatLimit");

	const FString AncestryFeatCapAttributeName = TEXT("FeAncestryFeatLimit");

	// From the Pathfinder 2E Core Rulebook, pages 36, 40, 44, 48, 52, and 56:
	// "At 1st level, you gain one ancestry feat, and you gain an additional ancestry feat every 4 levels thereafter (at
	// 5th, 9th, 13th, and 17th level)."
	const TMap<float, float> ExpectedFeatLimitsByLevel = {
		{ 1.0f, 1.0f},
		{ 2.0f, 1.0f},
		{ 3.0f, 1.0f},
		{ 4.0f, 1.0f},
		{ 5.0f, 2.0f},
		{ 6.0f, 2.0f},
		{ 7.0f, 2.0f},
		{ 8.0f, 2.0f},
		{ 9.0f, 3.0f},
		{10.0f, 3.0f},
		{11.0f, 3.0f},
		{12.0f, 3.0f},
		{13.0f, 4.0f},
		{14.0f, 4.0f},
		{15.0f, 4.0f},
		{16.0f, 4.0f},
		{17.0f, 5.0f},
		{18.0f, 5.0f},
		{19.0f, 5.0f},
		{20.0f, 5.0f},
	};

	TSubclassOf<UGameplayEffect> LoadGE() const;
END_DEFINE_PF_SPEC(FPF2AncestryFeatCapCalculationSpec)

void FPF2AncestryFeatCapCalculationSpec::Define()
{
	BeforeEach([=, this]()
	{
		this->SetupWorld();
		this->SetupTestPawn();

		this->BeginPlay();
	});

	AfterEach([=, this]()
	{
		this->DestroyTestPawn();
		this->DestroyWorld();
	});

	for (auto& LevelFeatLimit : ExpectedFeatLimitsByLevel)
	{
		const float CharacterLevel    = LevelFeatLimit.Key,
		            ExpectedFeatLimit = LevelFeatLimit.Value;

		Describe(FString::Format(TEXT("when character Level is '{0}'"), {FString::FormatAsNumber(CharacterLevel)}), [=, this]()
		{
			It(FString::Format(TEXT("returns an Ancestry Feat Cap of '{0}'"), {FString::FormatAsNumber(ExpectedFeatLimit)}), [=, this]()
			{
				const UPF2CharacterAttributeSet*    AttributeSet = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
				FAttributeCapture                   Attributes   = CaptureAttributes(AttributeSet);
				FGameplayAttributeData*             CapAttribute = Attributes[this->AncestryFeatCapAttributeName];
				const TSubclassOf<UGameplayEffect>& EffectBP     = this->LoadGE();

				this->InitializeAttributeAndApplyEffect(*CapAttribute, 0.0f, EffectBP, CharacterLevel);

				TestEqual(
					FString::Format(TEXT("{0}.BaseValue"), {this->AncestryFeatCapAttributeName}),
					CapAttribute->GetBaseValue(),
					ExpectedFeatLimit
				);

				TestEqual(
					FString::Format(TEXT("{0}.CurrentValue"), {this->AncestryFeatCapAttributeName}),
					CapAttribute->GetCurrentValue(),
					ExpectedFeatLimit
				);
			});
		});
	}
}

TSubclassOf<UGameplayEffect> FPF2AncestryFeatCapCalculationSpec::LoadGE() const
{
	return LoadBlueprint<UGameplayEffect>(this->BlueprintPath, this->AncestryFeatCapGameEffectCalc);
}
