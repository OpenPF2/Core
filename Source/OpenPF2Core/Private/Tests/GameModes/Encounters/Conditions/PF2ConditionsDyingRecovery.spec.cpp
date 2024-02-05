// OpenPF2 for UE Game Logic, Copyright 2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <AbilitySystemBlueprintLibrary.h>

#include "GameModes/Encounters/PF2EncounterModeOfPlayRuleSetBase.h"

#include "Libraries/PF2AttackStatLibrary.h"
#include "Libraries/PF2DiceLibrary.h"

#include "Tests/PF2SpecBase.h"

#include "Utilities/PF2InterfaceUtilities.h"

BEGIN_DEFINE_PF_SPEC(FPF2ConditionsDyingRecovery,
                     "OpenPF2.Conditions.DyingRecovery",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	UPF2CharacterAttributeSet* TestCharacterAttributeSet;
END_DEFINE_PF_SPEC(FPF2ConditionsDyingRecovery)

void FPF2ConditionsDyingRecovery::Define()
{
	static TSubclassOf<APF2EncounterModeOfPlayRuleSetBase> BP_Encounter_RuleSet;
	static TSubclassOf<UGameplayAbility>                   BP_ConditionCheck_Ability,
	                                                       BP_RecoveryCheck_Ability;
	static TSubclassOf<UGameplayEffect>                    BP_Unconscious_Effect,
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
		BP_Encounter_RuleSet = this->LoadBlueprint<APF2EncounterModeOfPlayRuleSetBase>(
			"/OpenPF2Core/OpenPF2/Optional/ModesOfPlay/Encounters",
			"BP_MoPRS_Encounter"
		);

		BP_ConditionCheck_Ability = this->LoadBlueprint<UGameplayAbility>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GA_ConditionCheck"
		);

		BP_RecoveryCheck_Ability = this->LoadBlueprint<UGameplayAbility>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GA_TurnStart_Dying_RecoveryCheck"
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
		OtherCharacter,
		TScriptInterface<IPF2CharacterInterface>,
		[this],
		{ return PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter()); }
	);

	LET(
		EncounterRuleSet,
		APF2EncounterModeOfPlayRuleSetBase*,
		[this],
		{ return Cast<APF2EncounterModeOfPlayRuleSetBase>(this->World->SpawnActor(BP_Encounter_RuleSet)); }
	);

	// With this random seed, the first 21 rolls expected are:
	// [1]  17,  [2]  6,  [3]  2,  [4] 15,  [5]  9,  [6]  7,  [7] 20,  [8] 8, [9] 7, [10] 12, [11] 12, [12] 8, [13] 3,
	// [14] 11, [15] 18, [16] 12, [17]  8, [18] 20, [19] 18, [20] 16, [21] 1
	LET(
		RandomSeed,
		int32,
		[],
		{ return 8675309; }
	);

	LET(
		GameplayEffectsToApplyBeforeStartOfTurn,
		TArray<TSubclassOf<UGameplayEffect>>,
		[],
		{ return TArray<TSubclassOf<UGameplayEffect>>(); }
	);

	// How many dice rolls to skip before the character's turn starts.
	LET(
		SkippedRollCount,
		int8,
		[],
		{ return 0; }
	);

	BeforeEach([=, this]
	{
		this->SetupWorld();
		this->SetupTestCharacter();

		this->BeginPlay();

		this->TestCharacterAttributeSet =
			const_cast<UPF2CharacterAttributeSet*>(this->TestCharacterAsc->GetSet<UPF2CharacterAttributeSet>());

		// Add all characters to the encounter.
		EncounterRuleSet->OnCharacterAddedToEncounter(this->TestCharacter);
		EncounterRuleSet->OnCharacterAddedToEncounter(*OtherCharacter);

		// Put characters in the desired initiative order (Other Character, Test Character)
		EncounterRuleSet->SetCharacterInitiative(this->TestCharacter, 15);
		EncounterRuleSet->SetCharacterInitiative(*OtherCharacter, 20);

		// Ensure the character starts out already dying.
		this->TestCharacterAttributeSet->HitPoints = 0;
		// ReSharper disable once CppExpressionWithoutSideEffects
		this->ApplyGameplayEffectToTestCharacter(BP_Unconscious_Effect);

		// Ensure character is checking conditions so that tag updates happen as expected.
		this->GrantCharacterAbility(this->TestCharacter, BP_ConditionCheck_Ability);
		this->GrantCharacterAbility(this->TestCharacter, BP_RecoveryCheck_Ability);

		UPF2DiceLibrary::SetRandomSeed(*RandomSeed);

		// Give the test a chance to fast forward to a specific, known roll based on the known seed set above.
		for (uint8 RollIndex = 0; RollIndex < *SkippedRollCount; ++RollIndex)
		{
			UPF2DiceLibrary::Roll(1, 20);
		}

		for (const TSubclassOf<UGameplayEffect>& Effect : *GameplayEffectsToApplyBeforeStartOfTurn)
		{
			// ReSharper disable once CppExpressionWithoutSideEffects
			this->ApplyGameplayEffectToTestCharacter(Effect);
		}

		// Suppress warning about the lack of a player controller; we don't need one for this test.
		this->AddExpectedError(
			"Either this character \\('Character\\[PF2TestCharacter_0\\]'\\) is only controllable by a remote client, or the character does not have an OpenPF2-compatible player controller\\.",
			EAutomationExpectedMessageFlags::Contains
		);

		// Start the character's turn, which should trigger a recovery check.
		EncounterRuleSet->StartTurnForCharacter(this->TestCharacter);
	});

	AfterEach([=, this]
	{
		this->DestroyTestCharacter();
		this->DestroyWorld();
	});

	Describe("when a character is starting a turn with a 'Trait_Condition_Dying_1' tag (DC: 10 + 1 = 11)", [=, this]
	{
		REDEFINE_LET(
			GameplayEffectsToApplyBeforeStartOfTurn,
			TArray<TSubclassOf<UGameplayEffect>>,
			[],
			{ return TArray({ BP_Dying1_Effect }); }
		);

		Describe("when the recovery check at the start of the character's turn is a critical success (natural 20 roll)", [=, this]
		{
			// Skipping ahead so that the next dice roll is a 20.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 6; }
			);

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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_1' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_2' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_3' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_4' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});
		});

		Describe("when the recovery check at the start of the character's turn is a success (11 <= roll < 20)", [=, this]
		{
			// Skipping ahead so that the next dice roll is a 11.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 13; }
			);

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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_1' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_2' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_3' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_4' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});
		});

		Describe("when the recovery check at the start of the character's turn is a failure (1 < roll < 11)", [=, this]
		{
			// Skipping ahead so that the next dice roll is a 6.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 1; }
			);

			It("replaces the 'Trait_Condition_Dying_1' gameplay tag with 'Trait_Condition_Dying_2' on the character", [=, this]
			{
				TestCharacterHasConditionLevel(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying")),
					2
				);
			});

			It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
			{
				TestCharacterHasCondition(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
				);
			});
		});

		Describe("when the recovery check at the start of the character's turn is a critical failure (natural 1 roll)", [=, this]
		{
			// Skipping ahead so that the next dice roll is a 1.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 20; }
			);

			It("replaces the 'Trait_Condition_Dying_1' gameplay tag with 'Trait_Condition_Dying_3' on the character", [=, this]
			{
				TestCharacterHasConditionLevel(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying")),
					3
				);
			});

			It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
			{
				TestCharacterHasCondition(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
				);
			});
		});
	});

	Describe("when a character is starting a turn with a 'Trait_Condition_Dying_2' tag (DC: 10 + 2 = 12)", [=, this]
	{
		REDEFINE_LET(
			GameplayEffectsToApplyBeforeStartOfTurn,
			TArray<TSubclassOf<UGameplayEffect>>,
			[],
			{ return TArray({ BP_Dying2_Effect }); }
		);

		Describe("when the recovery check at the start of the character's turn is a critical success (natural 20 roll)", [=, this]
		{
			// Skipping ahead so that the next dice roll is a 20.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 6; }
			);

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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_1' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_2' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_3' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});

			Describe("when the character has the 'Trait_Condition_Wounded_4' tag", [=, this]
			{
				REDEFINE_LET(
					GameplayEffectsToApplyBeforeStartOfTurn,
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

				It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
				{
					TestCharacterHasCondition(
						this->TestCharacter,
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					);
				});
			});
		});

		Describe("when the recovery check at the start of the character's turn is a success (12 <= roll < 20)", [=, this]
		{
			// Skipping ahead so that the next dice roll is a 12.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 9; }
			);

			It("replaces the 'Trait_Condition_Dying_2' gameplay tag with 'Trait_Condition_Dying_1' on the character", [=, this]
			{
				TestCharacterHasConditionLevel(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying")),
					1
				);
			});

			It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
			{
				TestCharacterHasCondition(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
				);
			});
		});

		Describe("when the recovery check at the start of the character's turn is a failure (2 < roll < 12", [=, this]
		{
			// Skipping ahead so that the next dice roll is an 11.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 13; }
			);

			It("replaces the 'Trait_Condition_Dying_2' gameplay tag with 'Trait_Condition_Dying_3' on the character", [=, this]
			{
				TestCharacterHasConditionLevel(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying")),
					3
				);
			});

			It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
			{
				TestCharacterHasCondition(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
				);
			});
		});

		Describe("when the recovery check at the start of the character's turn is a critical failure (roll <= 2)", [=, this]
		{
			// Skipping ahead so that the next dice roll is a 2.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 2; }
			);

			It("kills the character", [=, this]
			{
				TestCharacterHasCondition(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
				);
			});
		});
	});

	Describe("when a character is starting a turn with a 'Trait_Condition_Dying_3' tag (DC: 10 + 3 = 13)", [=, this]
	{
		REDEFINE_LET(
			GameplayEffectsToApplyBeforeStartOfTurn,
			TArray<TSubclassOf<UGameplayEffect>>,
			[],
			{ return TArray({ BP_Dying3_Effect }); }
		);

		Describe("when the recovery check at the start of the character's turn is a critical success (natural 20 roll)", [=, this]
		{
			// Skipping ahead so that the next dice roll is a 20.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 6; }
			);

			It("replaces the 'Trait_Condition_Dying_3' gameplay tag with 'Trait_Condition_Dying_1' on the character", [=, this]
			{
				TestCharacterHasConditionLevel(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying")),
					1
				);
			});

			It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
			{
				TestCharacterHasCondition(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
				);
			});
		});

		Describe("when the recovery check at the start of the character's turn is a success (13 <= roll < 20)", [=, this]
		{
			// No need to skip ahead -- the next dice roll is a 17.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 0; }
			);

			It("replaces the 'Trait_Condition_Dying_3' gameplay tag with 'Trait_Condition_Dying_2' on the character", [=, this]
			{
				TestCharacterHasConditionLevel(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying")),
					2
				);
			});

			It("retains the 'Trait_Condition_Unconscious' gameplay tag on the character", [=, this]
			{
				TestCharacterHasCondition(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
				);
			});
		});

		Describe("when the recovery check at the start of the character's turn is a failure (3 < roll < 13)", [=, this]
		{
			// Skipping ahead so that the next dice roll is a 12.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 9; }
			);

			It("kills the character", [=, this]
			{
				TestCharacterHasCondition(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
				);
			});
		});

		Describe("when the recovery check at the start of the character's turn is a critical failure (roll <= 3)", [=, this]
		{
			// Skipping ahead so that the next dice roll is a 3.
			REDEFINE_LET(
				SkippedRollCount,
				int8,
				[],
				{ return 12; }
			);

			It("kills the character", [=, this]
			{
				TestCharacterHasCondition(
					this->TestCharacter,
					FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
				);
			});
		});
	});

	Describe("when a character is starting a turn with a 'Trait_Condition_Dying_4' tag (DC: 10 + 4 = 14)", [=, this]
	{
		REDEFINE_LET(
			GameplayEffectsToApplyBeforeStartOfTurn,
			TArray<TSubclassOf<UGameplayEffect>>,
			[],
			{ return TArray({ BP_Dying4_Effect }); }
		);

		It("does not attempt a recovery check because the character is dead", [=, this]
		{
			// Confirm that no recovery check was done by ensuring that the next 4 dice rolls match the first 4 rolls
			// of the test seed. This tells us no dice were rolled, which we can infer means no recovery check was done.
			TestEqual("Next dice roll", UPF2DiceLibrary::Roll(4, 20), {17, 6, 2, 15});
		});
	});
}
