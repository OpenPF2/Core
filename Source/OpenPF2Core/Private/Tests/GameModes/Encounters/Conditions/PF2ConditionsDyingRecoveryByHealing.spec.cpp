// OpenPF2 for UE Game Logic, Copyright 2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


#include <AbilitySystemComponent.h>

#include "Abilities/PF2CharacterAttributeSet.h"

#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2ConditionsDyingRecoveryByHealing,
                     "OpenPF2.Conditions.DyingRecoveryByHealing",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	UPF2CharacterAttributeSet* TestCharacterAttributeSet;
	TSubclassOf<UGameplayEffect> HealEffect;

	/**
	 * Applies healing to the test character via a Gameplay Effect (GE), increasing available hit points.
	 *
	 * @param HealingAmount
	 *	The amount of healing to apply.
	 */
	void HealTestCharacter(const float HealingAmount) const;
END_DEFINE_PF_SPEC(FPF2ConditionsDyingRecoveryByHealing)

void FPF2ConditionsDyingRecoveryByHealing::Define()
{
	static TSubclassOf<UGameplayAbility> BP_ConditionCheck_Ability;
	static TSubclassOf<UGameplayEffect>  BP_Unconscious_Effect,
	                                     BP_Wounded1_Effect,
	                                     BP_Wounded2_Effect,
	                                     BP_Wounded3_Effect,
	                                     BP_Wounded4_Effect,
	                                     BP_Dying1_Effect,
	                                     BP_Dying2_Effect,
	                                     BP_Dying3_Effect,
	                                     BP_Dying4_Effect;

	BeforeAll([&, this]
	{
		this->HealEffect = this->LoadBlueprint<UGameplayEffect>("/OpenPF2Core/OpenPF2/Core", "GE_Heal");

		BP_ConditionCheck_Ability = this->LoadBlueprint<UGameplayAbility>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GA_ConditionCheck"
		);

		BP_Unconscious_Effect = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GE_Condition_Unconscious"
		);

		BP_Wounded1_Effect = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GE_Condition_Wounded_Level1"
		);

		BP_Wounded2_Effect = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GE_Condition_Wounded_Level2"
		);

		BP_Wounded3_Effect = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GE_Condition_Wounded_Level3"
		);

		BP_Wounded4_Effect = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GE_Condition_Wounded_Level4"
			);

		BP_Dying1_Effect = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GE_Condition_Dying_Level1"
		);

		BP_Dying2_Effect = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GE_Condition_Dying_Level2"
		);

		BP_Dying3_Effect = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GE_Condition_Dying_Level3"
		);

		BP_Dying4_Effect = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GE_Condition_Dying_Level4"
		);
	});

	LET(
		GameplayEffectsToApplyBeforeTest,
		TArray<TSubclassOf<UGameplayEffect>>,
		[],
		{ return TArray<TSubclassOf<UGameplayEffect>>(); }
	);

	BeforeEach([=, this]
	{
		this->SetupWorld();
		this->SetupTestCharacter();

		this->BeginPlay();

		this->TestCharacterAttributeSet =
			const_cast<UPF2CharacterAttributeSet*>(this->TestCharacterAsc->GetSet<UPF2CharacterAttributeSet>());

		// Ensure the character starts out already dying.
		this->TestCharacterAttributeSet->HitPoints = 0;
		// ReSharper disable once CppExpressionWithoutSideEffects
		this->ApplyGameplayEffectToTestCharacter(BP_Unconscious_Effect);

		// Ensure character is checking conditions so that tag updates happen as expected.
		this->GrantCharacterAbility(this->TestCharacter, BP_ConditionCheck_Ability);

		for (const TSubclassOf<UGameplayEffect>& Effect : *GameplayEffectsToApplyBeforeTest)
		{
			// ReSharper disable once CppExpressionWithoutSideEffects
			this->ApplyGameplayEffectToTestCharacter(Effect);
		}

		this->HealTestCharacter(1.0f);
	});

	AfterEach([=, this]
	{
		this->DestroyTestCharacter();
		this->DestroyWorld();
	});

	Describe("when a character has a 'Trait_Condition_Dying_1' gameplay tag", [=, this]
	{
		REDEFINE_LET(
			GameplayEffectsToApplyBeforeTest,
			TArray<TSubclassOf<UGameplayEffect>>,
			[],
			{ return TArray({ BP_Dying1_Effect }); }
		);

		Describe("when the character receives healing", [=, this]
		{
			Describe("when the character does not have any 'Trait_Condition_Wounded' tags", [=, this]
			{
				It("removes the 'Trait_Condition_Dying_1' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_1' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						1
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_1' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded1_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_1' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_2' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						2
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_2' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded2_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_1' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_3' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						3
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_3' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded3_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_1' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_4' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						4
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_4' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded4_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_1' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("retains the 'Trait_Condition_Wounded_4' gameplay tag on the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						4
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});
		});
	});

	Describe("when a character has a 'Trait_Condition_Dying_2' gameplay tag", [=, this]
	{
		REDEFINE_LET(
			GameplayEffectsToApplyBeforeTest,
			TArray<TSubclassOf<UGameplayEffect>>,
			[],
			{ return TArray({ BP_Dying2_Effect }); }
		);

		Describe("when the character receives healing", [=, this]
		{
			Describe("when the character does not have any 'Trait_Condition_Wounded' tags", [=, this]
			{
				It("removes the 'Trait_Condition_Dying_2' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_1' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						1
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_1' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded1_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_2' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_2' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						2
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_2' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded2_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_2' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_3' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						3
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_3' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded3_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_2' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_4' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						4
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_4' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded4_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_2' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("retains the 'Trait_Condition_Wounded_4' gameplay tag on the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						4
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});
		});
	});

	Describe("when a character has a 'Trait_Condition_Dying_3' gameplay tag (DC: 10 + 3 = 13)", [=, this]
	{
		REDEFINE_LET(
			GameplayEffectsToApplyBeforeTest,
			TArray<TSubclassOf<UGameplayEffect>>,
			[],
			{ return TArray({ BP_Dying3_Effect }); }
		);

		Describe("when the character receives healing", [=, this]
		{
			Describe("when the character does not have any 'Trait_Condition_Wounded' tags", [=, this]
			{
				It("removes the 'Trait_Condition_Dying_3' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_1' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						1
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_1' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded1_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_3' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_2' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						2
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_2' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded2_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_3' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_3' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						3
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_3' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded3_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_3' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("applies the 'Trait_Condition_Wounded_4' gameplay tag to the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						4
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_4' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeTest,
					TArray<TSubclassOf<UGameplayEffect>>,
					[],
					{ return PF2ArrayUtilities::Concatenate(**Previous, TArray({ BP_Wounded4_Effect })); }
				);

				It("removes the 'Trait_Condition_Dying_3' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					);
				});

				It("retains the 'Trait_Condition_Wounded_4' gameplay tag on the character", [=, this]
				{
					TestCharacterHasConditionLevel(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Wounded")),
						4
					);
				});

				It("removes the 'Trait_Condition_Unconscious' gameplay tag from the character", [=, this]
				{
					TestCharacterNotHaveCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});
		});
	});

	Describe("when a character has a 'Trait_Condition_Dying_4' gameplay tag (DC: 10 + 4 = 14)", [=, this]
	{
		REDEFINE_LET(
			GameplayEffectsToApplyBeforeTest,
			TArray<TSubclassOf<UGameplayEffect>>,
			[],
			{ return TArray({ BP_Dying4_Effect }); }
		);

		Describe("when the character receives healing", [=, this]
		{
			It("has no effect because the character is already dead", [=, this]
			{
				TestCharacterHasConditionLevel(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying")),
					4
				);

				TestCharacterHasCondition(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
				);
			});
		});
	});
}

void FPF2ConditionsDyingRecoveryByHealing::HealTestCharacter(const float HealingAmount) const
{
	FGameplayEffectSpecHandle EffectSpecHandle;

	EffectSpecHandle =
		this->BuildEffectSpec(
			this->HealEffect,
			{
				{"GameplayEffect.Parameter.Healing", HealingAmount},
			}
		);

	this->TestCharacterAsc->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
}
