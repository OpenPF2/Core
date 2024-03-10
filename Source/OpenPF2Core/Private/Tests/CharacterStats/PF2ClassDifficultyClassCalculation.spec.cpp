// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "CharacterStats/PF2CharacterAttributeSet.h"

#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2ClassDifficultyClassCalculationSpec,
                     "OpenPF2.ClassDifficultyClassCalculations",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	const FString BlueprintPath         = TEXT("/OpenPF2Core/OpenPF2/Core/CharacterStats");
	const FString ClassDcGameEffectCalc = TEXT("GE_CalcClassDifficultyClass");

	const FString CdcModAttributeName = TEXT("ClassDifficultyClass");

	const TMap<FString, float> ProficiencyValues =
	{
		{"Untrained", 0.0f},
		{"Trained",   3.0f},
		{"Expert",    5.0f},
		{"Master",    7.0f},
		{"Legendary", 9.0f},
	};

	const TMap<FString, FString> AbilityModifierAttributes = {
		{"Charisma",     "AbCharismaModifier"     },
		{"Constitution", "AbConstitutionModifier" },
		{"Dexterity",    "AbDexterityModifier"    },
		{"Intelligence", "AbIntelligenceModifier" },
		{"Strength",     "AbStrengthModifier"     },
		{"Wisdom",       "AbWisdomModifier"       },
	};

	TSubclassOf<UGameplayEffect> LoadGE() const;
END_DEFINE_PF_SPEC(FPF2ClassDifficultyClassCalculationSpec)

void FPF2ClassDifficultyClassCalculationSpec::Define()
{
	BeforeEach([=, this]()
	{
		this->SetupWorld();
		this->SetupTestPawn();

		this->BeginPlay();
	});

	BeforeEach([=, this]()
	{
		const UPF2CharacterAttributeSet* AttributeSet = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
		FAttributeCapture                Attributes   = CaptureAbilityModifierAttributes(AttributeSet);

		// Zero out all attributes so that only the "boosted" attribute has an effect.
		for (auto& Modifier : this->AbilityModifierAttributes)
		{
			const FString ModifierName = Modifier.Value;

			*(Attributes[ModifierName]) = 0.0f;
		}
	});

	AfterEach([=, this]()
	{
		this->DestroyTestPawn();
		this->DestroyWorld();
	});

	for (auto& KeyAbility : this->AbilityModifierAttributes)
	{
		const FString KeyAbilityName      = KeyAbility.Key,
		              KeyAbilityAttribute = KeyAbility.Value;

		Describe(FString::Format(TEXT("when the character's Key Ability is '{0}'"), {KeyAbilityName}), [=, this]
		{
			BeforeEach([=, this]()
			{
				this->ApplyUnreplicatedTag(FString::Format(TEXT("KeyAbility.{0}"), {KeyAbilityName}));
			});

			for (auto& BoostedAbility : this->AbilityModifierAttributes)
			{
				const FString BoostedAbilityName      = BoostedAbility.Key,
				              BoostedAbilityAttribute = BoostedAbility.Value;

				Describe(FString::Format(TEXT("when the '{0}' Ability Modifier is '5'"), {BoostedAbilityName}), [=, this]
				{
					BeforeEach([=, this]()
					{
						const UPF2CharacterAttributeSet* AttributeSet = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
						FAttributeCapture                Attributes   = CaptureAbilityModifierAttributes(AttributeSet);

						*(Attributes[BoostedAbilityAttribute]) = 5.0f;
					});

					for (auto& Proficiency : this->ProficiencyValues)
					{
						const FString ProficiencyLevel = Proficiency.Key;
						const float   ProficiencyValue = Proficiency.Value;

						Describe(FString::Format(TEXT("when the character is '{0}' in Class DC"), {ProficiencyLevel}), [=, this]()
						{
							float ExpectedClassDcMod;

							BeforeEach([=, this]()
							{
								this->ApplyUnreplicatedTag(FString::Format(TEXT("ClassDc.{0}"), {ProficiencyLevel}));
							});

							if (KeyAbilityName == BoostedAbilityName)
							{
								ExpectedClassDcMod = 10.0f + 5.0f + ProficiencyValue;
							}
							else
							{
								ExpectedClassDcMod = 10.0f + 0.0f + ProficiencyValue;
							}

							It(FString::Format(TEXT("calculates a Class DC Modifier of '{0}'"), {FString::FormatAsNumber(ExpectedClassDcMod)}), [=, this]()
							{
								const UPF2CharacterAttributeSet*    AttributeSet     = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
								FAttributeCapture                   Attributes       = CaptureAttributes(AttributeSet);
								FGameplayAttributeData*             ClassDcAttribute = Attributes[this->CdcModAttributeName];
								const TSubclassOf<UGameplayEffect>& EffectBP         = this->LoadGE();

								this->InitializeAttributeAndApplyEffect(*ClassDcAttribute, 0.0f, EffectBP);

								TestEqual(
									FString::Format(TEXT("{0}.BaseValue"), {this->CdcModAttributeName}),
									ClassDcAttribute->GetBaseValue(),
									0.0f
								);

								TestEqual(
									FString::Format(TEXT("{0}.CurrentValue"), {this->CdcModAttributeName}),
									ClassDcAttribute->GetCurrentValue(),
									ExpectedClassDcMod
								);
							});
						});
					}
				});
			}
		});
	}
}

TSubclassOf<UGameplayEffect> FPF2ClassDifficultyClassCalculationSpec::LoadGE() const
{
	return LoadBlueprint<UGameplayEffect>(this->BlueprintPath, this->ClassDcGameEffectCalc);
}
