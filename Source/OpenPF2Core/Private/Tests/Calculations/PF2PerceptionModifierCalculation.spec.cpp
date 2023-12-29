// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2CharacterAttributeSet.h"
#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2PerceptionModifierCalculationSpec,
                     "OpenPF2.PerceptionModifierCalculations",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	const FString BlueprintPath            = TEXT("/OpenPF2Core/OpenPF2/Core/Calculations");
	const FString PerceptionGameEffectCalc = TEXT("GE_CalcPerceptionModifier");

	const FString AbModFriendlyName  = TEXT("Wisdom");
	const FString AbModAttributeName = TEXT("AbWisdomModifier");

	const FString PcpModAttributeName = TEXT("PerceptionModifier");

	const TMap<float, TMap<FString, float>> ExpectedValues =
	{
		{
			-5.0f,
			{
				{"Untrained", -5.0f},
				{"Trained",   -2.0f},
				{"Expert",     0.0f},
				{"Master",     2.0f},
				{"Legendary",  4.0f},
			},
		},
		{
			0.0f,
			{
				{"Untrained",  0.0f},
				{"Trained",    3.0f},
				{"Expert",     5.0f},
				{"Master",     7.0f},
				{"Legendary",  9.0f},
			},
		},
		{
			3.0f,
			{
				{"Untrained",  3.0f},
				{"Trained",    6.0f},
				{"Expert",     8.0f},
				{"Master",    10.0f},
				{"Legendary", 12.0f},
			},
		},
	};

	TSubclassOf<UGameplayEffect> LoadGE() const;

	void VerifyPerceptionModifier(const float    AbModScore,
	                              const FString& ProficiencyLevel,
	                              const float    ExpectedPcpMod);
END_DEFINE_PF_SPEC(FPF2PerceptionModifierCalculationSpec)

void FPF2PerceptionModifierCalculationSpec::Define()
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

	for (const auto AbilityValues : this->ExpectedValues)
	{
		const float                AbModScore     = AbilityValues.Key;
		const TMap<FString, float> TrainingScores = AbilityValues.Value;

		Describe(FString::Format(TEXT("when the character has a Perception Modifier of '{0}'"), {FString::FormatAsNumber(AbModScore)}), [=, this]()
		{
			for (const auto ProficiencyValues : TrainingScores)
			{
				const FString ProficiencyLevel = ProficiencyValues.Key;
				const float   ExpectedPcpMod   = ProficiencyValues.Value;

				Describe(FString::Format(TEXT("when the character is '{0}' in Perception"), {ProficiencyLevel}), [=, this]()
				{
					It(FString::Format(TEXT("calculates a Perception Modifier of '{0}'"), {FString::FormatAsNumber(ExpectedPcpMod)}), [=, this]()
					{
						this->VerifyPerceptionModifier(AbModScore, ProficiencyLevel, ExpectedPcpMod);
					});
				});
			}
		});
	}
}

TSubclassOf<UGameplayEffect> FPF2PerceptionModifierCalculationSpec::LoadGE() const
{
	return LoadBlueprint<UGameplayEffect>(this->BlueprintPath, this->PerceptionGameEffectCalc);
}

void FPF2PerceptionModifierCalculationSpec::VerifyPerceptionModifier(const float    AbModScore,
                                                                     const FString& ProficiencyLevel,
                                                                     const float    ExpectedPcpMod)
{
	const UPF2CharacterAttributeSet*    AttributeSet   = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
	FAttributeCapture                   Attributes     = CaptureAttributes(AttributeSet);
	FGameplayAttributeData*             AbModAttribute = Attributes[this->AbModAttributeName];
	const FGameplayAttributeData*       PcpAttribute   = Attributes[this->PcpModAttributeName];
	const TSubclassOf<UGameplayEffect>& EffectBP       = this->LoadGE();

	this->ApplyUnreplicatedTag(FString::Format(TEXT("Perception.{0}"), {ProficiencyLevel}));
	this->InitializeAttributeAndApplyEffect(*AbModAttribute, AbModScore, EffectBP);

	TestEqual(
		FString::Format(TEXT("{0}.BaseValue"), {this->PcpModAttributeName}),
		PcpAttribute->GetBaseValue(),
		0.0f
	);

	TestEqual(
		FString::Format(TEXT("{0}.CurrentValue"), {this->PcpModAttributeName}),
		PcpAttribute->GetCurrentValue(),
		ExpectedPcpMod
	);
}
