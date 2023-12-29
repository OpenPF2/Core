// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2CharacterAttributeSet.h"
#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2ArmorClassCalculationSpec,
                     "OpenPF2.ArmorClassCalculations",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	const FString BlueprintPath         = TEXT("/OpenPF2Core/OpenPF2/Core/Calculations");
	const FString ClassDcGameEffectCalc = TEXT("GE_CalcArmorClass");

	const FString DexterityAbModAttributeName = TEXT("AbDexterityModifier");
	const FString AcModAttributeName          = TEXT("ArmorClass");

	const TMap<FString, float> ProficiencyValues =
	{
		{"Untrained", 0.0f},
		{"Trained",   3.0f},
		{"Expert",    5.0f},
		{"Master",    7.0f},
		{"Legendary", 9.0f},
	};

	const TArray<FString> ArmorClasses =
	{
		"Unarmored",
		"Light",
		"Medium",
		"Heavy"
	};

	const TArray<float> DexterityModInputs =
	{
		-5,
		0,
		3
	};

	TSubclassOf<UGameplayEffect> LoadGE() const;
END_DEFINE_PF_SPEC(FPF2ArmorClassCalculationSpec)

void FPF2ArmorClassCalculationSpec::Define()
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

	for (auto& DexterityMod : this->DexterityModInputs)
	{
		Describe(FString::Format(TEXT("when Dexterity Modifier is '{0}'"), {FString::FormatAsNumber(DexterityMod)}), [=, this]()
		{
			BeforeEach([=, this]()
			{
				const UPF2CharacterAttributeSet* AttributeSet = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
				FAttributeCapture                Attributes   = CaptureAbilityModifierAttributes(AttributeSet);

				*(Attributes[this->DexterityAbModAttributeName]) = DexterityMod;
			});

			for (auto& ArmorClassEquipped : this->ArmorClasses)
			{
				Describe(FString::Format(TEXT("when character's equipped Armor Class is '{0}'"), {ArmorClassEquipped}), [=, this]()
				{
					BeforeEach([=, this]()
					{
						this->ApplyUnreplicatedTag(FString::Format(TEXT("Armor.Equipped.{0}"), {ArmorClassEquipped}));
					});

					for (auto& ArmorClassProficient : this->ArmorClasses)
					{
						for (auto& Proficiency : ProficiencyValues)
						{
							const FString ProficiencyLevel = Proficiency.Key;
							const float   ProficiencyValue = Proficiency.Value;

							Describe(FString::Format(TEXT("when the character is '{0}' in '{1}' Armor"), {ProficiencyLevel, ArmorClassProficient}), [=, this]()
							{
								float ExpectedAcMod;

								BeforeEach([=, this]()
								{
									this->ApplyUnreplicatedTag(
										FString::Format(
											TEXT("Armor.Category.{0}.{1}"),
											{ArmorClassProficient, ProficiencyLevel}
										)
									);
								});

								if (ArmorClassEquipped == ArmorClassProficient)
								{
									ExpectedAcMod = 10 + DexterityMod + ProficiencyValue;
								}
								else
								{
									ExpectedAcMod = 10 + DexterityMod;
								}

								It(FString::Format(TEXT("calculates an Armor Class modifier of '{0}'"), {FString::FormatAsNumber(ExpectedAcMod)}), [=, this]()
								{
									const UPF2CharacterAttributeSet*    AttributeSet = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
									FAttributeCapture                   Attributes   = CaptureAttributes(AttributeSet);
									FGameplayAttributeData*             AcAttribute  = Attributes[this->AcModAttributeName];
									const TSubclassOf<UGameplayEffect>& EffectBP     = this->LoadGE();

									this->InitializeAttributeAndApplyEffect(*AcAttribute, 0.0f, EffectBP);

									TestEqual(
										FString::Format(TEXT("{0}.BaseValue"), {this->AcModAttributeName}),
										AcAttribute->GetBaseValue(),
										0.0f
									);

									TestEqual(
										FString::Format(TEXT("{0}.CurrentValue"), {this->AcModAttributeName}),
										AcAttribute->GetCurrentValue(),
										ExpectedAcMod
									);
								});
							});
						}
					}
				});
			}
		});
	}
}

TSubclassOf<UGameplayEffect> FPF2ArmorClassCalculationSpec::LoadGE() const
{
	return LoadBlueprint<UGameplayEffect>(this->BlueprintPath, this->ClassDcGameEffectCalc);
}
