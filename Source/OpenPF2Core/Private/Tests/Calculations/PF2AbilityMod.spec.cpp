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

BEGIN_DEFINE_PF_SPEC(FPF2AbilityModSpec,
                     "OpenPF2.AbilityMods",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	const FString BlueprintPath       = TEXT("/OpenPF2Core/OpenPF2/Core/CharacterStats/AbilityModifiers");
	const FString AbModGameEffectCalc = TEXT("GE_CalcAbilityModifiers");

	const TArray<FString> AbModMmcNames = {
		TEXT("MMC_AbModCharisma"),
		TEXT("MMC_AbModConstitution"),
		TEXT("MMC_AbModDexterity"),
		TEXT("MMC_AbModIntelligence"),
		TEXT("MMC_AbModStrength"),
		TEXT("MMC_AbModWisdom"),
	};

	const TArray<FString> AbModGeNames = {
		this->AbModGameEffectCalc,
	};

	TMap<FString, TSubclassOf<UPF2AbilityModifierCalculationBase>> AbModMMCs;
	TMap<FString, TSubclassOf<UGameplayEffect>>                AbModGEs;
	TMap<float, float>                                         AbModMappings;

	void LoadMMCs();
	void LoadGEs();
	void LoadAbModMappings();

	void VerifyModifier(const FString& TargetAbilityAttributeName,
	                    const FString& TargetModifierAttributeName,
	                    const float    AbilityValue,
	                    const float    ExpectedModifier);

	void VerifyCorrectAbilityAffected(const FString& TargetAbilityAttributeName,
	                                  const FString& TargetModifierAttributeName);

	void VerifyModifierRemoved(const FString& TargetAbilityAttributeName,
	                           const FString& TargetModifierAttributeName);
END_DEFINE_PF_SPEC(FPF2AbilityModSpec)

void FPF2AbilityModSpec::Define()
{
	Describe("Blueprint Loading for Ability Mod MMCs", [=, this]()
	{
		BeforeEach([=, this]()
		{
			this->LoadMMCs();
		});

		AfterEach([=, this]()
		{
			this->AbModMMCs.Empty();
		});

		for (const auto& BlueprintName : this->AbModMmcNames)
		{
			It(BlueprintName + " should load", [=, this]()
			{
				const TSubclassOf<UPF2AbilityModifierCalculationBase>& MmcBlueprint = this->AbModMMCs[BlueprintName];

				TestTrue(BlueprintName + " is not null.", IsValid(MmcBlueprint));
			});
		}
	});

	Describe("Blueprint Loading for Ability Mod GEs", [=, this]()
	{
		BeforeEach([=, this]()
		{
			this->LoadGEs();
		});

		AfterEach([=, this]()
		{
			this->AbModGEs.Empty();
		});

		for (const auto& BlueprintName : this->AbModGeNames)
		{
			It(BlueprintName + " should load", [=, this]()
			{
				const TSubclassOf<UGameplayEffect>& EffectBP = this->AbModGEs[BlueprintName];

				TestTrue(BlueprintName + " is not null.", IsValid(EffectBP));
			});
		}
	});

	Describe("Effects of Modifiers", [=, this]()
	{
		BeforeEach([=, this]()
		{
			this->SetupWorld();
			this->SetupTestPawn();

			this->LoadGEs();
			this->LoadAbModMappings();

			this->BeginPlay();
		});

		AfterEach([=, this]()
		{
			this->DestroyTestPawn();
			this->DestroyWorld();

			this->AbModGEs.Empty();
			this->AbModMappings.Empty();
		});

		Describe("Charisma Modifier", [=, this]()
		{
			const FString TargetAbilityAttributeName  = TEXT("AbCharisma");
			const FString TargetModifierAttributeName = TEXT("AbCharismaModifier");

			Describe("when GE is applied", [=, this]()
			{
				It("calculates a modifier based on the current ability score", [=, this]()
				{
					for (const auto ValuePair : this->AbModMappings)
					{
						const float AbilityValue          = ValuePair.Key,
						            ExpectedModifierValue = ValuePair.Value;

						this->VerifyModifier(
							TargetAbilityAttributeName,
							TargetModifierAttributeName,
							AbilityValue,
							ExpectedModifierValue
						);

						// Workaround: Without this, Ability scores from one test iteration seem to affect the next.
						this->DestroyTestPawn();
						this->SetupTestPawn();
					}
				});

				It("applies the calculation only to the associated modifier attribute", [=, this]()
				{
					this->VerifyCorrectAbilityAffected(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});

			Describe("when GE is removed after being applied", [=, this]()
			{
				It("resets the modifier", [=, this]()
				{
					this->VerifyModifierRemoved(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});
		});

		Describe("Constitution Modifier", [=, this]()
		{
			const FString TargetAbilityAttributeName  = TEXT("AbConstitution");
			const FString TargetModifierAttributeName = TEXT("AbConstitutionModifier");

			Describe("when GE is applied", [=, this]()
			{
				It("calculates a modifier based on the current ability score", [=, this]()
				{
					for (const auto ValuePair : this->AbModMappings)
					{
						const float AbilityValue          = ValuePair.Key,
						            ExpectedModifierValue = ValuePair.Value;

						this->VerifyModifier(
							TargetAbilityAttributeName,
							TargetModifierAttributeName,
							AbilityValue,
							ExpectedModifierValue
						);

						// Workaround: Without this, Ability scores from one test iteration seem to affect the next.
						this->DestroyTestPawn();
						this->SetupTestPawn();
					}
				});

				It("applies the calculation only to the associated modifier attribute", [=, this]()
				{
					this->VerifyCorrectAbilityAffected(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});

			Describe("when GE is removed after being applied", [=, this]()
			{
				It("resets the modifier", [=, this]()
				{
					this->VerifyModifierRemoved(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});
		});

		Describe("Dexterity Modifier", [=, this]()
		{
			const FString TargetAbilityAttributeName  = TEXT("AbDexterity");
			const FString TargetModifierAttributeName = TEXT("AbDexterityModifier");

			Describe("when GE is applied", [=, this]()
			{
				It("calculates a modifier based on the current ability score", [=, this]()
				{
					for (const auto ValuePair : this->AbModMappings)
					{
						const float AbilityValue          = ValuePair.Key,
						            ExpectedModifierValue = ValuePair.Value;

						this->VerifyModifier(
							TargetAbilityAttributeName,
							TargetModifierAttributeName,
							AbilityValue,
							ExpectedModifierValue
						);

						// Workaround: Without this, Ability scores from one test iteration seem to affect the next.
						this->DestroyTestPawn();
						this->SetupTestPawn();
					}
				});

				It("applies the calculation only to the associated modifier attribute", [=, this]()
				{
					this->VerifyCorrectAbilityAffected(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});

			Describe("when GE is removed after being applied", [=, this]()
			{
				It("resets the modifier", [=, this]()
				{
					this->VerifyModifierRemoved(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});
		});

		Describe("Intelligence Modifier", [=, this]()
		{
			const FString TargetAbilityAttributeName  = TEXT("AbIntelligence");
			const FString TargetModifierAttributeName = TEXT("AbIntelligenceModifier");

			Describe("when GE is applied", [=, this]()
			{
				It("calculates a modifier based on the current ability score", [=, this]()
				{
					for (const auto ValuePair : this->AbModMappings)
					{
						const float AbilityValue          = ValuePair.Key,
						            ExpectedModifierValue = ValuePair.Value;

						this->VerifyModifier(
							TargetAbilityAttributeName,
							TargetModifierAttributeName,
							AbilityValue,
							ExpectedModifierValue
						);

						// Workaround: Without this, Ability scores from one test iteration seem to affect the next.
						this->DestroyTestPawn();
						this->SetupTestPawn();
					}
				});

				It("applies the calculation only to the associated modifier attribute", [=, this]()
				{
					this->VerifyCorrectAbilityAffected(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});

			Describe("when GE is removed after being applied", [=, this]()
			{
				It("resets the modifier", [=, this]()
				{
					this->VerifyModifierRemoved(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});
		});

		Describe("Strength Modifier", [=, this]()
		{
			const FString TargetAbilityAttributeName  = TEXT("AbStrength");
			const FString TargetModifierAttributeName = TEXT("AbStrengthModifier");

			Describe("when GE is applied", [=, this]()
			{
				It("calculates a modifier based on the current ability score", [=, this]()
				{
					for (const auto ValuePair : this->AbModMappings)
					{
						const float AbilityValue          = ValuePair.Key,
						            ExpectedModifierValue = ValuePair.Value;

						this->VerifyModifier(
							TargetAbilityAttributeName,
							TargetModifierAttributeName,
							AbilityValue,
							ExpectedModifierValue
						);

						// Workaround: Without this, Ability scores from one test iteration seem to affect the next.
						this->DestroyTestPawn();
						this->SetupTestPawn();
					}
				});

				It("applies the calculation only to the associated modifier attribute", [=, this]()
				{
					this->VerifyCorrectAbilityAffected(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});

			Describe("when GE is removed after being applied", [=, this]()
			{
				It("resets the modifier", [=, this]()
				{
					this->VerifyModifierRemoved(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});
		});

		Describe("Wisdom Modifier", [=, this]()
		{
			const FString TargetAbilityAttributeName  = TEXT("AbWisdom");
			const FString TargetModifierAttributeName = TEXT("AbWisdomModifier");

			Describe("when GE is applied", [=, this]()
			{
				It("calculates a modifier based on the current ability score", [=, this]()
				{
					for (const auto ValuePair : this->AbModMappings)
					{
						const float AbilityValue          = ValuePair.Key,
						            ExpectedModifierValue = ValuePair.Value;

						this->VerifyModifier(
							TargetAbilityAttributeName,
							TargetModifierAttributeName,
							AbilityValue,
							ExpectedModifierValue
						);

						// Workaround: Without this, Ability scores from one test iteration seem to affect the next.
						this->DestroyTestPawn();
						this->SetupTestPawn();
					}
				});

				It("applies the calculation only to the associated modifier attribute", [=, this]()
				{
					this->VerifyCorrectAbilityAffected(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});

			Describe("when GE is removed after being applied", [=, this]()
			{
				It("resets the modifier", [=, this]()
				{
					this->VerifyModifierRemoved(TargetAbilityAttributeName, TargetModifierAttributeName);
				});
			});
		});
	});
}

void FPF2AbilityModSpec::LoadMMCs()
{
	for (auto& BlueprintName : this->AbModMmcNames)
	{
		TSubclassOf<UPF2AbilityModifierCalculationBase> CalculationBP =
			LoadBlueprint<UPF2AbilityModifierCalculationBase>(this->BlueprintPath, BlueprintName);

		this->AbModMMCs.Add(BlueprintName, CalculationBP);
	}
}

void FPF2AbilityModSpec::LoadGEs()
{
	for (auto& BlueprintName : this->AbModGeNames)
	{
		TSubclassOf<UGameplayEffect> GameplayEffectBP =
			LoadBlueprint<UGameplayEffect>(this->BlueprintPath, BlueprintName);

		this->AbModGEs.Add(BlueprintName, GameplayEffectBP);
	}
}

void FPF2AbilityModSpec::LoadAbModMappings()
{
	// Table 1-1
	this->AbModMappings.Add(1, -5);
	this->AbModMappings.Add(2, -4);
	this->AbModMappings.Add(3, -4);
	this->AbModMappings.Add(4, -3);
	this->AbModMappings.Add(5, -3);
	this->AbModMappings.Add(6, -2);
	this->AbModMappings.Add(7, -2);
	this->AbModMappings.Add(8, -1);
	this->AbModMappings.Add(9, -1);
	this->AbModMappings.Add(10, 0);
	this->AbModMappings.Add(11, 0);
	this->AbModMappings.Add(12, 1);
	this->AbModMappings.Add(13, 1);
	this->AbModMappings.Add(14, 2);
	this->AbModMappings.Add(15, 2);
	this->AbModMappings.Add(16, 3);
	this->AbModMappings.Add(17, 3);
	this->AbModMappings.Add(18, 4);
	this->AbModMappings.Add(19, 4);
	this->AbModMappings.Add(20, 5);
	this->AbModMappings.Add(21, 5);
	this->AbModMappings.Add(22, 6);
	this->AbModMappings.Add(23, 6);
	this->AbModMappings.Add(24, 7);
	this->AbModMappings.Add(25, 7);
}

void FPF2AbilityModSpec::VerifyModifier(const FString& TargetAbilityAttributeName,
                                        const FString& TargetModifierAttributeName,
                                        const float    AbilityValue,
                                        const float    ExpectedModifier)
{
	const TSubclassOf<UGameplayEffect>& EffectBP = this->AbModGEs[this->AbModGameEffectCalc];

	if (IsValid(EffectBP))
	{
		const UPF2CharacterAttributeSet  *AttributeSet            = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
		FAttributeCapture                Attributes               = CaptureAttributes(AttributeSet);
		FGameplayAttributeData           *TargetAbilityAttribute  = Attributes[TargetAbilityAttributeName],
		                                 *TargetModifierAttribute = Attributes[TargetModifierAttributeName];

		// Sanity check test logic.
		TestNotEqual("Captured at least one attribute", Attributes.Num(), 0);

		// Initialize modifier a known value.
		*TargetModifierAttribute = 0.0f;

		this->InitializeAttributeAndApplyEffect(*TargetAbilityAttribute, AbilityValue, EffectBP);

		TestEqual(
			FString::Format(
				TEXT("{0}.BaseValue(for ability score '{1}')"), {TargetModifierAttributeName, AbilityValue}
			),
			TargetModifierAttribute->GetBaseValue(),
			0.0f
		);

		TestEqual(
			FString::Format(
				TEXT("{0}.CurrentValue(for ability score '{1}')"), {TargetModifierAttributeName, AbilityValue}
			),
			TargetModifierAttribute->GetCurrentValue(),
			ExpectedModifier
		);
	}
	else
	{
		AddWarning("GE is not loaded.");
	}
}

void FPF2AbilityModSpec::VerifyCorrectAbilityAffected(const FString& TargetAbilityAttributeName,
                                                      const FString& TargetModifierAttributeName)
{
	const TSubclassOf<UGameplayEffect>& EffectBP = this->AbModGEs[this->AbModGameEffectCalc];

	if (IsValid(EffectBP))
	{
		const UPF2CharacterAttributeSet* AttributeSet           = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
		FAttributeCapture                AbilityAttributes      = CaptureAbilityAttributes(AttributeSet),
		                                 ModifierAttributes     = CaptureAbilityModifierAttributes(AttributeSet);
		FGameplayAttributeData*          TargetAbilityAttribute = AbilityAttributes[TargetAbilityAttributeName];

		// Sanity check test logic.
		TestNotEqual("Captured at least one ability attribute", AbilityAttributes.Num(), 0);
		TestNotEqual("Captured at least one modifier attribute", ModifierAttributes.Num(), 0);

		// Start all attributes at a known value.
		for (const auto AttributePair : AbilityAttributes)
		{
			FGameplayAttributeData& CurrentAttribute = *(AttributePair.Value);

			// Attribute value, from which the modifier is derived.
			CurrentAttribute = 12.0f;
		}

		// Start all attribute modifiers from a known value.
		for (const auto AttributePair : ModifierAttributes)
		{
			FGameplayAttributeData& CurrentAttribute = *(AttributePair.Value);

			// From Table 1-1 in Core Rulebook: Output modifier for attribute value of 12 => 1
			CurrentAttribute = 1.0f;
		}

		// This sets _only_ the target ability to 13, which should result in a modifier of +3
		this->InitializeAttributeAndApplyEffect(*TargetAbilityAttribute, 16.0f, EffectBP);

		for (const auto AttributePair : AbilityAttributes)
		{
			FGameplayAttributeData& CurrentAttribute     = *(AttributePair.Value);
			FString                 CurrentAttributeName = AttributePair.Key;

			// Current Value is Target Attribute, upon which modifier is based
			if (CurrentAttributeName == TargetAbilityAttributeName)
			{
				TestEqual(
					CurrentAttributeName + ".BaseValue",
					CurrentAttribute.GetBaseValue(),
					16.0f
				);

				TestEqual(
					CurrentAttributeName + ".CurrentValue",
					CurrentAttribute.GetCurrentValue(),
					16.0f
				);
			}
			// Current Value is another attribute
			else
			{
				TestEqual(
					CurrentAttributeName + ".BaseValue",
					CurrentAttribute.GetBaseValue(),
					12.0f
				);

				TestEqual(
					CurrentAttributeName + ".CurrentValue",
					CurrentAttribute.GetCurrentValue(),
					12.0f
				);
			}
		}

		for (const auto AttributePair : ModifierAttributes)
		{
			FGameplayAttributeData& CurrentAttribute     = *(AttributePair.Value);
			FString                 CurrentAttributeName = AttributePair.Key;

			// Current Value is Target Attribute Modifier
			if (CurrentAttributeName == TargetModifierAttributeName)
			{
				TestEqual(
					CurrentAttributeName + ".BaseValue",
					CurrentAttribute.GetBaseValue(),
					1.0f
				);

				// Ability score 16 => +3 modifier, per Table 1-1 in Core Rulebook.
				TestEqual(
					CurrentAttributeName + ".CurrentValue",
					CurrentAttribute.GetCurrentValue(),
					3.0f
				);
			}
			// Current Value is a Modifier for another attribute
			else
			{
				TestEqual(
					CurrentAttributeName + ".BaseValue",
					CurrentAttribute.GetBaseValue(),
					1.0f
				);

				TestEqual(
					CurrentAttributeName + ".CurrentValue",
					CurrentAttribute.GetCurrentValue(),
					1.0f
				);
			}
		}
	}
	else
	{
		AddWarning("GE is not loaded.");
	}
}

void FPF2AbilityModSpec::VerifyModifierRemoved(const FString& TargetAbilityAttributeName,
                                               const FString& TargetModifierAttributeName)
{
	const TSubclassOf<UGameplayEffect>& EffectBP = this->AbModGEs[this->AbModGameEffectCalc];

	if (IsValid(EffectBP))
	{
		const UPF2CharacterAttributeSet* AttributeSet            = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
		FAttributeCapture                Attributes              = CaptureAttributes(AttributeSet);
		FGameplayAttributeData*          TargetAbilityAttribute  = Attributes[TargetAbilityAttributeName];
		const FGameplayAttributeData*    TargetModifierAttribute = Attributes[TargetModifierAttributeName];

		const FActiveGameplayEffectHandle EffectHandle =
			this->InitializeAttributeAndApplyEffect(*TargetAbilityAttribute, 13.0f, EffectBP);

		// Sanity check test logic.
		TestNotEqual("Captured at least one attribute", Attributes.Num(), 0);

		this->TestPawnAsc->RemoveActiveGameplayEffect(EffectHandle);

		// Target Ability upon which modifier is based will not be reset because we set it manually in
		// InitializeAttributeAndApplyEffect().
		TestEqual(
			TargetAbilityAttributeName + ".BaseValue",
			TargetAbilityAttribute->GetBaseValue(),
			13.0f
		);

		TestEqual(
			TargetAbilityAttributeName + ".CurrentValue",
			TargetAbilityAttribute->GetCurrentValue(),
			13.0f
		);

		// Target Ability Modifier *should* be reset to 0 now that the GE was removed.
		TestEqual(
			TargetModifierAttributeName + ".BaseValue",
			TargetModifierAttribute->GetBaseValue(),
			0.0f
		);

		TestEqual(
			TargetModifierAttributeName + ".CurrentValue",
			TargetModifierAttribute->GetCurrentValue(),
			0.0f
		);
	}
	else
	{
		AddWarning("GE is not loaded.");
	}
}
