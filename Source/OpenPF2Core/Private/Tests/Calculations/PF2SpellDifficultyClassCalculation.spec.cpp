// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2CharacterAttributeSet.h"
#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2SpellDifficultyClassCalculationsSpec,
                     "OpenPF2.SpellDifficultyClassCalculations",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	const FString BlueprintPath                      = TEXT("/OpenPF2Core/OpenPF2/Core/CharacterStats");
	const FString SpellDifficultyClassGameEffectCalc = TEXT("GE_CalcSpellDifficultyClass");

	const FString SpellDifficultyClassAttributeName = TEXT("SpellDifficultyClass");

	const TMap<FString, float> ProficiencyValues =
	{
		{"Untrained", 0.0f},
		{"Trained",   3.0f},
		{"Expert",    5.0f},
		{"Master",    7.0f},
		{"Legendary", 9.0f},
	};

	// The classes in the core rulebook don't have any spell-casting abilities other than Wisdom and Charisma, but
	// there's no reason to limit game makers to just those options. Plus, add-on material for OpenPF2 could always
	// invent a new class that uses something like Intelligence for a character that does complex spell casting or
	// Dexterity for a character who performs physically-dexterous spells.
	const TMap<FString, FString> AllAbilityModifierAttributes = {
		{"Charisma",     "AbCharismaModifier"     },
		{"Constitution", "AbConstitutionModifier" },
		{"Dexterity",    "AbDexterityModifier"    },
		{"Intelligence", "AbIntelligenceModifier" },
		{"Strength",     "AbStrengthModifier"     },
		{"Wisdom",       "AbWisdomModifier"       },
	};

	TSubclassOf<UGameplayEffect> LoadGE() const;
END_DEFINE_PF_SPEC(FPF2SpellDifficultyClassCalculationsSpec)

void FPF2SpellDifficultyClassCalculationsSpec::Define()
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
		for (auto& Modifier : this->AllAbilityModifierAttributes)
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

	for (auto& SpellAbility : this->AllAbilityModifierAttributes)
	{
		const FString SpellAbilityName      = SpellAbility.Key,
		              SpellAbilityAttribute = SpellAbility.Value;

		Describe(FString::Format(TEXT("when the character's Spellcasting Ability is '{0}'"), {SpellAbilityName}), [=, this]
		{
			BeforeEach([=, this]()
			{
				this->ApplyUnreplicatedTag(FString::Format(TEXT("SpellcastingAbility.{0}"), {SpellAbilityName}));
			});

			for (auto& BoostedAbility : this->AllAbilityModifierAttributes)
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

						Describe(FString::Format(TEXT("when the character is '{0}' in Spell Difficulty Class"), {ProficiencyLevel}), [=, this]()
						{
							float ExpectedSpellDifficultyClassMod;

							BeforeEach([=, this]()
							{
								this->ApplyUnreplicatedTag(FString::Format(TEXT("SpellDc.{0}"), {ProficiencyLevel}));
							});

							if (SpellAbilityName == BoostedAbilityName)
							{
								ExpectedSpellDifficultyClassMod = 10.0f + 5.0f + ProficiencyValue;
							}
							else
							{
								ExpectedSpellDifficultyClassMod = 10.0f + 0.0f + ProficiencyValue;
							}

							It(FString::Format(TEXT("calculates a Spell Difficulty Class Modifier of '{0}'"), {FString::FormatAsNumber(ExpectedSpellDifficultyClassMod)}), [=, this]()
							{
								const UPF2CharacterAttributeSet*    AttributeSet                  = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
								FAttributeCapture                   Attributes                    = CaptureSpellAttributes(AttributeSet);
								FGameplayAttributeData*             SpellDifficultyClassAttribute = Attributes[this->SpellDifficultyClassAttributeName];
								const TSubclassOf<UGameplayEffect>& EffectBP                      = this->LoadGE();

								this->InitializeAttributeAndApplyEffect(*SpellDifficultyClassAttribute, 0.0f, EffectBP);

								TestEqual(
									FString::Format(TEXT("{0}.BaseValue"), {this->SpellDifficultyClassAttributeName}),
									SpellDifficultyClassAttribute->GetBaseValue(),
									0.0f
								);

								TestEqual(
									FString::Format(TEXT("{0}.CurrentValue"), {this->SpellDifficultyClassAttributeName}),
									SpellDifficultyClassAttribute->GetCurrentValue(),
									ExpectedSpellDifficultyClassMod
								);
							});
						});
					}
				});
			}
		});
	}
}

TSubclassOf<UGameplayEffect> FPF2SpellDifficultyClassCalculationsSpec::LoadGE() const
{
	return LoadBlueprint<UGameplayEffect>(this->BlueprintPath, this->SpellDifficultyClassGameEffectCalc);
}
