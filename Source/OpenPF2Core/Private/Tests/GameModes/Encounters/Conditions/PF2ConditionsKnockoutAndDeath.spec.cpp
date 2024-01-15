// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <AbilitySystemBlueprintLibrary.h>

#include "Calculations/PF2DegreeOfSuccess.h"

#include "GameModes/Encounters/PF2EncounterModeOfPlayRuleSetBase.h"

#include "Libraries/PF2AttackStatLibrary.h"

#include "Tests/PF2SpecBase.h"

#include "Utilities/PF2InterfaceUtilities.h"

BEGIN_DEFINE_PF_SPEC(FPF2ConditionsKnockoutAndDeath,
                     "OpenPF2.Conditions.KnockoutAndDeath",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	UPF2CharacterAttributeSet* TestCharacterAttributeSet;

	/**
	 * Applies a damage gameplay effect (GE) to the target character.
	 *
	 * The damage can optionally simulate the specified tags on the source, with an attack having the specified degree
	 * of success.
	 *
	 * @param Attacker
	 *	The character to simulate as the instigator of the damage.
	 * @param AttackAbilityHandle
	 *	The handle of the ability to use as the source of the attack.
	 * @param DamageEffect
	 *	The damage GE to apply.
	 * @param DegreeOfSuccess
	 *	The optional degree of success value to simulate for the bleed attack.
	 * @param DamageAmount
	 *	The optional amount of damage to inflict.
	 */
	void ApplyDamageToTestCharacter(
		const TScriptInterface<IPF2CharacterInterface> Attacker,
		const FGameplayAbilitySpecHandle&              AttackAbilityHandle,
		const TSubclassOf<UGameplayEffect>             DamageEffect,
		const EPF2DegreeOfSuccess                      DegreeOfSuccess      = EPF2DegreeOfSuccess::None,
		const float                                    DamageAmount         = 20.0f) const;

	/**
	 * Applies an arbitrary Gameplay Effect (GE) to the test character.
	 *
	 * This should be used for passive GEs and stat updates, not attack GEs.
	 *
	 * @param EffectType
	 *	The type of GE to apply to the character.
	 *
	 * @return
	 *	The handle of the GE, now active on the character.
	 */
	FActiveGameplayEffectHandle ApplyGameplayEffectToTestCharacter(const TSubclassOf<UGameplayEffect>& EffectType) const;
END_DEFINE_PF_SPEC(FPF2ConditionsKnockoutAndDeath)

void FPF2ConditionsKnockoutAndDeath::Define()
{
	static TSubclassOf<APF2EncounterModeOfPlayRuleSetBase> BP_Encounter_RuleSet;
	static TSubclassOf<UGameplayAbility>                   BP_ConditionCheck_Ability;
	static TSubclassOf<UGameplayEffect>                    BP_InitializeBaseHealthStats,
	                                                       BP_BleedDamage_Effect,
	                                                       BP_Wounded1_Effect,
	                                                       BP_Wounded2_Effect,
	                                                       BP_Wounded3_Effect,
	                                                       BP_Wounded4_Effect;

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

		// We use the human base stats GE for this because it grants about 8 HP of health.
		BP_InitializeBaseHealthStats = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Optional/Ancestries/Human",
			"GE_Ancestry_Human_Base"
		);

		BP_BleedDamage_Effect = this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Optional/DamageTypes",
			"GE_ApplyBleedDamage"
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
	});

	LET(
		EncounterRuleSet,
		APF2EncounterModeOfPlayRuleSetBase*,
		[this],
		{ return Cast<APF2EncounterModeOfPlayRuleSetBase>(this->World->SpawnActor(BP_Encounter_RuleSet)); }
	);

	LET(
		AttackDegreeOfSuccess,
		EPF2DegreeOfSuccess,
		[],
		{ return EPF2DegreeOfSuccess::None; }
	);

	LET(
		EffectSpecsAppliedToTargetBeforeAttack,
		TArray<FGameplayEffectSpecHandle>,
		[],
		{ return TArray<FGameplayEffectSpecHandle>(); }
	);

	LET(
		TagsAppliedToAbilityBeforeAttack,
		FGameplayTagContainer,
		[],
		{ return FGameplayTagContainer(); }
	);

	LET(
		OtherCharacter1,
		TScriptInterface<IPF2CharacterInterface>,
		[this],
		{ return PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter()); }
	);

	LET(
		OtherCharacter2,
		TScriptInterface<IPF2CharacterInterface>,
		[this],
		{ return PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter()); }
	);

	LET(
		Wounded1GeSpec,
		FGameplayEffectSpecHandle,
		[this],
		{ return this->BuildEffectSpec(BP_Wounded1_Effect); }
	);

	LET(
		Wounded2GeSpec,
		FGameplayEffectSpecHandle,
		[this],
		{ return this->BuildEffectSpec(BP_Wounded2_Effect); }
	);

	LET(
		Wounded3GeSpec,
		FGameplayEffectSpecHandle,
		[this],
		{ return this->BuildEffectSpec(BP_Wounded3_Effect); }
	);

	LET(
		Wounded4GeSpec,
		FGameplayEffectSpecHandle,
		[this],
		{ return this->BuildEffectSpec(BP_Wounded4_Effect); }
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
		EncounterRuleSet->OnCharacterAddedToEncounter(*OtherCharacter1);
		EncounterRuleSet->OnCharacterAddedToEncounter(*OtherCharacter2);

		// Put characters in the desired initiative order (Character 2, Character 1, Test Character)
		EncounterRuleSet->SetCharacterInitiative(this->TestCharacter, 15);
		EncounterRuleSet->SetCharacterInitiative(*OtherCharacter1, 20);
		EncounterRuleSet->SetCharacterInitiative(*OtherCharacter2, 22);

		// Ensure character is checking conditions so that tag updates happen as expected.
		this->GrantCharacterAbility(this->TestCharacter, BP_ConditionCheck_Ability);

		// ReSharper disable once CppExpressionWithoutSideEffects
		// Ensure target character has some health.
		this->ApplyGameplayEffectToTestCharacter(BP_InitializeBaseHealthStats);

		// Sanity check initiative preconditions.
		TestArrayEquals(
			"GetAllCharactersInInitiativeOrder()",
			EncounterRuleSet->GetAllCharactersInInitiativeOrder(),
			{
				*OtherCharacter2,
				*OtherCharacter1,
				this->TestCharacter,
			}
		);

		// Suppress warning about the lack of a player controller; we don't need one for this test.
		this->AddExpectedError(
			"Either this character \\('Character\\[PF2TestCharacter_1\\]'\\) is only controllable by a remote client, or the character does not have an OpenPF2-compatible player controller\\.",
			EAutomationExpectedMessageFlags::Contains
		);

		// Advance to OtherCharacter1's turn.
		EncounterRuleSet->StartTurnForCharacter(*OtherCharacter1);
	});

	AfterEach([=, this]
	{
		this->DestroyTestCharacter();
		this->DestroyWorld();
	});

	Describe("when an attack fails to knock out a character", [=, this]
	{
		BeforeEach([=, this]
		{
			const FGameplayAbilitySpecHandle FakeAttackAbilityHandle =
				this->GrantCharacterFakeAbility(*OtherCharacter1, *TagsAppliedToAbilityBeforeAttack);

			for (const auto& EffectSpec : *EffectSpecsAppliedToTargetBeforeAttack)
			{
				this->TestCharacterAsc->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
			}

			this->ApplyDamageToTestCharacter(
				*OtherCharacter1,
				FakeAttackAbilityHandle,
				BP_BleedDamage_Effect,
				*AttackDegreeOfSuccess,
				7
			);
		});

		Describe("when the attack that fails to knock out the character is lethal", [=, this]
		{
			REDEFINE_LET(
				TagsAppliedToAbilityBeforeAttack,
				FGameplayTagContainer,
				[],
				{ return FGameplayTagContainer(); }
			);

			Describe("when the attack is a success", [=, this]
			{
				REDEFINE_LET(
					AttackDegreeOfSuccess,
					EPF2DegreeOfSuccess,
					[],
					{ return EPF2DegreeOfSuccess::Success; }
				);

				It("has no effect on the initiative position of the character", [=, this]
				{
					TestArrayEquals(
						"GetAllCharactersInInitiativeOrder()",
						EncounterRuleSet->GetAllCharactersInInitiativeOrder(),
						{
							*OtherCharacter2,
							*OtherCharacter1,
							this->TestCharacter,
						}
					);
				});

				It("does not apply the 'Trait_Condition_Unconscious' gameplay tag to the character", [=, this]
				{
					TestFalse(
						TEXT("Has tag 'Trait.Condition.Unconscious'"),
						this->TestCharacterAsc->HasMatchingGameplayTag(
							FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
						)
					);
				});

				Describe("when the character does not have any 'Trait_Condition_Wounded' tags", [=, this]
				{
					It("does not apply any 'Trait_Condition_Dying' gameplay tags to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
							)
						);
					});

					It("does not kill the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_1' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded1GeSpec],
						{ return TArray{ *Wounded1GeSpec }; }
					);

					It("does not apply any 'Trait_Condition_Dying' gameplay tags to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
							)
						);
					});

					It("does not kill the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_2' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded2GeSpec],
						{ return TArray{ *Wounded2GeSpec }; }
					);

					It("does not apply any 'Trait_Condition_Dying' gameplay tags to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
							)
						);
					});

					It("does not kill the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_3' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded3GeSpec],
						{ return TArray{ *Wounded3GeSpec }; }
					);

					It("does not apply any 'Trait_Condition_Dying' gameplay tags to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
							)
						);
					});

					It("does not kill the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_4' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded4GeSpec],
						{ return TArray{ *Wounded4GeSpec }; }
					);

					It("does not apply any 'Trait_Condition_Dying' gameplay tags to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
							)
						);
					});

					It("does not kill the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});
			});

			Describe("when the attack is a critical success", [=, this]
			{
				REDEFINE_LET(
					AttackDegreeOfSuccess,
					EPF2DegreeOfSuccess,
					[],
					{ return EPF2DegreeOfSuccess::CriticalSuccess; }
				);

				It("has no effect on the initiative position of the character", [=, this]
				{
					TestArrayEquals(
						"GetAllCharactersInInitiativeOrder()",
						EncounterRuleSet->GetAllCharactersInInitiativeOrder(),
						{
							*OtherCharacter2,
							*OtherCharacter1,
							this->TestCharacter,
						}
					);
				});

				It("does not apply the 'Trait_Condition_Unconscious' gameplay tag to the character", [=, this]
				{
					TestFalse(
						TEXT("Has tag 'Trait.Condition.Unconscious'"),
						this->TestCharacterAsc->HasMatchingGameplayTag(
							FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
						)
					);
				});

				Describe("when the character does not have any 'Trait_Condition_Wounded' tags", [=, this]
				{
					It("does not apply any 'Trait_Condition_Dying' gameplay tags to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
							)
						);
					});

					It("does not kill the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_1' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded1GeSpec],
						{ return TArray{ *Wounded1GeSpec }; }
					);

					It("does not apply any 'Trait_Condition_Dying' gameplay tags to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
							)
						);
					});

					It("does not kill the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_2' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded2GeSpec],
						{ return TArray{ *Wounded2GeSpec }; }
					);

					It("does not apply any 'Trait_Condition_Dying' gameplay tags to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
							)
						);
					});

					It("does not kill the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_3' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded3GeSpec],
						{ return TArray{ *Wounded3GeSpec }; }
					);

					It("does not apply any 'Trait_Condition_Dying' gameplay tags to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
							)
						);
					});

					It("does not kill the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_4' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded4GeSpec],
						{ return TArray{ *Wounded4GeSpec }; }
					);

					It("does not apply any 'Trait_Condition_Dying' gameplay tags to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
							)
						);
					});

					It("does not kill the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});
			});
		});

		Describe("when the attack that knocks out the Character is nonlethal", [=, this]
		{
			REDEFINE_LET(
				TagsAppliedToAbilityBeforeAttack,
				FGameplayTagContainer,
				[this],
				{return FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Trait.Effect.Nonlethal")); }
			);

			It("has no effect on the initiative position of the character", [=, this]
			{
				TestArrayEquals(
					"GetAllCharactersInInitiativeOrder()",
					EncounterRuleSet->GetAllCharactersInInitiativeOrder(),
					{
						*OtherCharacter2,
						*OtherCharacter1,
						this->TestCharacter,
					}
				);
			});

			It("does not apply the 'Trait_Condition_Unconscious' gameplay tag to the character", [=, this]
			{
				TestFalse(
					TEXT("Has tag 'Trait.Condition.Unconscious'"),
					this->TestCharacterAsc->HasMatchingGameplayTag(
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					)
				);
			});

			It("does not apply any 'Trait_Condition_Dying' gameplay tag to the character", [=, this]
			{
				TestFalse(
					TEXT("Has tag 'Trait.Condition.Dying'"),
					this->TestCharacterAsc->HasMatchingGameplayTag(
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					)
				);
			});
		});
	});

	Describe("when an attack knocks out a character", [=, this]
	{
		BeforeEach([=, this]
		{
			const FGameplayAbilitySpecHandle FakeAttackAbilityHandle =
				this->GrantCharacterFakeAbility(*OtherCharacter1, *TagsAppliedToAbilityBeforeAttack);

			for (const auto& EffectSpec : *EffectSpecsAppliedToTargetBeforeAttack)
			{
				this->TestCharacterAsc->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
			}

			this->ApplyDamageToTestCharacter(
				*OtherCharacter1,
				FakeAttackAbilityHandle,
				BP_BleedDamage_Effect,
				*AttackDegreeOfSuccess
			);
		});

		Describe("when the attack that knocks out the character is lethal", [=, this]
		{
			REDEFINE_LET(
				TagsAppliedToAbilityBeforeAttack,
				FGameplayTagContainer,
				[],
				{ return FGameplayTagContainer(); }
			);

			Describe("when the attack is a success", [=, this]
			{
				REDEFINE_LET(
					AttackDegreeOfSuccess,
					EPF2DegreeOfSuccess,
					[],
					{ return EPF2DegreeOfSuccess::Success; }
				);

				It("moves the initiative position of the character directly before the turn in which they were reduced to 0 HP", [=, this]
				{
					TestArrayEquals(
						"GetAllCharactersInInitiativeOrder()",
						EncounterRuleSet->GetAllCharactersInInitiativeOrder(),
						{
							*OtherCharacter2,
							this->TestCharacter,
							*OtherCharacter1,
						}
					);
				});

				It("applies the 'Trait_Condition_Unconscious' gameplay tag to the character", [=, this]
				{
					TestTrue(
						TEXT("Has tag 'Trait.Condition.Unconscious'"),
						this->TestCharacterAsc->HasMatchingGameplayTag(
							FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
						)
					);
				});

				Describe("when the character does not have any 'Trait_Condition_Wounded' tags", [=, this]
				{
					It("applies the 'Trait_Condition_Dying_1' gameplay tag to the character", [=, this]
					{
						TestTrue(
							TEXT("Has tag 'Trait.Condition.Dying.1'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.1"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.2'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.2"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.3'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.3"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.4'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.4"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_1' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded1GeSpec],
						{ return TArray{ *Wounded1GeSpec }; }
					);

					It("applies the 'Trait_Condition_Dying_2' gameplay tag to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.1'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.1"))
							)
						);

						TestTrue(
							TEXT("Has tag 'Trait.Condition.Dying.2'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.2"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.3'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.3"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.4'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.4"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_2' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded2GeSpec],
						{ return TArray{ *Wounded2GeSpec }; }
					);

					It("applies the 'Trait_Condition_Dying_3' gameplay tag to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.1'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.1"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.2'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.2"))
							)
						);

						TestTrue(
							TEXT("Has tag 'Trait.Condition.Dying.3'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.3"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.4'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.4"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_3' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded3GeSpec],
						{ return TArray{ *Wounded3GeSpec }; }
					);

					It("kills the character", [=, this]
					{
						TestTrue(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_4' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded4GeSpec],
						{ return TArray{ *Wounded4GeSpec }; }
					);

					It("kills the character", [=, this]
					{
						TestTrue(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});
			});

			Describe("when the attack is a critical success", [=, this]
			{
				REDEFINE_LET(
					AttackDegreeOfSuccess,
					EPF2DegreeOfSuccess,
					[],
					{ return EPF2DegreeOfSuccess::CriticalSuccess; }
				);

				It("moves the initiative position of the character directly before the turn in which they were reduced to 0 HP", [=, this]
				{
					TestArrayEquals(
						"GetAllCharactersInInitiativeOrder()",
						EncounterRuleSet->GetAllCharactersInInitiativeOrder(),
						{
							*OtherCharacter2,
							this->TestCharacter,
							*OtherCharacter1,
						}
					);
				});

				It("applies the 'Trait_Condition_Unconscious' gameplay tag to the character", [=, this]
				{
					TestTrue(
						TEXT("Has tag 'Trait.Condition.Unconscious'"),
						this->TestCharacterAsc->HasMatchingGameplayTag(
							FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
						)
					);
				});

				Describe("when the character does not have any 'Trait_Condition_Wounded' tags", [=, this]
				{
					It("applies the 'Trait_Condition_Dying_2' gameplay tag to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.1'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.1"))
							)
						);

						TestTrue(
							TEXT("Has tag 'Trait.Condition.Dying.2'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.2"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.3'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.3"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.4'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.4"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_1' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded1GeSpec],
						{ return TArray{ *Wounded1GeSpec }; }
					);

					It("applies the 'Trait_Condition_Dying_3' gameplay tag to the character", [=, this]
					{
						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.1'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.1"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.2'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.2"))
							)
						);

						TestTrue(
							TEXT("Has tag 'Trait.Condition.Dying.3'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.3"))
							)
						);

						TestFalse(
							TEXT("Has tag 'Trait.Condition.Dying.4'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying.4"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_2' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded2GeSpec],
						{ return TArray{ *Wounded2GeSpec }; }
					);

					It("kills the character", [=, this]
					{
						TestTrue(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_3' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded3GeSpec],
						{ return TArray{ *Wounded3GeSpec }; }
					);

					It("kills the character", [=, this]
					{
						TestTrue(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});

				Describe("when the character has the 'Trait_Condition_Wounded_4' tag", [=, this]
				{
					REDEFINE_LET(
						EffectSpecsAppliedToTargetBeforeAttack,
						TArray<FGameplayEffectSpecHandle>,
						[Wounded4GeSpec],
						{ return TArray{ *Wounded4GeSpec }; }
					);

					It("kills the character", [=, this]
					{
						TestTrue(
							TEXT("Has tag 'Trait.Condition.Dead'"),
							this->TestCharacterAsc->HasMatchingGameplayTag(
								FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dead"))
							)
						);
					});
				});
			});
		});

		Describe("when the attack that knocks out the Character is nonlethal", [=, this]
		{
			REDEFINE_LET(
				TagsAppliedToAbilityBeforeAttack,
				FGameplayTagContainer,
				[this],
				{return FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Trait.Effect.Nonlethal")); }
			);

			It("moves the initiative position of the character directly before the turn in which they were reduced to 0 HP", [=, this]
			{
				TestArrayEquals(
					"GetAllCharactersInInitiativeOrder()",
					EncounterRuleSet->GetAllCharactersInInitiativeOrder(),
					{
						*OtherCharacter2,
						this->TestCharacter,
						*OtherCharacter1,
					}
				);
			});

			It("applies the 'Trait_Condition_Unconscious' gameplay tag to the character", [=, this]
			{
				TestTrue(
					TEXT("Has tag 'Trait.Condition.Unconscious'"),
					this->TestCharacterAsc->HasMatchingGameplayTag(
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Unconscious"))
					)
				);
			});

			It("does not apply any 'Trait_Condition_Dying' gameplay tag to the character", [=, this]
			{
				TestFalse(
					TEXT("Has tag 'Trait.Condition.Dying'"),
					this->TestCharacterAsc->HasMatchingGameplayTag(
						FGameplayTag::RequestGameplayTag(TEXT("Trait.Condition.Dying"))
					)
				);
			});
		});
	});
}

void FPF2ConditionsKnockoutAndDeath::ApplyDamageToTestCharacter(
	const TScriptInterface<IPF2CharacterInterface> Attacker,
	const FGameplayAbilitySpecHandle&              AttackAbilityHandle,
	const TSubclassOf<UGameplayEffect>             DamageEffect,
	const EPF2DegreeOfSuccess                      DegreeOfSuccess,
	const float                                    DamageAmount) const
{
	FGameplayEffectSpecHandle EffectSpecHandle;

	this->TestCharacterAttributeSet->TmpLastIncomingAttackDegreeOfSuccess =
		UPF2AttackStatLibrary::DegreeOfSuccessStatFromEnum(DegreeOfSuccess);

	const UAbilitySystemComponent* AttackerAsc   = Attacker->GetAbilitySystemComponent();
	const FGameplayAbilitySpec*    AbilitySpec   = AttackerAsc->FindAbilitySpecFromHandle(AttackAbilityHandle);
	const UGameplayAbility*        AttackAbility = AbilitySpec->Ability.Get();

	EffectSpecHandle =
		this->BuildEffectSpec(
			DamageEffect,
			Attacker,
			AttackAbility,
			{
				{"GameplayEffect.Parameter.Damage", DamageAmount},
			}
		);

	this->TestCharacterAsc->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
}

FActiveGameplayEffectHandle FPF2ConditionsKnockoutAndDeath::ApplyGameplayEffectToTestCharacter(
	const TSubclassOf<UGameplayEffect>& EffectType) const
{
	FGameplayEffectContext* GameplayEffectContext = new FGameplayEffectContext();

	return this->TestCharacterAsc->BP_ApplyGameplayEffectToSelf(
		EffectType,
		1.0,
		FGameplayEffectContextHandle(GameplayEffectContext)
	);
}
