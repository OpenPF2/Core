// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2CharacterAttributeSet.h"
#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2SavingThrowModifierCalculationSpec,
                     "OpenPF2.SavingThrowModifierCalculations",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	const FString BlueprintPath             = TEXT("/OpenPF2Core/OpenPF2/Core/CharacterStats");
	const FString SavingThrowGameEffectCalc = TEXT("GE_CalcSavingThrowModifiers");

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

	void VerifySavingThrowModifier(const FString& AbModFriendlyName,
	                               const FString& AbModAttributeName,
	                               const FString& SavingModAttributeFriendlyName,
	                               const FString& SavingModAttributeName);

	void VerifySavingThrowModifier(const FString& AbModAttributeName,
	                               const float    AbModScore,
	                               const FString& SavingModAttributeFriendlyName,
	                               const FString& SavingModAttributeName,
	                               const FString& ProficiencyLevel,
	                               const float    ExpectedSavingMod);
END_DEFINE_PF_SPEC(FPF2SavingThrowModifierCalculationSpec)

void FPF2SavingThrowModifierCalculationSpec::Define()
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

	Describe("Fortitude Saving Throws", [=, this]
	{
		const FString AbModFriendlyName              = TEXT("Constitution");
		const FString AbModAttributeName             = TEXT("AbConstitutionModifier");
		const FString SavingModAttributeFriendlyName = TEXT("Fortitude");
		const FString SavingModAttributeName         = TEXT("StFortitudeModifier");

		this->VerifySavingThrowModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SavingModAttributeFriendlyName,
			SavingModAttributeName
		);
	});

	Describe("Reflex Saving Throws", [=, this]
	{
		const FString AbModFriendlyName              = TEXT("Dexterity");
		const FString AbModAttributeName             = TEXT("AbDexterityModifier");
		const FString SavingModAttributeFriendlyName = TEXT("Reflex");
		const FString SavingModAttributeName         = TEXT("StReflexModifier");

		this->VerifySavingThrowModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SavingModAttributeFriendlyName,
			SavingModAttributeName
		);
	});

	Describe("Will Saving Throws", [=, this]
	{
		const FString AbModFriendlyName              = TEXT("Wisdom");
		const FString AbModAttributeName             = TEXT("AbWisdomModifier");
		const FString SavingModAttributeFriendlyName = TEXT("Will");
		const FString SavingModAttributeName         = TEXT("StWillModifier");

		this->VerifySavingThrowModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SavingModAttributeFriendlyName,
			SavingModAttributeName
		);
	});
}

TSubclassOf<UGameplayEffect> FPF2SavingThrowModifierCalculationSpec::LoadGE() const
{
	return LoadBlueprint<UGameplayEffect>(this->BlueprintPath, this->SavingThrowGameEffectCalc);
}

void FPF2SavingThrowModifierCalculationSpec::VerifySavingThrowModifier(const FString& AbModFriendlyName,
                                                                       const FString& AbModAttributeName,
                                                                       const FString& SavingModAttributeFriendlyName,
                                                                       const FString& SavingModAttributeName)
{
	for (const auto AbilityValues : this->ExpectedValues)
	{
		const float                AbModScore     = AbilityValues.Key;
		const TMap<FString, float> TrainingScores = AbilityValues.Value;

		Describe(FString::Format(TEXT("when the character has a '{0}' modifier of '{1}'"), {AbModFriendlyName, FString::FormatAsNumber(AbModScore)}), [=, this]()
		{
			for (const auto ProficiencyValues : TrainingScores)
			{
				const FString ProficiencyLevel  = ProficiencyValues.Key;
				const float   ExpectedSavingMod = ProficiencyValues.Value;

				Describe(FString::Format(TEXT("when the character is '{0}' in '{1}' saving throws"), {ProficiencyLevel, SavingModAttributeFriendlyName}), [=, this]()
				{
					It(FString::Format(TEXT("calculates a '{0}' saving throw of '{1}'"), {SavingModAttributeFriendlyName, FString::FormatAsNumber(ExpectedSavingMod)}), [=, this]()
					{
						this->VerifySavingThrowModifier(
							AbModAttributeName,
							AbModScore,
							SavingModAttributeFriendlyName,
							SavingModAttributeName,
							ProficiencyLevel,
							ExpectedSavingMod
						);
					});
				});
			}
		});
	}
}

void FPF2SavingThrowModifierCalculationSpec::VerifySavingThrowModifier(const FString& AbModAttributeName,
                                                                       const float    AbModScore,
                                                                       const FString& SavingModAttributeFriendlyName,
                                                                       const FString& SavingModAttributeName,
                                                                       const FString& ProficiencyLevel,
                                                                       const float    ExpectedSavingMod)
{
	const UPF2CharacterAttributeSet*    AttributeSet    = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
	FAttributeCapture                   Attributes      = CaptureAttributes(AttributeSet);
	FGameplayAttributeData*             AbModAttribute  = Attributes[AbModAttributeName];
	const FGameplayAttributeData*       SavingAttribute = Attributes[SavingModAttributeName];
	const TSubclassOf<UGameplayEffect>& EffectBP        = this->LoadGE();

	this->ApplyUnreplicatedTag(FString::Format(TEXT("SavingThrow.{0}.{1}"), {SavingModAttributeFriendlyName, ProficiencyLevel}));
	this->InitializeAttributeAndApplyEffect(*AbModAttribute, AbModScore, EffectBP);

	TestEqual(
		FString::Format(TEXT("{0}.BaseValue"), {SavingModAttributeName}),
		SavingAttribute->GetBaseValue(),
		0.0f
	);

	TestEqual(
		FString::Format(TEXT("{0}.CurrentValue"), {SavingModAttributeName}),
		SavingAttribute->GetCurrentValue(),
		ExpectedSavingMod
	);
}
