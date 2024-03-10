// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/Attacks/PF2AttackAttributeSet.h"

#include "ModesOfPlay/Encounter/PF2CharacterInitiativeQueueComponent.h"
#include "ModesOfPlay/Encounter/PF2CharacterInitiativeQueueInterface.h"

#include "Tests/PF2SpecBase.h"

#include "Utilities/PF2InterfaceUtilities.h"

BEGIN_DEFINE_PF_SPEC(FPF2CharacterInitiativeQueueComponentSpec,
                     "OpenPF2.CharacterInitiativeQueueComponent",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	IPF2CharacterInitiativeQueueInterface* Component;
END_DEFINE_PF_SPEC(FPF2CharacterInitiativeQueueComponentSpec)

void FPF2CharacterInitiativeQueueComponentSpec::Define()
{
	BeforeEach([=, this]
	{
		this->SetupWorld();
		this->SetupTestPawn();

		this->Component = SpawnActorComponent<UPF2CharacterInitiativeQueueComponent>();
	});

	AfterEach([=, this]
	{
		this->DestroyTestPawn();
		this->DestroyWorld();
	});

	Describe("IsEmpty", [this]
	{
		Describe("when the queue is empty", [this]
		{
			It("returns `true`", [this]
			{
				TestTrue("IsEmpty()", this->Component->IsEmpty());
			});
		});

		Describe("when the queue contains one item", [this]
		{
			BeforeEach([=, this]
			{
				IPF2CharacterInterface *Character = this->SpawnCharacter();

				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character), 1);
			});

			It("returns `false`", [this]
			{
				TestFalse("IsEmpty()", this->Component->IsEmpty());
			});
		});

		Describe("when the queue contains multiple items with the same initiative", [this]
		{
			BeforeEach([=, this]
			{
				IPF2CharacterInterface *Character1 = this->SpawnCharacter(),
				                       *Character2 = this->SpawnCharacter();

				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character1), 10);
				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character2), 10);
			});

			It("returns `false`", [this]
			{
				TestFalse("IsEmpty()", this->Component->IsEmpty());
			});
		});

		Describe("when the queue contains multiple items with different initiatives", [this]
		{
			BeforeEach([=, this]
			{
				IPF2CharacterInterface *Character1 = this->SpawnCharacter(),
				                       *Character2 = this->SpawnCharacter();

				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character1), 10);
				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character2), 20);
			});

			It("returns `false`", [this]
			{
				TestFalse("IsEmpty()", this->Component->IsEmpty());
			});
		});
	});

	Describe("GetCharacterInitiative", [this]
	{
		Describe("when the queue is empty", [this]
		{
			It("returns `INDEX_NONE` for all inputs", [this]
			{
				IPF2CharacterInterface *Character1 = this->SpawnCharacter(),
									   *Character2 = this->SpawnCharacter();

				TestEqual(
					"GetCharacterInitiative(Character1)",
					this->Component->GetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character1)),
					INDEX_NONE
				);

				TestEqual(
					"GetCharacterInitiative(Character2)",
					this->Component->GetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character2)),
					INDEX_NONE
				);
			});
		});

		Describe("when the queue contains multiple items with the same initiative", [this]
		{
			static IPF2CharacterInterface *Character1,
			                              *Character2,
			                              *Character3,
			                              *Character4;

			BeforeEach([=, this]
			{
				Character1 = this->SpawnCharacter();
				Character2 = this->SpawnCharacter();
				Character3 = this->SpawnCharacter();
				Character4 = this->SpawnCharacter();

				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character1), 10);
				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character2), 10);
				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character3), 20);
			});

			Describe("when initiative is fetched for characters in the queue", [this]
			{
				It("returns the expected initiative of each character", [this]
				{
					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character1)),
						10
					);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character2)),
						10
					);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character3)),
						20
					);
				});
			});

			Describe("when initiative is fetched for a character that is not in the queue", [this]
			{
				It("returns `INDEX_NONE`", [this]
				{
					TestEqual(
						"GetCharacterInitiative(Character4)",
						this->Component->GetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character4)),
						INDEX_NONE
					);
				});
			});
		});

		Describe("when the queue contains multiple items with different initiatives", [this]
		{
			static IPF2CharacterInterface *Character1,
			                              *Character2,
			                              *Character3,
			                              *Character4;

			BeforeEach([=, this]
			{
				Character1 = this->SpawnCharacter();
				Character2 = this->SpawnCharacter();
				Character3 = this->SpawnCharacter();
				Character4 = this->SpawnCharacter();

				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character1), 10);
				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character2), 20);
				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character3), 30);
			});

			Describe("when initiative is fetched for characters in the queue", [this]
			{
				It("returns the expected initiative of each character", [this]
				{
					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character1)),
						10
					);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character2)),
						20
					);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character3)),
						30
					);
				});
			});

			Describe("when initiative is fetched for a character that is not in the queue", [this]
			{
				It("returns `INDEX_NONE`", [this]
				{
					TestEqual(
						"GetCharacterInitiative(Character4)",
						this->Component->GetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character4)),
						INDEX_NONE
					);
				});
			});
		});
	});

	Describe("SetCharacterInitiative", [this]
	{
		Describe("when given a character that did not have an initiative set", [this]
		{
			Describe("when given an initiative equal to -1", [this]
			{
				It("makes no changes to initiative for the character", [this]
				{
					IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

					const TScriptInterface<IPF2CharacterInterface> Character =
						PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

					AddExpectedError(
						TEXT("\\[SA SRV\\] Initiative for character \\('Character\\[PF2TestCharacter_0\\]'\\) must be greater than 0; attempted to set it to '-1'\\."),
						EAutomationExpectedErrorFlags::Exact,
						1
					);

					this->Component->SetCharacterInitiative(Character, -1);

					TestEqual(
						"GetCharacterInitiative(Character)",
						this->Component->GetCharacterInitiative(Character),
						INDEX_NONE
					);

					TestEqual(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{}
					);
				});
			});

			Describe("when given an initiative equal to 0", [this]
			{
				It("makes no changes to initiative for the character", [this]
				{
					IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

					const TScriptInterface<IPF2CharacterInterface> Character =
						PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

					AddExpectedError(
						TEXT("\\[SA SRV\\] Initiative for character \\('Character\\[PF2TestCharacter_0\\]'\\) must be greater than 0; attempted to set it to '0'\\."),
						EAutomationExpectedErrorFlags::Exact,
						1
					);

					this->Component->SetCharacterInitiative(Character, 0);

					TestEqual(
						"GetCharacterInitiative(Character)",
						this->Component->GetCharacterInitiative(Character),
						INDEX_NONE
					);

					TestEqual(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{}
					);
				});
			});

			Describe("when given an initiative greater than 0", [this]
			{
				It("sets initiative for the character", [this]
				{
					IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

					const TScriptInterface<IPF2CharacterInterface> Character =
						PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

					this->Component->SetCharacterInitiative(Character, 25);

					TestEqual(
						"GetCharacterInitiative(Character)",
						this->Component->GetCharacterInitiative(Character),
						25
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{Character}
					);
				});
			});
		});

		Describe("when given a character that already has an initiative set", [this]
		{
			Describe("when given an initiative equal to -1", [this]
			{
				It("makes no changes to initiative for the character", [this]
				{
					IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

					const TScriptInterface<IPF2CharacterInterface> Character =
						PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

					this->Component->SetCharacterInitiative(Character, 25);

					AddExpectedError(
						TEXT("\\[SA SRV\\] Initiative for character \\('Character\\[PF2TestCharacter_0\\]'\\) must be greater than 0; attempted to set it to '-1'\\."),
						EAutomationExpectedErrorFlags::Exact,
						1
					);

					this->Component->SetCharacterInitiative(Character, -1);

					TestEqual(
						"GetCharacterInitiative(Character)",
						this->Component->GetCharacterInitiative(Character),
						25
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{Character}
					);
				});
			});

			Describe("when given an initiative equal to 0", [this]
			{
				It("makes no changes to initiative for the character", [this]
				{
					IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

					const TScriptInterface<IPF2CharacterInterface> Character =
						PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

					this->Component->SetCharacterInitiative(Character, 25);

					AddExpectedError(
						TEXT("\\[SA SRV\\] Initiative for character \\('Character\\[PF2TestCharacter_0\\]'\\) must be greater than 0; attempted to set it to '0'\\."),
						EAutomationExpectedErrorFlags::Exact,
						1
					);

					this->Component->SetCharacterInitiative(Character, 0);

					TestEqual(
						"GetCharacterInitiative(Character)",
						this->Component->GetCharacterInitiative(Character),
						25
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{Character}
					);
				});
			});

			Describe("when given an initiative greater than 0", [this]
			{
				Describe("when given a different initiative than the one the character already has", [this]
				{
					It("changes initiative for the character", [this]
					{
						IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

						const TScriptInterface<IPF2CharacterInterface> Character =
							PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

						this->Component->SetCharacterInitiative(Character, 25);
						this->Component->SetCharacterInitiative(Character, 35);

						TestEqual(
							"GetCharacterInitiative(Character)",
							this->Component->GetCharacterInitiative(Character),
							35
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{Character}
						);
					});
				});

				Describe("when given the same initiative as the character already has", [this]
				{
					It("makes no changes to initiative for the character", [this]
					{
						IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

						const TScriptInterface<IPF2CharacterInterface> Character =
							PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

						this->Component->SetCharacterInitiative(Character, 25);
						this->Component->SetCharacterInitiative(Character, 25);

						TestEqual(
							"GetCharacterInitiative(Character)",
							this->Component->GetCharacterInitiative(Character),
							25
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{Character}
						);
					});
				});
			});
		});
	});

	Describe("IsInitiativeSetForCharacter", [this]
	{
		Describe("when the queue is empty", [this]
		{
			It("returns `false` for all inputs", [this]
			{
				IPF2CharacterInterface *Character1 = this->SpawnCharacter(),
									   *Character2 = this->SpawnCharacter();

				TestFalse(
					"IsInitiativeSetForCharacter(Character1)",
					this->Component->IsInitiativeSetForCharacter(PF2InterfaceUtilities::ToScriptInterface(Character1))
				);

				TestFalse(
					"IsInitiativeSetForCharacter(Character2)",
					this->Component->IsInitiativeSetForCharacter(PF2InterfaceUtilities::ToScriptInterface(Character2))
				);
			});
		});

		Describe("when the queue contains multiple items with the same initiative", [this]
		{
			static IPF2CharacterInterface *Character1,
			                              *Character2,
			                              *Character3,
			                              *Character4;

			BeforeEach([=, this]
			{
				Character1 = this->SpawnCharacter();
				Character2 = this->SpawnCharacter();
				Character3 = this->SpawnCharacter();
				Character4 = this->SpawnCharacter();

				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character1), 10);
				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character2), 10);
				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character3), 20);
			});

			Describe("when initiative is fetched for characters in the queue", [this]
			{
				It("returns `true`", [this]
				{
					TestTrue(
						"IsInitiativeSetForCharacter(Character1)",
						this->Component->IsInitiativeSetForCharacter(
							PF2InterfaceUtilities::ToScriptInterface(Character1)
						)
					);

					TestTrue(
						"IsInitiativeSetForCharacter(Character2)",
						this->Component->IsInitiativeSetForCharacter(
							PF2InterfaceUtilities::ToScriptInterface(Character2)
						)
					);

					TestTrue(
						"IsInitiativeSetForCharacter(Character3)",
						this->Component->IsInitiativeSetForCharacter(
							PF2InterfaceUtilities::ToScriptInterface(Character3)
						)
					);
				});
			});

			Describe("when initiative is fetched for a character that is not in the queue", [this]
			{
				It("returns `false`", [this]
				{
					TestFalse(
						"IsInitiativeSetForCharacter(Character4)",
						this->Component->IsInitiativeSetForCharacter(
							PF2InterfaceUtilities::ToScriptInterface(Character4)
						)
					);
				});
			});
		});

		Describe("when the queue contains multiple items with different initiatives", [this]
		{
			static IPF2CharacterInterface *Character1,
			                              *Character2,
			                              *Character3,
			                              *Character4;

			BeforeEach([=, this]
			{
				Character1 = this->SpawnCharacter();
				Character2 = this->SpawnCharacter();
				Character3 = this->SpawnCharacter();
				Character4 = this->SpawnCharacter();

				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character1), 10);
				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character2), 20);
				this->Component->SetCharacterInitiative(PF2InterfaceUtilities::ToScriptInterface(Character3), 30);
			});

			Describe("when initiative is fetched for characters in the queue", [this]
			{
				It("returns `true`", [this]
				{
					TestTrue(
						"IsInitiativeSetForCharacter(Character1)",
						this->Component->IsInitiativeSetForCharacter(
							PF2InterfaceUtilities::ToScriptInterface(Character1)
						)
					);

					TestTrue(
						"IsInitiativeSetForCharacter(Character2)",
						this->Component->IsInitiativeSetForCharacter(
							PF2InterfaceUtilities::ToScriptInterface(Character2)
						)
					);

					TestTrue(
						"IsInitiativeSetForCharacter(Character3)",
						this->Component->IsInitiativeSetForCharacter(
							PF2InterfaceUtilities::ToScriptInterface(Character3)
						)
					);
				});
			});

			Describe("when initiative is fetched for a character that is not in the queue", [this]
			{
				It("returns `false`", [this]
				{
					TestFalse(
						"IsInitiativeSetForCharacter(Character4)",
						this->Component->IsInitiativeSetForCharacter(
							PF2InterfaceUtilities::ToScriptInterface(Character4)
						)
					);
				});
			});
		});
	});

	Describe("InsertCharacterAtOrAboveInitiative", [this]
	{
		Describe("when given an initiative of -1", [this]
		{
			It("makes no changes to initiative for the character", [this]
			{
				IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

				const TScriptInterface<IPF2CharacterInterface> Character =
					PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

				this->Component->SetCharacterInitiative(Character, 25);

				AddExpectedError(
					TEXT("\\[SA SRV\\] Initiative for character \\('Character\\[PF2TestCharacter_0\\]'\\) must be greater than 0; attempted to shift it to '-1'\\."),
					EAutomationExpectedErrorFlags::Exact,
					1
				);

				this->Component->InsertCharacterAtOrAboveInitiative(Character, -1);

				TestEqual(
					"GetCharacterInitiative(Character)",
					this->Component->GetCharacterInitiative(Character),
					25
				);

				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{Character}
				);
			});
		});

		Describe("when given an initiative equal to 0", [this]
		{
			It("makes no changes to initiative for the character", [this]
			{
				IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

				const TScriptInterface<IPF2CharacterInterface> Character =
					PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

				this->Component->SetCharacterInitiative(Character, 25);

				AddExpectedError(
					TEXT("\\[SA SRV\\] Initiative for character \\('Character\\[PF2TestCharacter_0\\]'\\) must be greater than 0; attempted to shift it to '0'\\."),
					EAutomationExpectedErrorFlags::Exact,
					1
				);

				this->Component->InsertCharacterAtOrAboveInitiative(Character, 0);

				TestEqual(
					"GetCharacterInitiative(Character)",
					this->Component->GetCharacterInitiative(Character),
					25
				);

				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{Character}
				);
			});
		});

		Describe("when given an initiative greater than 0", [this]
		{
			static TScriptInterface<IPF2CharacterInterface> Character1,
			                                                Character2,
			                                                Character3,
			                                                Character4;

			BeforeEach([=, this]
			{
				Character1 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character2 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character3 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character4 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());

				this->Component->SetCharacterInitiative(Character1, 1);
				this->Component->SetCharacterInitiative(Character2, 2);
				this->Component->SetCharacterInitiative(Character3, 3);
			});

			Describe("when the target character already has the specified initiative score", [this]
			{
				It("makes no changes to initiative for the character", [this]
				{
					// Before:
					// - Character3 - 3
					// - Character2 - 2
					// - Character1 - 1
					// After:
					// - Character3 - 3
					// - Character2 - 2
					// - Character1 - 1
					this->Component->InsertCharacterAtOrAboveInitiative(Character2, 2);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(Character3),
						3
					);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(Character2),
						2
					);

					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(Character1),
						1
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{
							Character3,
							Character2,
							Character1,
						}
					);
				});
			});

			Describe("when no other character has the specified initiative score", [this]
			{
				It("adjusts initiative for the character to the target initiative score", [this]
				{
					// Before:
					// - Character3 - 3
					// - Character2 - 2
					// - Character1 - 1
					// After:
					// - Character2 - 10
					// - Character3 - 3
					// - Character1 - 1
					this->Component->InsertCharacterAtOrAboveInitiative(Character2, 10);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(Character2),
						10
					);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(Character3),
						3
					);

					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(Character1),
						1
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{
							Character2,
							Character3,
							Character1,
						}
					);
				});
			});

			Describe("when one other character has the specified initiative score", [this]
			{
				Describe("when the incremented initiative score has no conflicts", [this]
				{
					It("adjusts initiative for the character to one more than the given initiative score", [this]
					{
						// Before:
						// - Character3 - 3
						// - Character2 - 2
						// - Character1 - 1
						// After:
						// - Character2 - 4
						// - Character3 - 3
						// - Character1 - 1
						this->Component->InsertCharacterAtOrAboveInitiative(Character2, 3);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							4
						);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							3
						);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							1
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character2,
								Character3,
								Character1,
							}
						);
					});
				});

				Describe("when the incremented initiative score has a conflict", [this]
				{
					It("scales all existing initiative scores and the target initiative score by 10 and then adds one to the scaled initiative score", [this]
					{
						// Before:
						// - Character3 - 3
						// - Character2 - 2
						// - Character1 - 1
						// After:
						// - Character3 - 21
						// - Character2 - 20
						// - Character1 - 10
						this->Component->InsertCharacterAtOrAboveInitiative(Character3, 2);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							21
						);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							20
						);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							10
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character3,
								Character2,
								Character1,
							}
						);
					});
				});

				Describe("when the incremented initiative score has a sequence of conflicts", [this]
				{
					It("scales all existing initiative scores and the target initiative score by 10 and then adds one to the scaled initiative score", [this]
					{
						// Before:
						// - Character3 - 3
						// - Character2 - 2
						// - Character1 - 1
						// After:
						// - Character2 - 20
						// - Character3 - 11
						// - Character1 - 10
						this->Component->InsertCharacterAtOrAboveInitiative(Character3, 1);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							20
						);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							11
						);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							10
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character2,
								Character3,
								Character1,
							}
						);
					});
				});
			});

			Describe("when multiple other characters have the specified initiative score", [this]
			{
				Describe("when the incremented initiative score has no conflicts", [this]
				{
					It("adjusts initiative for the character to one more than the given initiative score", [this]
					{
						// Before:
						// - Character3 - 3
						// - Character4 - 3
						// - Character2 - 2
						// - Character1 - 1
						// After:
						// - Character1 - 4
						// - Character3 - 3
						// - Character4 - 3
						// - Character2 - 2
						this->Component->SetCharacterInitiative(Character4, 3);
						this->Component->InsertCharacterAtOrAboveInitiative(Character1, 3);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							4
						);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							3
						);

						TestEqual(
							"GetCharacterInitiative(Character4)",
							this->Component->GetCharacterInitiative(Character4),
							3
						);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							2
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character1,
								Character3,
								Character4,
								Character2,
							}
						);
					});
				});

				Describe("when the incremented initiative score has a conflict", [this]
				{
					It("scales all existing initiative scores and the target initiative score by 10 and then adds one to the scaled initiative score", [this]
					{
						// Before:
						// - Character3 - 3
						// - Character2 - 2
						// - Character4 - 2
						// - Character1 - 1
						// After:
						// - Character3 - 30
						// - Character1 - 21
						// - Character2 - 20
						// - Character4 - 20
						this->Component->SetCharacterInitiative(Character4, 2);
						this->Component->InsertCharacterAtOrAboveInitiative(Character1, 2);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							30
							);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							21
						);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							20
						);

						TestEqual(
							"GetCharacterInitiative(Character4)",
							this->Component->GetCharacterInitiative(Character4),
							20
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character3,
								Character1,
								Character2,
								Character4,
							}
						);
					});
				});

				Describe("when the incremented initiative score has a sequence of conflicts", [this]
				{
					It("scales all existing initiative scores and the target initiative score by 10 and then adds one to the scaled initiative score", [this]
					{
						// Before:
						// - Character3 - 3
						// - Character2 - 2
						// - Character1 - 1
						// - Character4 - 1
						// After:
						// - Character2 - 20
						// - Character3 - 11
						// - Character1 - 10
						// - Character4 - 10
						this->Component->SetCharacterInitiative(Character4, 1);
						this->Component->InsertCharacterAtOrAboveInitiative(Character3, 1);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							20
						);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							11
						);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							10
						);

						TestEqual(
							"GetCharacterInitiative(Character4)",
							this->Component->GetCharacterInitiative(Character4),
							10
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character2,
								Character3,
								Character1,
								Character4,
							}
						);
					});
				});
			});
		});
	});

	Describe("InsertCharacterAtOrBelowInitiative", [this]
	{
		Describe("when given an initiative of -1", [this]
		{
			It("makes no changes to initiative for the character", [this]
			{
				IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

				const TScriptInterface<IPF2CharacterInterface> Character =
					PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

				this->Component->SetCharacterInitiative(Character, 25);

				AddExpectedError(
					TEXT("\\[SA SRV\\] Initiative for character \\('Character\\[PF2TestCharacter_0\\]'\\) must be greater than 0; attempted to shift it to '-1'\\."),
					EAutomationExpectedErrorFlags::Exact,
					1
				);

				this->Component->InsertCharacterAtOrBelowInitiative(Character, -1);

				TestEqual(
					"GetCharacterInitiative(Character)",
					this->Component->GetCharacterInitiative(Character),
					25
				);

				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{Character}
				);
			});
		});

		Describe("when given an initiative equal to 0", [this]
		{
			It("makes no changes to initiative for the character", [this]
			{
				IPF2CharacterInterface* CharacterIntf = this->SpawnCharacter();

				const TScriptInterface<IPF2CharacterInterface> Character =
					PF2InterfaceUtilities::ToScriptInterface(CharacterIntf);

				this->Component->SetCharacterInitiative(Character, 25);

				AddExpectedError(
					TEXT("\\[SA SRV\\] Initiative for character \\('Character\\[PF2TestCharacter_0\\]'\\) must be greater than 0; attempted to shift it to '0'\\."),
					EAutomationExpectedErrorFlags::Exact,
					1
				);

				this->Component->InsertCharacterAtOrBelowInitiative(Character, 0);

				TestEqual(
					"GetCharacterInitiative(Character)",
					this->Component->GetCharacterInitiative(Character),
					25
				);

				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{Character}
				);
			});
		});

		Describe("when given an initiative equal to 1", [this]
		{
			static TScriptInterface<IPF2CharacterInterface> Character1,
			                                                Character2,
			                                                Character3,
			                                                Character4;

			BeforeEach([=, this]
			{
				Character1 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character2 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character3 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character4 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
			});

			Describe("when the target character already has the specified initiative score", [this]
			{
				It("makes no changes to initiative for the character", [this]
				{
					// Before:
					// - Character3 - 3
					// - Character2 - 2
					// - Character1 - 1
					// After:
					// - Character3 - 3
					// - Character2 - 2
					// - Character1 - 1
					this->Component->SetCharacterInitiative(Character1, 1);
					this->Component->SetCharacterInitiative(Character2, 2);
					this->Component->SetCharacterInitiative(Character3, 3);

					this->Component->InsertCharacterAtOrBelowInitiative(Character1, 1);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(Character3),
						3
					);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(Character2),
						2
					);

					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(Character1),
						1
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{
							Character3,
							Character2,
							Character1,
						}
					);
				});
			});

			Describe("when no other character has the specified initiative score", [this]
			{
				It("adjusts initiative for the character to the target initiative score", [this]
				{
					// Before:
					// - Character3 - 4
					// - Character2 - 3
					// - Character1 - 2
					// After:
					// - Character2 - 3
					// - Character1 - 2
					// - Character3 - 1
					this->Component->SetCharacterInitiative(Character1, 2);
					this->Component->SetCharacterInitiative(Character2, 3);
					this->Component->SetCharacterInitiative(Character3, 4);

					this->Component->InsertCharacterAtOrBelowInitiative(Character3, 1);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(Character2),
						3
					);

					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(Character1),
						2
					);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(Character3),
						1
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{
							Character2,
							Character1,
							Character3,
						}
					);
				});
			});

			Describe("when one other character has the specified initiative score", [this]
			{
				It("avoids an initiative score of 0 by scaling all existing initiative scores and the target initiative score by 10 and then subtracting one from the scaled initiative score", [this]
				{
					// Before:
					// - Character3 - 3
					// - Character2 - 2
					// - Character1 - 1
					// After:
					// - Character3 - 30
					// - Character2 - 20
					// - Character1 - 10
					// - Character4 - 9
					this->Component->SetCharacterInitiative(Character1, 1);
					this->Component->SetCharacterInitiative(Character2, 2);
					this->Component->SetCharacterInitiative(Character3, 3);

					this->Component->InsertCharacterAtOrBelowInitiative(Character4, 1);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(Character3),
						30
					);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(Character2),
						20
					);

					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(Character1),
						10
					);

					TestEqual(
						"GetCharacterInitiative(Character4)",
						this->Component->GetCharacterInitiative(Character4),
						9
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{
							Character3,
							Character2,
							Character1,
							Character4,
						}
					);
				});
			});

			Describe("when multiple other characters have the specified initiative score", [this]
			{
				It("avoids an initiative score of 0 by scaling all existing initiative scores and the target initiative score by 10 and then subtracting one from the scaled initiative score", [this]
				{
					// Before:
					// - Character4 - 3
					// - Character3 - 2
					// - Character1 - 1
					// - Character2 - 1
					// After:
					// - Character3 - 20
					// - Character1 - 10
					// - Character2 - 10
					// - Character4 - 9
					this->Component->SetCharacterInitiative(Character1, 1);
					this->Component->SetCharacterInitiative(Character2, 1);
					this->Component->SetCharacterInitiative(Character3, 2);
					this->Component->SetCharacterInitiative(Character4, 3);

					this->Component->InsertCharacterAtOrBelowInitiative(Character4, 1);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(Character3),
						20
					);

					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(Character1),
						10
					);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(Character2),
						10
					);

					TestEqual(
						"GetCharacterInitiative(Character4)",
						this->Component->GetCharacterInitiative(Character4),
						9
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{
							Character3,
							Character1,
							Character2,
							Character4,
						}
					);
				});
			});

			Describe("when multiple characters are being inserted at or below 1", [this]
			{
				It("for every character, it scales all existing initiative scores and the target initiative score by 10 and then subtracts one", [this]
				{
					// Before:
					// - Character1 - 1
					// After:
					// - Character1 - 100
					// - Character4 - 90
					// - Character2 - 10
					// - Character3 - 9
					this->Component->SetCharacterInitiative(Character1, 1);

					this->Component->InsertCharacterAtOrBelowInitiative(Character4, 1);
					this->Component->InsertCharacterAtOrBelowInitiative(Character2, 1);
					this->Component->InsertCharacterAtOrBelowInitiative(Character3, 1);

					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(Character1),
						100
					);

					TestEqual(
						"GetCharacterInitiative(Character4)",
						this->Component->GetCharacterInitiative(Character4),
						90
					);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(Character2),
						10
					);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(Character3),
						9
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{
							Character1,
							Character4,
							Character2,
							Character3,
						}
					);
				});
			});
		});

		Describe("when given an initiative greater than 1", [this]
		{
			static TScriptInterface<IPF2CharacterInterface> Character1,
			                                                Character2,
			                                                Character3,
															Character4,
															Character5;

			BeforeEach([=, this]
			{
				Character1 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character2 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character3 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character4 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character5 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());

				this->Component->SetCharacterInitiative(Character1, 1);
				this->Component->SetCharacterInitiative(Character2, 3);
				this->Component->SetCharacterInitiative(Character3, 4);
			});

			Describe("when the target character already has the specified initiative score", [this]
			{
				It("makes no changes to initiative for the character", [this]
				{
					// Before:
					// - Character3 - 4
					// - Character2 - 3
					// - Character1 - 1
					// After:
					// - Character3 - 4
					// - Character2 - 3
					// - Character1 - 1
					this->Component->InsertCharacterAtOrBelowInitiative(Character2, 3);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(Character3),
						4
					);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(Character2),
						3
					);

					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(Character1),
						1
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{
							Character3,
							Character2,
							Character1,
						}
					);
				});
			});

			Describe("when no other character has the specified initiative score", [this]
			{
				It("adjusts initiative for the character to the target initiative score", [this]
				{
					// Before:
					// - Character3 - 4
					// - Character2 - 3
					// - Character1 - 1
					// After:
					// - Character2 - 10
					// - Character3 - 4
					// - Character1 - 1
					this->Component->InsertCharacterAtOrBelowInitiative(Character2, 10);

					TestEqual(
						"GetCharacterInitiative(Character2)",
						this->Component->GetCharacterInitiative(Character2),
						10
					);

					TestEqual(
						"GetCharacterInitiative(Character3)",
						this->Component->GetCharacterInitiative(Character3),
						4
					);

					TestEqual(
						"GetCharacterInitiative(Character1)",
						this->Component->GetCharacterInitiative(Character1),
						1
					);

					TestArrayEquals(
						"GetCharactersInInitiativeOrder()",
						this->Component->GetCharactersInInitiativeOrder(),
						{
							Character2,
							Character3,
							Character1,
						}
					);
				});
			});

			Describe("when one other character has the specified initiative score", [this]
			{
				Describe("when the decremented initiative score has no conflicts", [this]
				{
					It("adjusts initiative for the character to one value lower than the given initiative score", [this]
					{
						// Before:
						// - Character3 - 4
						// - Character2 - 3
						// - Character1 - 1
						// After:
						// - Character3 - 4
						// - Character2 - 3
						// - Character4 - 2
						// - Character1 - 1
						this->Component->InsertCharacterAtOrBelowInitiative(Character4, 3);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							4
						);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							3
						);

						TestEqual(
							"GetCharacterInitiative(Character4)",
							this->Component->GetCharacterInitiative(Character4),
							2
						);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							1
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character3,
								Character2,
								Character4,
								Character1,
							}
						);
					});
				});

				Describe("when the decremented initiative score has a conflict", [this]
				{
					It("scales all existing initiative scores and the target initiative score by 10 and then subtracts one from the scaled initiative score", [this]
					{
						// Before:
						// - Character3 - 4
						// - Character2 - 3
						// - Character1 - 1
						// After:
						// - Character3 - 40
						// - Character1 - 39
						// - Character2 - 30
						this->Component->InsertCharacterAtOrBelowInitiative(Character1, 4);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							40
						);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							39
						);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							30
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character3,
								Character1,
								Character2,
							}
						);
					});
				});

				Describe("when the decremented initiative score has a sequence of conflicts", [this]
				{
					It("scales all existing initiative scores and the target initiative score by 10 and then subtracts one to the scaled initiative score", [this]
					{
						// Before:
						// - Character3 - 4
						// - Character2 - 3
						// - Character4 - 2
						// - Character1 - 1
						// After:
						// - Character3 - 40
						// - Character1 - 39
						// - Character2 - 30
						// - Character4 - 20
						this->Component->SetCharacterInitiative(Character4, 2);

						this->Component->InsertCharacterAtOrBelowInitiative(Character1, 4);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							40
						);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							39
						);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							30
						);

						TestEqual(
							"GetCharacterInitiative(Character4)",
							this->Component->GetCharacterInitiative(Character4),
							20
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character3,
								Character1,
								Character2,
								Character4,
							}
						);
					});
				});
			});

			Describe("when multiple other characters have the specified initiative score", [this]
			{
				Describe("when the decremented initiative score has no conflicts", [this]
				{
					It("adjusts initiative for the character to one less than the given initiative score", [this]
					{
						// Before:
						// - Character3 - 4
						// - Character2 - 3
						// - Character4 - 3
						// - Character1 - 1
						// After:
						// - Character2 - 3
						// - Character4 - 3
						// - Character3 - 2
						// - Character1 - 1
						this->Component->SetCharacterInitiative(Character4, 3);
						this->Component->InsertCharacterAtOrBelowInitiative(Character3, 3);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							3
						);

						TestEqual(
							"GetCharacterInitiative(Character4)",
							this->Component->GetCharacterInitiative(Character4),
							3
						);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							2
						);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							1
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character2,
								Character4,
								Character3,
								Character1,
							}
						);
					});
				});

				Describe("when the decremented initiative score has a conflict", [this]
				{
					It("scales all existing initiative scores and the target initiative score by 10 and then subtracts one from the scaled initiative score", [this]
					{
						// Before:
						// - Character3 - 4
						// - Character2 - 2
						// - Character4 - 2
						// - Character1 - 1
						// After:
						// - Character2 - 20
						// - Character4 - 20
						// - Character3 - 19
						// - Character1 - 10
						this->Component->SetCharacterInitiative(Character2, 2);
						this->Component->SetCharacterInitiative(Character4, 2);

						this->Component->InsertCharacterAtOrBelowInitiative(Character3, 2);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							20
						);

						TestEqual(
							"GetCharacterInitiative(Character4)",
							this->Component->GetCharacterInitiative(Character4),
							20
						);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							19
						);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							10
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character2,
								Character4,
								Character3,
								Character1,
							}
						);
					});
				});

				Describe("when the decremented initiative score has a sequence of conflicts", [this]
				{
					It("scales all existing initiative scores and the target initiative score by 10 and then adds one to the scaled initiative score", [this]
					{
						// Before:
						// - Character3 - 3
						// - Character4 - 3
						// - Character2 - 2
						// - Character1 - 1
						// After:
						// - Character3 - 30
						// - Character4 - 30
						// - Character5 - 29
						// - Character2 - 20
						// - Character1 - 10
						this->Component->SetCharacterInitiative(Character1, 1);
						this->Component->SetCharacterInitiative(Character2, 2);
						this->Component->SetCharacterInitiative(Character3, 3);
						this->Component->SetCharacterInitiative(Character4, 3);

						this->Component->InsertCharacterAtOrBelowInitiative(Character5, 3);

						TestEqual(
							"GetCharacterInitiative(Character3)",
							this->Component->GetCharacterInitiative(Character3),
							30
						);

						TestEqual(
							"GetCharacterInitiative(Character4)",
							this->Component->GetCharacterInitiative(Character4),
							30
						);

						TestEqual(
							"GetCharacterInitiative(Character5)",
							this->Component->GetCharacterInitiative(Character5),
							29
						);

						TestEqual(
							"GetCharacterInitiative(Character2)",
							this->Component->GetCharacterInitiative(Character2),
							20
						);

						TestEqual(
							"GetCharacterInitiative(Character1)",
							this->Component->GetCharacterInitiative(Character1),
							10
						);

						TestArrayEquals(
							"GetCharactersInInitiativeOrder()",
							this->Component->GetCharactersInInitiativeOrder(),
							{
								Character3,
								Character4,
								Character5,
								Character2,
								Character1,
							}
						);
					});
				});
			});
		});
	});

	Describe("ClearInitiativeForCharacter", [this]
	{
		static TScriptInterface<IPF2CharacterInterface> Character1,
		                                                Character2,
		                                                Character3,
		                                                Character4;

		BeforeEach([=, this]
		{
			Character1 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
			Character2 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
			Character3 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
			Character4 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());

			this->Component->SetCharacterInitiative(Character1, 1);
			this->Component->SetCharacterInitiative(Character2, 2);
			this->Component->SetCharacterInitiative(Character3, 3);
		});

		Describe("when given a character that does not have an initiative set", [this]
		{
			It("has no effect on the initiatives of other characters", [this]
			{
				this->Component->ClearInitiativeForCharacter(Character4);

				TestEqual(
					"GetCharacterInitiative(Character1)",
					this->Component->GetCharacterInitiative(Character1),
					1
				);

				TestEqual(
					"GetCharacterInitiative(Character2)",
					this->Component->GetCharacterInitiative(Character2),
					2
				);

				TestEqual(
					"GetCharacterInitiative(Character3)",
					this->Component->GetCharacterInitiative(Character3),
					3
				);

				TestEqual(
					"GetCharacterInitiative(Character4)",
					this->Component->GetCharacterInitiative(Character4),
					INDEX_NONE
				);

				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{
						Character3,
						Character2,
						Character1,
					}
				);
			});
		});

		Describe("when given a character has an initiative set", [this]
		{
			It("clears only the initiative for that character", [this]
			{
				this->Component->ClearInitiativeForCharacter(Character2);

				TestEqual(
					"GetCharacterInitiative(Character3)",
					this->Component->GetCharacterInitiative(Character3),
					3
				);

				TestEqual(
					"GetCharacterInitiative(Character2)",
					this->Component->GetCharacterInitiative(Character2),
					INDEX_NONE
				);

				TestEqual(
					"GetCharacterInitiative(Character1)",
					this->Component->GetCharacterInitiative(Character1),
					1
				);

				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{
						Character3,
						Character1,
					}
				);
			});
		});
	});

	Describe("ClearInitiativeForAllCharacters", [this]
	{
		static TScriptInterface<IPF2CharacterInterface> Character1,
		                                                Character2,
		                                                Character3,
		                                                Character4;

		BeforeEach([=, this]
		{
			Character1 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
			Character2 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
			Character3 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
			Character4 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
		});

		Describe("when the queue is empty", [this]
		{
			It("has no effect on initiative for all characters", [this]
			{
				this->Component->ClearInitiativeForAllCharacters();

				TestEqual(
					"GetCharacterInitiative(Character1)",
					this->Component->GetCharacterInitiative(Character1),
					INDEX_NONE
				);

				TestEqual(
					"GetCharacterInitiative(Character2)",
					this->Component->GetCharacterInitiative(Character2),
					INDEX_NONE
				);

				TestEqual(
					"GetCharacterInitiative(Character3)",
					this->Component->GetCharacterInitiative(Character3),
					INDEX_NONE
				);

				TestEqual(
					"GetCharacterInitiative(Character4)",
					this->Component->GetCharacterInitiative(Character4),
					INDEX_NONE
				);

				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{}
				);
			});
		});

		Describe("when characters have an initiative set", [this]
		{
			BeforeEach([=, this]
			{
				this->Component->SetCharacterInitiative(Character1, 1);
				this->Component->SetCharacterInitiative(Character2, 2);
				this->Component->SetCharacterInitiative(Character3, 3);
			});

			It("clears initiatives for all characters", [this]
			{
				this->Component->ClearInitiativeForAllCharacters();

				TestEqual(
					"GetCharacterInitiative(Character1)",
					this->Component->GetCharacterInitiative(Character1),
					INDEX_NONE
				);

				TestEqual(
					"GetCharacterInitiative(Character2)",
					this->Component->GetCharacterInitiative(Character2),
					INDEX_NONE
				);

				TestEqual(
					"GetCharacterInitiative(Character3)",
					this->Component->GetCharacterInitiative(Character3),
					INDEX_NONE
				);

				TestEqual(
					"GetCharacterInitiative(Character4)",
					this->Component->GetCharacterInitiative(Character4),
					INDEX_NONE
				);

				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{}
				);
			});

			It("does not prevent characters from being assigned an initiative again in the future", [this]
			{
				this->Component->ClearInitiativeForAllCharacters();

				this->Component->SetCharacterInitiative(Character1, 5);
				this->Component->SetCharacterInitiative(Character2, 1);
				this->Component->SetCharacterInitiative(Character3, 4);

				TestEqual("GetCharacterInitiative(Character1)", this->Component->GetCharacterInitiative(Character1), 5);
				TestEqual("GetCharacterInitiative(Character2)", this->Component->GetCharacterInitiative(Character2), 1);
				TestEqual("GetCharacterInitiative(Character3)", this->Component->GetCharacterInitiative(Character3), 4);

				TestEqual(
					"GetCharacterInitiative(Character4)",
					this->Component->GetCharacterInitiative(Character4),
					INDEX_NONE
				);

				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{
						Character1,
						Character3,
						Character2,
					}
				);
			});
		});
	});

	Describe("GetNextCharacterByInitiative", [this]
	{
		Describe("when the queue is empty", [this]
		{
			It("returns a script interface wrapping `nullptr`", [this]
			{
				TestEqual(
					"GetNextCharacterByInitiative()",
					this->Component->GetNextCharacterByInitiative(),
					TScriptInterface<IPF2CharacterInterface>(nullptr)
				);
			});
		});

		Describe("when the queue contains 1 character", [this]
		{
			static TScriptInterface<IPF2CharacterInterface> Character;

			BeforeEach([=, this]
			{
				Character = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());

				this->Component->SetCharacterInitiative(Character, 23);
			});

			It("returns the same character every time", [this]
			{
				// Cycle 5 times to confirm that the same result is returned multiple times.
				for (int32 i = 0; i < 5; ++i)
				{
					TestEqual(
						"GetNextCharacterByInitiative()",
						this->Component->GetNextCharacterByInitiative(),
						Character
					);
				}
			});
		});

		Describe("when the queue contains multiple characters", [this]
		{
			static TScriptInterface<IPF2CharacterInterface> Character1,
			                                                Character2,
			                                                Character3,
			                                                Character4,
			                                                Character5;

			BeforeEach([=, this]
			{
				Character1 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character2 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character3 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character4 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character5 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());

				// By initiative, order should be:
				// - Character1 [23]
				// - Character4 [15]
				// - Character2 [10]
				// - Character5 [9]
				// - Character3 [8]
				this->Component->SetCharacterInitiative(Character2, 10);
				this->Component->SetCharacterInitiative(Character1, 23);
				this->Component->SetCharacterInitiative(Character3, 8);
				this->Component->SetCharacterInitiative(Character4, 15);
				this->Component->SetCharacterInitiative(Character5, 9);
			});

			It("cycles through the characters in order from highest to lowest initiative", [this]
			{
				// Cycle 5 times to confirm that the same sequence is returned multiple times.
				for (int32 i = 0; i < 5; ++i)
				{
					TestEqual(
						"GetNextCharacterByInitiative() = Character1",
						this->Component->GetNextCharacterByInitiative(),
						Character1
					);

					TestEqual(
						"GetNextCharacterByInitiative() = Character4",
						this->Component->GetNextCharacterByInitiative(),
						Character4
					);

					TestEqual(
						"GetNextCharacterByInitiative() = Character2",
						this->Component->GetNextCharacterByInitiative(),
						Character2
					);

					TestEqual(
						"GetNextCharacterByInitiative() = Character5",
						this->Component->GetNextCharacterByInitiative(),
						Character5
					);

					TestEqual(
						"GetNextCharacterByInitiative() = Character3",
						this->Component->GetNextCharacterByInitiative(),
						Character3
					);
				}
			});

			Describe("when a character gets removed from the queue", [this]
			{
				Describe("when the character that was removed is neither the previous character returned nor the next character", [this]
				{
					It("cycles through the remaining characters in order from highest to lowest initiative", [this]
					{
						this->Component->ClearInitiativeForCharacter(Character4);

						// Cycle 5 times to confirm that the same sequence is returned multiple times.
						for (int32 i = 0; i < 5; ++i)
						{
							TestEqual(
								"GetNextCharacterByInitiative() = Character1",
								this->Component->GetNextCharacterByInitiative(),
								Character1
							);

							TestEqual(
								"GetNextCharacterByInitiative() = Character2",
								this->Component->GetNextCharacterByInitiative(),
								Character2
							);

							TestEqual(
								"GetNextCharacterByInitiative() = Character5",
								this->Component->GetNextCharacterByInitiative(),
								Character5
							);

							TestEqual(
								"GetNextCharacterByInitiative() = Character3",
								this->Component->GetNextCharacterByInitiative(),
								Character3
							);
						}
					});
				});

				Describe("when the character that was removed was the previous character returned", [this]
				{
					Describe("when it was the first character in the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Returns Character1.
							this->Component->GetNextCharacterByInitiative();

							this->Component->ClearInitiativeForCharacter(Character1);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character3",
									this->Component->GetNextCharacterByInitiative(),
									Character3
								);
							}
						});
					});

					Describe("when it was a character in the middle of the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Returns Character1.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character4.
							this->Component->GetNextCharacterByInitiative();

							this->Component->ClearInitiativeForCharacter(Character4);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character3",
									this->Component->GetNextCharacterByInitiative(),
									Character3
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character1",
									this->Component->GetNextCharacterByInitiative(),
									Character1
								);
							}
						});
					});

					Describe("when it was the last character in the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Returns Character1.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character4.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character2.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character5.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character3.
							this->Component->GetNextCharacterByInitiative();

							this->Component->ClearInitiativeForCharacter(Character3);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character1",
									this->Component->GetNextCharacterByInitiative(),
									Character1
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);
							}
						});
					});
				});

				Describe("when the character that was removed would have been the next character returned", [this]
				{
					Describe("when it was the first character in the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							this->Component->ClearInitiativeForCharacter(Character1);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character3",
									this->Component->GetNextCharacterByInitiative(),
									Character3
								);
							}
						});
					});

					Describe("when it was a character in the middle of the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Returns Character1.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character4.
							this->Component->GetNextCharacterByInitiative();

							this->Component->ClearInitiativeForCharacter(Character2);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character3",
									this->Component->GetNextCharacterByInitiative(),
									Character3
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character1",
									this->Component->GetNextCharacterByInitiative(),
									Character1
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);
							}
						});
					});

					Describe("when it was the last character in the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Returns Character1.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character4.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character2.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character5.
							this->Component->GetNextCharacterByInitiative();

							this->Component->ClearInitiativeForCharacter(Character3);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character1",
									this->Component->GetNextCharacterByInitiative(),
									Character1
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);
							}
						});
					});
				});
			});

			Describe("when a character's initiative gets changed in the queue", [this]
			{
				Describe("when the character whose initiative was changed is neither the previous character returned nor the next character", [this]
				{
					It("cycles through the remaining characters in order from highest to lowest initiative", [this]
					{
						// Insert Character4 below Character2 (initiative 10).
						this->Component->InsertCharacterAtOrBelowInitiative(Character4, 10);

						// Cycle 5 times to confirm that the same sequence is returned multiple times.
						for (int32 i = 0; i < 5; ++i)
						{
							TestEqual(
								"GetNextCharacterByInitiative() = Character1",
								this->Component->GetNextCharacterByInitiative(),
								Character1
							);

							TestEqual(
								"GetNextCharacterByInitiative() = Character2",
								this->Component->GetNextCharacterByInitiative(),
								Character2
							);

							TestEqual(
								"GetNextCharacterByInitiative() = Character4",
								this->Component->GetNextCharacterByInitiative(),
								Character4
							);

							TestEqual(
								"GetNextCharacterByInitiative() = Character5",
								this->Component->GetNextCharacterByInitiative(),
								Character5
							);

							TestEqual(
								"GetNextCharacterByInitiative() = Character3",
								this->Component->GetNextCharacterByInitiative(),
								Character3
							);
						}
					});
				});

				Describe("when the character whose initiative was changed was the previous character returned", [this]
				{
					Describe("when it was the first character in the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Returns Character1.
							this->Component->GetNextCharacterByInitiative();

							// Insert Character1 below Character2 (initiative 10).
							this->Component->InsertCharacterAtOrBelowInitiative(Character1, 10);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character1",
									this->Component->GetNextCharacterByInitiative(),
									Character1
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character3",
									this->Component->GetNextCharacterByInitiative(),
									Character3
								);
							}
						});
					});

					Describe("when it was a character in the middle of the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Returns Character1.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character4.
							this->Component->GetNextCharacterByInitiative();

							// Insert Character4 below Character2 (initiative 10).
							this->Component->InsertCharacterAtOrBelowInitiative(Character4, 10);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character3",
									this->Component->GetNextCharacterByInitiative(),
									Character3
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character1",
									this->Component->GetNextCharacterByInitiative(),
									Character1
								);
							}
						});
					});

					Describe("when it was the last character in the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Returns Character1.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character4.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character2.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character5.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character3.
							this->Component->GetNextCharacterByInitiative();

							// Insert Character3 below Character2 (initiative 10).
							this->Component->InsertCharacterAtOrBelowInitiative(Character3, 10);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character1",
									this->Component->GetNextCharacterByInitiative(),
									Character1
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character3",
									this->Component->GetNextCharacterByInitiative(),
									Character3
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);
							}
						});
					});
				});

				Describe("when the character whose initiative was changed would have been the next character returned", [this]
				{
					Describe("when it was the first character in the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Insert Character1 below Character2 (initiative 10).
							this->Component->InsertCharacterAtOrBelowInitiative(Character1, 10);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character1",
									this->Component->GetNextCharacterByInitiative(),
									Character1
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character3",
									this->Component->GetNextCharacterByInitiative(),
									Character3
								);
							}
						});
					});

					Describe("when it was a character in the middle of the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Returns Character1.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character4.
							this->Component->GetNextCharacterByInitiative();

							// Insert Character2 below Character5 (initiative 9).
							this->Component->InsertCharacterAtOrBelowInitiative(Character2, 9);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character3",
									this->Component->GetNextCharacterByInitiative(),
									Character3
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character1",
									this->Component->GetNextCharacterByInitiative(),
									Character1
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);
							}
						});
					});

					Describe("when it was the last character in the queue", [this]
					{
						It("cycles through the remaining characters in order from highest to lowest initiative", [this]
						{
							// Returns Character1.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character4.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character2.
							this->Component->GetNextCharacterByInitiative();

							// Returns Character5.
							this->Component->GetNextCharacterByInitiative();

							// Insert Character3 below Character2 (initiative 10).
							this->Component->InsertCharacterAtOrBelowInitiative(Character3, 10);

							// Cycle 5 times to confirm that the same sequence is returned multiple times.
							for (int32 i = 0; i < 5; ++i)
							{
								TestEqual(
									"GetNextCharacterByInitiative() = Character1",
									this->Component->GetNextCharacterByInitiative(),
									Character1
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character4",
									this->Component->GetNextCharacterByInitiative(),
									Character4
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character2",
									this->Component->GetNextCharacterByInitiative(),
									Character2
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character3",
									this->Component->GetNextCharacterByInitiative(),
									Character3
								);

								TestEqual(
									"GetNextCharacterByInitiative() = Character5",
									this->Component->GetNextCharacterByInitiative(),
									Character5
								);
							}
						});
					});
				});
			});
		});
	});

	Describe("GetCharactersInInitiativeOrder", [this]
	{
		Describe("when the queue is empty", [this]
		{
			It("returns an empty array", [this]
			{
				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{}
				);
			});
		});

		Describe("when the queue contains 1 character", [this]
		{
			static TScriptInterface<IPF2CharacterInterface> Character;

			BeforeEach([=, this]
			{
				Character = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());

				this->Component->SetCharacterInitiative(Character, 23);
			});

			It("returns an array containing only that character", [this]
			{
				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{Character}
				);
			});
		});

		Describe("when the queue contains multiple characters", [this]
		{
			static TScriptInterface<IPF2CharacterInterface> Character1,
			                                                Character2,
			                                                Character3,
			                                                Character4,
			                                                Character5;

			BeforeEach([=, this]
			{
				Character1 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character2 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character3 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character4 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());
				Character5 = PF2InterfaceUtilities::ToScriptInterface(this->SpawnCharacter());

				// By initiative, order should be:
				// - Character1 [23]
				// - Character4 [15]
				// - Character2 [10]
				// - Character5 [9]
				// - Character3 [8]
				this->Component->SetCharacterInitiative(Character2, 10);
				this->Component->SetCharacterInitiative(Character1, 23);
				this->Component->SetCharacterInitiative(Character3, 8);
				this->Component->SetCharacterInitiative(Character4, 15);
				this->Component->SetCharacterInitiative(Character5, 9);
			});

			It("returns an array containing all characters ordered from highest to lowest initiative", [this]
			{
				TestArrayEquals(
					"GetCharactersInInitiativeOrder()",
					this->Component->GetCharactersInInitiativeOrder(),
					{
						Character1,
						Character4,
						Character2,
						Character5,
						Character3,
					}
				);
			});
		});
	});
}
