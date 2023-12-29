// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2CharacterAttributeSet.h"
#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2SkillModifierCalculationSpec,
                     "OpenPF2.SkillModifierCalculations",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	const FString BlueprintPath       = TEXT("/OpenPF2Core/OpenPF2/Core/Calculations/Skills");
	const FString SkillGameEffectCalc = TEXT("GE_CalcSkillModifiers");

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

	void VerifySkillModifier(const FString& AbModFriendlyName,
	                         const FString& AbModAttributeName,
	                         const FString& SkillModAttributeFriendlyName,
	                         const FString& SkillModAttributeName);

	void VerifySkillModifier(const FString& AbModAttributeName,
	                         const float    AbModScore,
	                         const FString& SkillModAttributeFriendlyName,
	                         const FString& SkillModAttributeName,
	                         const FString& ProficiencyLevel,
	                         const float    ExpectedSkillMod);
END_DEFINE_PF_SPEC(FPF2SkillModifierCalculationSpec)

void FPF2SkillModifierCalculationSpec::Define()
{
	BeforeEach([=, this]()
	{
		this->SetupWorld();
		this->SetupPawn();

		this->BeginPlay();
	});

	AfterEach([=, this]()
	{
		this->DestroyPawn();
		this->DestroyWorld();
	});

	Describe("Acrobatics Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Dexterity");
		const FString AbModAttributeName            = TEXT("AbDexterityModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Acrobatics");
		const FString SkillModAttributeName         = TEXT("SkAcrobaticsModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Arcana Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Intelligence");
		const FString AbModAttributeName            = TEXT("AbIntelligenceModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Arcana");
		const FString SkillModAttributeName         = TEXT("SkArcanaModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Athletics Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Strength");
		const FString AbModAttributeName            = TEXT("AbStrengthModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Athletics");
		const FString SkillModAttributeName         = TEXT("SkAthleticsModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Crafting Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Intelligence");
		const FString AbModAttributeName            = TEXT("AbIntelligenceModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Crafting");
		const FString SkillModAttributeName         = TEXT("SkCraftingModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Deception Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Charisma");
		const FString AbModAttributeName            = TEXT("AbCharismaModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Deception");
		const FString SkillModAttributeName         = TEXT("SkDeceptionModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Diplomacy Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Charisma");
		const FString AbModAttributeName            = TEXT("AbCharismaModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Diplomacy");
		const FString SkillModAttributeName         = TEXT("SkDiplomacyModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Intimidation Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Charisma");
		const FString AbModAttributeName            = TEXT("AbCharismaModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Intimidation");
		const FString SkillModAttributeName         = TEXT("SkIntimidationModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Lore 1 Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Intelligence");
		const FString AbModAttributeName            = TEXT("AbIntelligenceModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Lore1");
		const FString SkillModAttributeName         = TEXT("SkLore1Modifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Lore 2 Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Intelligence");
		const FString AbModAttributeName            = TEXT("AbIntelligenceModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Lore2");
		const FString SkillModAttributeName         = TEXT("SkLore2Modifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Medicine Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Wisdom");
		const FString AbModAttributeName            = TEXT("AbWisdomModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Medicine");
		const FString SkillModAttributeName         = TEXT("SkMedicineModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Nature Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Wisdom");
		const FString AbModAttributeName            = TEXT("AbWisdomModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Nature");
		const FString SkillModAttributeName         = TEXT("SkNatureModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Occultism Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Intelligence");
		const FString AbModAttributeName            = TEXT("AbIntelligenceModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Occultism");
		const FString SkillModAttributeName         = TEXT("SkOccultismModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Performance Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Charisma");
		const FString AbModAttributeName            = TEXT("AbCharismaModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Performance");
		const FString SkillModAttributeName         = TEXT("SkPerformanceModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Religion Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Wisdom");
		const FString AbModAttributeName            = TEXT("AbWisdomModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Religion");
		const FString SkillModAttributeName         = TEXT("SkReligionModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Society Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Intelligence");
		const FString AbModAttributeName            = TEXT("AbIntelligenceModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Society");
		const FString SkillModAttributeName         = TEXT("SkSocietyModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Stealth Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Dexterity");
		const FString AbModAttributeName            = TEXT("AbDexterityModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Stealth");
		const FString SkillModAttributeName         = TEXT("SkStealthModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Survival Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Wisdom");
		const FString AbModAttributeName            = TEXT("AbWisdomModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Survival");
		const FString SkillModAttributeName         = TEXT("SkSurvivalModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});

	Describe("Thievery Skill", [=, this]
	{
		const FString AbModFriendlyName             = TEXT("Dexterity");
		const FString AbModAttributeName            = TEXT("AbDexterityModifier");
		const FString SkillModAttributeFriendlyName = TEXT("Thievery");
		const FString SkillModAttributeName         = TEXT("SkThieveryModifier");

		this->VerifySkillModifier(
			AbModFriendlyName,
			AbModAttributeName,
			SkillModAttributeFriendlyName,
			SkillModAttributeName
		);
	});
}

TSubclassOf<UGameplayEffect> FPF2SkillModifierCalculationSpec::LoadGE() const
{
	return LoadBlueprint<UGameplayEffect>(this->BlueprintPath, this->SkillGameEffectCalc);
}

void FPF2SkillModifierCalculationSpec::VerifySkillModifier(const FString& AbModFriendlyName,
                                                           const FString& AbModAttributeName,
                                                           const FString& SkillModAttributeFriendlyName,
                                                           const FString& SkillModAttributeName)
{
	for (const auto AbilityValues : this->ExpectedValues)
	{
		const float                AbModScore     = AbilityValues.Key;
		const TMap<FString, float> TrainingScores = AbilityValues.Value;

		Describe(FString::Format(TEXT("when the character has a '{0}' Modifier of '{1}'"), {AbModFriendlyName, FString::FormatAsNumber(AbModScore)}), [=, this]()
		{
			for (const auto ProficiencyValues : TrainingScores)
			{
				const FString ProficiencyLevel  = ProficiencyValues.Key;
				const float   ExpectedSkillMod = ProficiencyValues.Value;

				Describe(FString::Format(TEXT("when the character is '{0}' in '{1}' skill"), {ProficiencyLevel, SkillModAttributeFriendlyName}), [=, this]()
				{
					It(FString::Format(TEXT("calculates a '{0}' skill of '{1}'"), {SkillModAttributeFriendlyName, FString::FormatAsNumber(ExpectedSkillMod)}), [=, this]()
					{
						this->VerifySkillModifier(
							AbModAttributeName,
							AbModScore,
							SkillModAttributeFriendlyName,
							SkillModAttributeName,
							ProficiencyLevel,
							ExpectedSkillMod
						);
					});
				});
			}
		});
	}
}

void FPF2SkillModifierCalculationSpec::VerifySkillModifier(const FString& AbModAttributeName,
                                                           const float    AbModScore,
                                                           const FString& SkillModAttributeFriendlyName,
                                                           const FString& SkillModAttributeName,
                                                           const FString& ProficiencyLevel,
                                                           const float    ExpectedSkillMod)
{
	const UPF2CharacterAttributeSet*    AttributeSet   = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
	FAttributeCapture                   Attributes     = CaptureAttributes(AttributeSet);
	FGameplayAttributeData*             AbModAttribute = Attributes[AbModAttributeName];
	const FGameplayAttributeData*       SkillAttribute = Attributes[SkillModAttributeName];
	const TSubclassOf<UGameplayEffect>& EffectBP       = this->LoadGE();

	this->ApplyUnreplicatedTag(FString::Format(TEXT("Skill.{0}.{1}"), {SkillModAttributeFriendlyName, ProficiencyLevel}));
	this->InitializeAttributeAndApplyEffect(*AbModAttribute, AbModScore, EffectBP);

	TestEqual(
		FString::Format(TEXT("{0}.BaseValue"), {SkillModAttributeName}),
		SkillAttribute->GetBaseValue(),
		0.0f
	);

	TestEqual(
		FString::Format(TEXT("{0}.CurrentValue"), {SkillModAttributeName}),
		SkillAttribute->GetCurrentValue(),
		ExpectedSkillMod
	);
}
