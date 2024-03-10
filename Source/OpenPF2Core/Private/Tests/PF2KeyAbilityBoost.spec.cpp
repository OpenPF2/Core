// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <GameplayEffect.h>

#include "CharacterStats/PF2CharacterAttributeSet.h"

#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FKeyAbilityBoostSpec,
                     "OpenPF2.KeyAbilityBoosts",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	const FString BlueprintPath                 = TEXT("/OpenPF2Core/OpenPF2/Core/CharacterStats");
	const FString KeyAbilityBoostGameEffectCalc = TEXT("GE_CalcKeyAbilityBoost");

	const TMap<FString, FString> KeyAbilities = {
		{ "Strength",     "AbStrength"     },
		{ "Dexterity",    "AbDexterity"    },
		{ "Constitution", "AbConstitution" },
		{ "Intelligence", "AbIntelligence" },
		{ "Wisdom",       "AbWisdom"       },
		{ "Charisma",     "AbCharisma"     },
	};

	FActiveGameplayEffectHandle TargetEffectHandle;

	TSubclassOf<UGameplayEffect> LoadGE() const;
END_DEFINE_PF_SPEC(FKeyAbilityBoostSpec)

void FKeyAbilityBoostSpec::Define()
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

	for (auto& KeyAbility : this->KeyAbilities)
	{
		const FString KeyAbilityName          = KeyAbility.Key;
		const FString KeyAbilityAttributeName = KeyAbility.Value;

		Describe(FString::Format(TEXT("when the character's key ability is '{0}'"), {KeyAbilityName}), [=, this]()
		{
			BeforeEach([=, this]()
			{
				const UPF2CharacterAttributeSet*    AttributeSet         = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
				FAttributeCapture                   Attributes           = CaptureAbilityAttributes(AttributeSet);
				FGameplayAttributeData*             KeyAbilityAttribute  = Attributes[KeyAbilityAttributeName];
				const FString                       TagName              = FString::Format(TEXT("KeyAbility.{0}"), {KeyAbilityName});
				const TSubclassOf<UGameplayEffect>& EffectBP             = this->LoadGE();

				// Start all attributes from a known value.
				for (const auto& AttributeCapture : Attributes)
				{
					FGameplayAttributeData* Attribute = AttributeCapture.Value;

					*Attribute = 0.0f;
				}

				this->ApplyUnreplicatedTag(TagName);
				this->InitializeAttributeAndApplyEffect(*KeyAbilityAttribute, 15.0f, EffectBP);
			});

			It(FString::Format(TEXT("boosts '{0}'"), {KeyAbilityName}), [=, this]()
			{
				const UPF2CharacterAttributeSet* AttributeSet        = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
				FAttributeCapture                Attributes          = CaptureAbilityAttributes(AttributeSet);
				const FGameplayAttributeData*    KeyAbilityAttribute = Attributes[KeyAbilityAttributeName];

				TestEqual(
					FString::Format(TEXT("{0}.BaseValue"), {KeyAbilityName}),
					KeyAbilityAttribute->GetBaseValue(),
					15.0f
				);

				TestEqual(
					FString::Format(TEXT("{0}.CurrentValue"), {KeyAbilityName}),
					KeyAbilityAttribute->GetCurrentValue(),
					17.0f
				);
			});

			It(TEXT("does not boost any other abilities"), [=, this]()
			{
				const UPF2CharacterAttributeSet* AttributeSet = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();
				FAttributeCapture                Attributes   = CaptureAbilityAttributes(AttributeSet);

				for (const auto& AttributeCapture : Attributes)
				{
					FString                       AttributeName = AttributeCapture.Key;
					const FGameplayAttributeData* Attribute     = AttributeCapture.Value;

					if (AttributeName != KeyAbilityAttributeName)
					{
						TestEqual(
							FString::Format(TEXT("{0}.BaseValue"), {AttributeName}),
							Attribute->GetBaseValue(),
							0.0f
						);

						TestEqual(
							FString::Format(TEXT("{0}.CurrentValue"), {AttributeName}),
							Attribute->GetCurrentValue(),
							0.0f
						);
					}
				}
			});
		});
	}
}

TSubclassOf<UGameplayEffect> FKeyAbilityBoostSpec::LoadGE() const
{
	return LoadBlueprint<UGameplayEffect>(this->BlueprintPath, this->KeyAbilityBoostGameEffectCalc);
}
