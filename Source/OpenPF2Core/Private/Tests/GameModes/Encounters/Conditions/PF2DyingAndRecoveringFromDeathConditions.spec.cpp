// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "GameModes/Encounters/PF2EncounterModeOfPlayRuleSetBase.h"

#include "Tests/PF2SpecBase.h"

#include "Utilities/PF2InterfaceUtilities.h"

BEGIN_DEFINE_PF_SPEC(FPF2DyingAndRecoveringFromDeathConditions,
                     "OpenPF2.Conditions.DyingAndRecoveringFromDeath",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	APF2EncounterModeOfPlayRuleSetBase* EncounterRuleSet;

	UPF2CharacterAttributeSet* TestCharacterAttributeSet;

	TScriptInterface<IPF2CharacterInterface> OtherCharacter1,
	                                         OtherCharacter2;

	FGameplayEffectSpecHandle OtherCharacter1BleedGeSpec;
END_DEFINE_PF_SPEC(FPF2DyingAndRecoveringFromDeathConditions)

void FPF2DyingAndRecoveringFromDeathConditions::Define()
{
	static TSubclassOf<APF2EncounterModeOfPlayRuleSetBase> BP_Encounter_RuleSet =
		this->LoadBlueprint<APF2EncounterModeOfPlayRuleSetBase>(
			"/OpenPF2Core/OpenPF2/Optional/ModesOfPlay/Encounters",
			"BP_MoPRS_Encounter"
		);

	static TSubclassOf<UGameplayEffect> BP_BleedDamage_Effect =
		this->LoadBlueprint<UGameplayEffect>(
			"/OpenPF2Core/OpenPF2/Optional/DamageTypes",
			"GE_ApplyBleedDamage"
		);

	static TSubclassOf<UGameplayAbility> BP_ConditionCheck_Ability =
		this->LoadBlueprint<UGameplayAbility>(
			"/OpenPF2Core/OpenPF2/Core/Conditions",
			"GA_ConditionCheck"
		);

	BeforeEach([=, this]
	{
		FGameplayAbilitySpecHandle FakeAbilityHandle;

		this->SetupWorld();
		this->SetupTestCharacter();

		this->BeginPlay();

		this->EncounterRuleSet =
			Cast<APF2EncounterModeOfPlayRuleSetBase>(this->World->SpawnActor(BP_Encounter_RuleSet));

		this->TestCharacterAttributeSet =
			const_cast<UPF2CharacterAttributeSet*>(this->TestCharacterAsc->GetSet<UPF2CharacterAttributeSet>());

		this->OtherCharacter1 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
		this->OtherCharacter2 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());

		this->EncounterRuleSet->OnCharacterAddedToEncounter(this->TestCharacter);
		this->EncounterRuleSet->OnCharacterAddedToEncounter(OtherCharacter1);
		this->EncounterRuleSet->OnCharacterAddedToEncounter(OtherCharacter2);

		this->EncounterRuleSet->SetCharacterInitiative(this->TestCharacter, 15);
		this->EncounterRuleSet->SetCharacterInitiative(OtherCharacter1, 20);
		this->EncounterRuleSet->SetCharacterInitiative(OtherCharacter2, 22);

		this->GrantCharacterAbility(this->TestCharacter, BP_ConditionCheck_Ability);
		FakeAbilityHandle = this->GrantCharacterFakeAbility(OtherCharacter1);

		this->OtherCharacter1BleedGeSpec = this->BuildEffectSpecForInstigator(
			BP_BleedDamage_Effect,
			OtherCharacter1,
			FakeAbilityHandle,
			{
				{"GameplayEffect.Parameter.Damage", 20.0f},
			});

		// Sanity check initiative preconditions.
		TestArrayEquals(
			"GetAllCharactersInInitiativeOrder()",
			this->EncounterRuleSet->GetAllCharactersInInitiativeOrder(),
			{
				OtherCharacter2,
				OtherCharacter1,
				this->TestCharacter,
			}
		);

		// Advance to OtherCharacter1's turn.
		this->EncounterRuleSet->StartTurnForCharacter(OtherCharacter1);
	});

	AfterEach([=, this]
	{
		this->DestroyTestCharacter();
		this->DestroyWorld();
	});

	Describe("when the HP of a character is reduced to 0 HP", [=, this]
	{
		BeforeEach([=, this]
		{
			this->TestCharacterAsc->ApplyGameplayEffectSpecToSelf(*this->OtherCharacter1BleedGeSpec.Data);
		});

		It("the initiative position of the character is moved directly before the turn in which they were reduced to 0 HP", [=, this]
		{
			TestArrayEquals(
				"GetAllCharactersInInitiativeOrder()",
				this->EncounterRuleSet->GetAllCharactersInInitiativeOrder(),
				{
					OtherCharacter2,
					this->TestCharacter,
					OtherCharacter1,
				}
			);
		});

		xIt("the 'Trait_Condition_Unconscious' gameplay tag is applied to the character", [=, this]
		{

		});

		Describe("when the attack that knocks out the character is lethal", [=, this] {
			Describe("when the attack is not a critical success", [=, this]
			{
				Describe("when Character B does not have any 'Trait_Condition_Wounded' tags", [=, this]
				{
					// Character B gains the "Trait_Condition_Dying_1" tag
				});

				Describe("when the attack is not a critical success", [=, this]
				{
					// - when Character B has the 'Trait_Condition_Wounded_1' tag: Character B gains the 'Trait_Condition_Dying_2' tag
				});

				Describe("when the attack is not a critical success", [=, this]
				{
					// - when Character B has the 'Trait_Condition_Wounded_2' tag: Character B gains the 'Trait_Condition_Dying_3' tag
				});

				Describe("when the attack is not a critical success", [=, this]
				{
					// - when Character B has the 'Trait_Condition_Wounded_4' tag: Character B dies
				});
			});

			// - when the attack was a critical success:
			// 	 - when Character B does not have any 'Trait_Condition_Wounded' tags: Character B gains the 'Trait_Condition_Dying_2' tag
			// 	 - when Character B has the 'Trait_Condition_Wounded_1' tag: Character B gains the 'Trait_Condition_Dying_3' tag
			// 	 - when Character B has the 'Trait_Condition_Wounded_2' tag: Character B dies
			// 	 - when Character B has the 'Trait_Condition_Wounded_3' tag: Character B dies
			// 	 - when Character B has the 'Trait_Condition_Wounded_4' tag: Character B dies

		});

		Describe("when the attack that knocks out the Character is nonlethal", [=, this] {
		  // Character B does not gain any 'Trait_Condition_Dying' tags
	});

/**
 *



when that Character B has any 'Trait_Condition_Dying' tag:
  - when that Character B is starting a turn: Character B attempts a recovery check
	- when the recovery check is a Critical Success:
	  - when Character B has the 'Trait_Condition_Dying_1' tag:
		- Character B loses the 'Trait_Condition_Dying_1' tag
		- when Character B does not have any 'Trait_Condition_Wounded' tags: Character B gains the 'Trait_Condition_Wounded_1' tag
		- when Character B has the 'Trait_Condition_Wounded_1' tag: Character B gains the 'Trait_Condition_Wounded_2' tag
		- when Character B has the 'Trait_Condition_Wounded_2' tag: Character B gains the 'Trait_Condition_Wounded_3' tag
		- when Character B has the 'Trait_Condition_Wounded_3' tag: Character B gains the 'Trait_Condition_Wounded_4' tag
		- when Character B has the 'Trait_Condition_Wounded_4' tag: Character B retains the 'Trait_Condition_Wounded_4' tag

	  - when Character B has the 'Trait_Condition_Dying_2' tag:
		- Character B loses the 'Trait_Condition_Dying_2' tag
		- when Character B does not have any 'Trait_Condition_Wounded' tags: Character B gains the 'Trait_Condition_Wounded_1' tag
		- when Character B has the 'Trait_Condition_Wounded_1' tag: Character B gains the 'Trait_Condition_Wounded_2' tag
		- when Character B has the 'Trait_Condition_Wounded_2' tag: Character B gains the 'Trait_Condition_Wounded_3' tag
		- when Character B has the 'Trait_Condition_Wounded_3' tag: Character B gains the 'Trait_Condition_Wounded_4' tag
		- when Character B has the 'Trait_Condition_Wounded_4' tag: Character B retains the 'Trait_Condition_Wounded_4' tag

	  - when Character B has the 'Trait_Condition_Dying_3' tag:
		- Character B loses the 'Trait_Condition_Dying_3' tag
		- Character B gains the 'Trait_Condition_Dying_1' tag

	- when the recovery check is a Success:
	  - when Character B has the 'Trait_Condition_Dying_1' tag:
		- Character B loses the 'Trait_Condition_Dying_1' tag

		- when Character B does not have any 'Trait_Condition_Wounded' tags: Character B gains the 'Trait_Condition_Wounded_1' tag
		- when Character B has the 'Trait_Condition_Wounded_1' tag: Character B gains the 'Trait_Condition_Wounded_2' tag
		- when Character B has the 'Trait_Condition_Wounded_2' tag: Character B gains the 'Trait_Condition_Wounded_3' tag
		- when Character B has the 'Trait_Condition_Wounded_3' tag: Character B gains the 'Trait_Condition_Wounded_4' tag
		- when Character B has the 'Trait_Condition_Wounded_4' tag: Character B retains the 'Trait_Condition_Wounded_4' tag

	  - when Character B has the 'Trait_Condition_Dying_2' tag:
		- Character B loses the 'Trait_Condition_Dying_2' tag
		- Character B gains the 'Trait_Condition_Dying_1' tag

	  - when Character B has the 'Trait_Condition_Dying_3' tag:
		- Character B loses the 'Trait_Condition_Dying_3' tag
		- Character B gains the 'Trait_Condition_Dying_2' tag

	- when the recovery check is a Failure:
	  - when Character B has the 'Trait_Condition_Dying_1' tag:
		- Character B loses the 'Trait_Condition_Dying_1' tag
		- Character B gains the 'Trait_Condition_Dying_2' tag

	  - when Character B has the 'Trait_Condition_Dying_2' tag:
		- Character B loses the 'Trait_Condition_Dying_2' tag
		- Character B gains the 'Trait_Condition_Dying_3' tag

	  - when Character B has the 'Trait_Condition_Dying_3' tag:
		- Character B dies

	- when the recovery check is a Critical Failure:
	  - when Character B has the 'Trait_Condition_Dying_1' tag:
		- Character B loses the 'Trait_Condition_Dying_1' tag
		- Character B gains the 'Trait_Condition_Dying_3' tag

	  - when Character B has the 'Trait_Condition_Dying_2' tag:
		- Character B dies

	  - when Character B has the 'Trait_Condition_Dying_3' tag:
		- Character B dies
		*/
	});
}
