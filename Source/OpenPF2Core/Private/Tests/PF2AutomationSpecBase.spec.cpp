// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <GameplayEffect.h>

#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2AutomationSpecBaseSpec,
                     "OpenPF2.AutomationSpecBase",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	int32 BeforeAllRunCount1;
	FString BeforeAllRunOrder1;
	FString BeforeEachRunOrder1;

	int32 BeforeAllRunCount2;
	FString BeforeAllRunOrder2;
	FString BeforeEachRunOrder2;

	int32 BeforeAllRunCount3;
	FString BeforeAllRunOrder3;
	FString BeforeEachRunOrder3;

	int32 BeforeAllRunCount4;
	FString BeforeAllRunOrder4;
	FString BeforeEachRunOrder4;

	int32 BeforeAllRunCount5;
	FString BeforeAllRunOrder5;
	FString BeforeEachRunOrder5;

	int32 BeforeAllRunCount6;
	FString BeforeAllRunOrder6;
	FString BeforeEachRunOrder6;

	int32 BeforeAllRunCount7;
	FString BeforeAllRunOrder7;
	FString BeforeEachRunOrder7;

	int32 BeforeAllRunCount8;
	FString BeforeAllRunOrder8;
	FString BeforeEachRunOrder8;

	int32 BeforeAllRunCount9;
	FString BeforeAllRunOrder9;
	FString BeforeEachRunOrder9;

	bool bBlock1Ran;
	bool bBlock2Ran;
	bool bBeforeAllBlock1Ran;
	bool bBeforeAllBlock2Ran;

	struct FTestObject
	{
		FString SomeValue;

		explicit FTestObject(const FString& SomeValue) : SomeValue(SomeValue)
		{
		}
	};
END_DEFINE_PF_SPEC(FPF2AutomationSpecBaseSpec)

void FPF2AutomationSpecBaseSpec::Define()
{
	Describe("Let()", [=, this]
	{
		Describe("when a variable is defined in a scope", [=, this]
		{
			const TSpecVariable<TSharedPtr<FTestObject>> OuterValue1 =
				Let(TGeneratorFunc<TSharedPtr<FTestObject>>([]{ return MakeShared<FTestObject>("Outer"); }));

			const TSpecVariable<TSharedPtr<FTestObject>> OuterValue2 =
				Let(TGeneratorFunc<TSharedPtr<FTestObject>>([OuterValue1]{ return *OuterValue1; }));

			It("can supply the value via Get()", [=, this]
			{
				TestEqual("OuterValue1.Get().SomeValue", OuterValue1.Get().Get()->SomeValue, "Outer");
			});

			It("can supply the value via dereferencing", [=, this]
			{
				TestEqual("*OuterValue1->SomeValue", (*OuterValue1).Get()->SomeValue, "Outer");
			});

			It("can supply the value via arrow dereferencing", [=, this]
			{
				TestEqual("OuterValue1->SomeValue", OuterValue1->SomeValue, "Outer");
			});

			It("returns the same value every time during the same test", [=, this]
			{
				(*OuterValue1)->SomeValue = "Changed";

				TestEqual("OuterValue1", OuterValue1.Get().Get()->SomeValue, "Changed");
				TestEqual("OuterValue1", (*OuterValue1).Get()->SomeValue, "Changed");
				TestEqual("OuterValue1", OuterValue1->SomeValue, "Changed");
			});

			It("can provide values to variables after it in the scope", [=, this]
			{
				TestEqual("OuterValue2->SomeValue", OuterValue2->SomeValue, "Outer");
			});

			Describe("when a different variable is defined in a nested scope", [=, this]
			{
				const TSpecVariable<TSharedPtr<FTestObject>> InnerValue =
					Let(TGeneratorFunc<TSharedPtr<FTestObject>>([] { return MakeShared<FTestObject>("Inner"); }));

				It("tracks the two variable separately in the current scope", [=, this]
				{
					TestEqual("*OuterValue1->SomeValue", OuterValue1->SomeValue, "Outer");
					TestEqual("*InnerValue->SomeValue", InnerValue->SomeValue, "Inner");
				});
			});

			Describe("when the same variable is redefined a second time in the same scope", [=, this]
			{
				Describe("when the redefinition does not reference the original value", [=, this]
				{
					const TSpecVariable<FString> MyVariable = Let(TGeneratorFunc<FString>([] { return "ABC"; }));

					RedefineLet(
						MyVariable,
						TGeneratorRedefineFunc<FString>([](const TSpecVariablePtr<FString>&) { return "DEF"; })
					);

					It("replaces the original value in the scope", [=, this]
					{
						TestEqual("MyVariable", *MyVariable, "DEF");
					});
				});

				Describe("when the redefinition references the original value", [=, this]
				{
					const TSpecVariable<FString> MyVariable = Let(TGeneratorFunc<FString>([]
					{
						return "ABC";
					}));

					RedefineLet(MyVariable, TGeneratorRedefineFunc<FString>([](const TSpecVariablePtr<FString>& Original)
					{
						return **Original + "DEF";
					}));

					It("replaces the original value in the scope", [=, this]
					{
						// ReSharper disable once StringLiteralTypo
						TestEqual("MyVariable", *MyVariable, "ABCDEF");
					});
				});
			});

			Describe("when changing the value of a variable via its reference", [=, this]
			{
				const TSpecVariable<FString> MyVariable = Let(TGeneratorFunc<FString>([] { return "ABC"; }));

				It("affects the value of the variable in the test that changes it", [=, this]
				{
					*MyVariable = "DEF";

					TestEqual("MyVariable", *MyVariable, "DEF");
				});

				It("does not affect the value of the variable in other tests", [=, this]
				{
					TestEqual("MyVariable", *MyVariable, "ABC");
				});
			});

			Describe("when the same variable is redefined in a nested scope", [=, this]
			{
				Describe("when the redefinition does not reference the original value", [=, this]
				{
					RedefineLet(
						OuterValue1,
						TGeneratorRedefineFunc<TSharedPtr<FTestObject>>([](const TSpecVariablePtr<TSharedPtr<FTestObject>>&)
						{
							return MakeShared<FTestObject>("Inner");
						})
					);

					It("replaces the original value in the scope", [=, this]
					{
						TestEqual("OuterValue1->SomeValue", OuterValue1->SomeValue, "Inner");
					});

					It("impacts the values of dependent variables in the outer scope", [=, this]
					{
						TestEqual("OuterValue2->SomeValue", OuterValue2->SomeValue, "Inner");
					});

					Describe("when the same variable is redefined a third time in an even deeper nested scope", [=, this]
					{
						Describe("when the second redefinition does not reference the original value", [=, this]
						{
							RedefineLet(
								OuterValue1,
								TGeneratorRedefineFunc<TSharedPtr<FTestObject>>([](const TSpecVariablePtr<TSharedPtr<FTestObject>>&)
								{
									return MakeShared<FTestObject>("DeepInner");
								})
							);

							It("replaces the original value in the scope", [=, this]
							{
								TestEqual("OuterValue1->SomeValue", OuterValue1->SomeValue, "DeepInner");
							});

							It("impacts the values of dependent variables in the outer scope", [=, this]
							{
								TestEqual("OuterValue2->SomeValue", OuterValue2->SomeValue, "DeepInner");
							});
						});

						Describe("when the second redefinition references the original value", [=, this]
						{
							RedefineLet(
								OuterValue1,
								TGeneratorRedefineFunc<TSharedPtr<FTestObject>>([](const TSpecVariablePtr<TSharedPtr<FTestObject>>& Original)
								{
									return MakeShared<FTestObject>((*Original)->SomeValue + "DeepInner");
								})
							);

							It("replaces the original value in the scope", [=, this]
							{
								TestEqual("OuterValue1->SomeValue", OuterValue1->SomeValue, "InnerDeepInner");
							});

							It("impacts the values of dependent variables in the outer scope", [=, this]
							{
								TestEqual("OuterValue2->SomeValue", OuterValue2->SomeValue, "InnerDeepInner");
							});
						});
					});
				});

				Describe("when the redefinition references the original value", [=, this]
				{
					RedefineLet(
						OuterValue1,
						TGeneratorRedefineFunc<TSharedPtr<FTestObject>>([](const TSpecVariablePtr<TSharedPtr<FTestObject>>& Original)
						{
							return MakeShared<FTestObject>((*Original)->SomeValue + "Inner");
						})
					);

					It("replaces the original value in the scope", [=, this]
					{
						TestEqual("OuterValue1->SomeValue", OuterValue1->SomeValue, "OuterInner");
					});

					It("impacts the values of dependent variables in the outer scope", [=, this]
					{
						TestEqual("OuterValue2->SomeValue", OuterValue2->SomeValue, "OuterInner");
					});

					Describe("when the same variable is redefined a third time in an even deeper nested scope", [=, this]
					{
						Describe("when the second redefinition does not reference the original value", [=, this]
						{
							RedefineLet(
								OuterValue1,
								TGeneratorRedefineFunc<TSharedPtr<FTestObject>>([](const TSpecVariablePtr<TSharedPtr<FTestObject>>&)
								{
									return MakeShared<FTestObject>("DeepInner");
								})
							);

							It("replaces the original value in the scope", [=, this]
							{
								TestEqual("OuterValue1->SomeValue", OuterValue1->SomeValue, "DeepInner");
							});

							It("impacts the values of dependent variables in the outer scope", [=, this]
							{
								TestEqual("OuterValue2->SomeValue", OuterValue2->SomeValue, "DeepInner");
							});
						});

						Describe("when the second redefinition references the original value", [=, this]
						{
							RedefineLet(
								OuterValue1,
								TGeneratorRedefineFunc<TSharedPtr<FTestObject>>([](const TSpecVariablePtr<TSharedPtr<FTestObject>>& Original)
								{
									return MakeShared<FTestObject>((*Original)->SomeValue + "DeepInner");
								})
							);

							It("replaces the original value in the scope", [=, this]
							{
								TestEqual("OuterValue1->SomeValue", OuterValue1->SomeValue, "OuterInnerDeepInner");
							});

							It("impacts the values of dependent variables in the outer scope", [=, this]
							{
								TestEqual("OuterValue2->SomeValue", OuterValue2->SomeValue, "OuterInnerDeepInner");
							});
						});
					});
				});
			});
		});
	});

	Describe("BeforeEach() and Let()", [=, this]
	{
		Describe("when a variable is referenced by a BeforeEach() block", [=, this]
		{
			const TSpecVariable<FString> Variable = Let(TGeneratorFunc<FString>([]()
			{
				return "ABC";
			}));

			BeforeEach([=, this]
			{
				*Variable += "XYZ";
			});

			It("provides a value to the BeforeEach() block the same as in a test", [=, this]
			{
				// ReSharper disable once StringLiteralTypo
				TestEqual("Variable", *Variable, "ABCXYZ");
			});

			Describe("when the variable is redefined in a nested scope", [=, this]
			{
				RedefineLet(
					Variable,
					TGeneratorRedefineFunc<FString>([](const TSpecVariablePtr<FString>&)
					{
						return "Inner";
					})
				);

				It("provides the redefined value to the outer BeforeEach() block", [=, this]
				{
					// ReSharper disable once StringLiteralTypo
					TestEqual("Variable", *Variable, "InnerXYZ");
				});
			});
		});
	});

	Describe("BeforeAll()", [=, this]
	{
		BeforeAll([=, this]
		{
			this->BeforeAllRunCount1  = 0;
			this->BeforeAllRunCount2  = 0;
			this->BeforeAllRunCount3  = 0;
			this->BeforeAllRunCount4  = 0;
			this->BeforeAllRunCount5  = 0;
			this->BeforeAllRunCount6  = 0;
			this->BeforeAllRunCount7  = 0;
			this->BeforeAllRunCount8  = 0;
			this->BeforeAllRunCount9  = 0;
			this->bBlock1Ran          = false;
			this->bBlock2Ran          = false;
			this->bBeforeAllBlock1Ran = false;
			this->bBeforeAllBlock2Ran = false;
		});

		Describe("when multiple specs use BeforeAll()", [=, this]
		{
			Describe("when running tests in the first block and only the first block has run", [=, this]
			{
				BeforeAll([=, this]
				{
					this->bBeforeAllBlock1Ran = true;
				});

				It("evaluates the BeforeAll() only for the first block", [=, this]
				{
					this->bBlock1Ran = true;

					if (!this->bBlock2Ran)
					{
						TestFalse("bBeforeAllBlock2Ran", this->bBeforeAllBlock2Ran);
					}
				});
			});

			Describe("when running tests in the second block and only the second block has run", [=, this]
			{
				BeforeAll([=, this]
				{
					this->bBeforeAllBlock2Ran = true;
				});

				It("evaluates the BeforeAll() only for the second block", [=, this]
				{
					this->bBlock2Ran = true;

					if (!this->bBlock1Ran)
					{
						TestFalse("bBeforeAllBlock1Ran", this->bBeforeAllBlock1Ran);
					}
				});
			});
		});

		Describe("when a spec uses BeforeEach() with BeforeAll()", [=, this]
		{
			Describe("when BeforeEach() is ordered before BeforeAll()", [=, this]
			{
				BeforeEach([=, this]
				{
					this->BeforeEachRunOrder1 = this->BeforeAllRunOrder1 + TEXT("B");
				});

				BeforeAll([=, this]
				{
					++this->BeforeAllRunCount1;
					this->BeforeAllRunOrder1 = TEXT("A");
				});

				It("evaluates BeforeAll() before BeforeEach()", [=, this]
				{
					TestEqual("RunOrder", this->BeforeEachRunOrder1, TEXT("AB"));
				});

				It("only runs each BeforeAll() once", [=, this]
				{
					TestEqual("RunCount", this->BeforeAllRunCount1, 1);
				});
			});

			Describe("when BeforeEach() is ordered after BeforeAll()", [=, this]
			{
				BeforeAll([=, this]
				{
					++this->BeforeAllRunCount2;
					this->BeforeAllRunOrder2 = TEXT("B");
				});

				BeforeEach([=, this]
				{
					this->BeforeEachRunOrder2 = this->BeforeAllRunOrder2 + TEXT("C");
				});

				It("evaluates BeforeAll() before BeforeEach()", [=, this]
				{
					TestEqual("RunOrder", this->BeforeEachRunOrder2, TEXT("BC"));
				});

				It("only runs each BeforeAll() once", [=, this]
				{
					TestEqual("RunCount", this->BeforeAllRunCount2, 1);
				});
			});
		});

		Describe("when a spec uses BeforeEach() in an outer scope", [=, this]
		{
			BeforeEach([=, this]
			{
				this->BeforeEachRunOrder3 = this->BeforeAllRunOrder3 + TEXT("Y");
			});

			Describe("when the spec uses BeforeAll() in an inner scope", [=, this]
			{
				BeforeAll([=, this]
				{
					++this->BeforeAllRunCount3;
					this->BeforeAllRunOrder3 = TEXT("X");
				});

				It("evaluates BeforeAll() in the inner scope before BeforeEach() in the outer scope", [=, this]
				{
					TestEqual("RunOrder", this->BeforeEachRunOrder3, TEXT("XY"));
				});

				It("only runs each BeforeAll() once", [=, this]
				{
					TestEqual("RunCount", this->BeforeAllRunCount3, 1);
				});
			});
		});

		Describe("when a spec uses both BeforeAll() and BeforeEach() in an outer scope", [=, this]
		{
			Describe("when the outer BeforeEach() is ordered before BeforeAll()", [=, this]
			{
				BeforeEach([=, this]
				{
					this->BeforeEachRunOrder4 = this->BeforeAllRunOrder4 + TEXT("Z");
					this->BeforeEachRunOrder5 = this->BeforeAllRunOrder5 + TEXT("C");
					this->BeforeEachRunOrder6 = this->BeforeAllRunOrder6 + TEXT("G");
				});

				BeforeAll([=, this]
				{
					++this->BeforeAllRunCount4;
					this->BeforeAllRunOrder4 = TEXT("X");
					this->BeforeAllRunOrder5 = TEXT("A");
					this->BeforeAllRunOrder6 = TEXT("E");
				});

				Describe("when the spec uses BeforeAll() in an inner scope", [=, this]
				{
					BeforeAll([=, this]
					{
						this->BeforeAllRunOrder4 += TEXT("Y");
					});

					It("evaluates BeforeAll() in the outer scope, BeforeAll() in the inner scope, and then BeforeEach() in the outer scope", [=, this]
					{
						TestEqual("RunOrder", this->BeforeEachRunOrder4, TEXT("XYZ"));
					});

					It("only runs each BeforeAll() once", [=, this]
					{
						TestEqual("RunCount", this->BeforeAllRunCount4, 1);
					});
				});

				Describe("when the spec uses both BeforeAll() and BeforeEach() in the inner scope", [=, this]
				{
					Describe("when the inner BeforeEach() is ordered before BeforeAll()", [=, this]
					{
						BeforeEach([=, this]
						{
							this->BeforeEachRunOrder5 += TEXT("D");
						});

						BeforeAll([=, this]
						{
							++this->BeforeAllRunCount5;
							this->BeforeAllRunOrder5 += TEXT("B");
						});

						It("evaluates BeforeAll() in the outer scope, BeforeAll() in the inner scope, BeforeEach() in the outer scope, and then BeforeEach() in the outer scope", [=, this]
						{
							TestEqual("RunOrder", this->BeforeEachRunOrder5, TEXT("ABCD"));
						});

						It("only runs each BeforeAll() once", [=, this]
						{
							TestEqual("RunCount", this->BeforeAllRunCount5, 1);
						});
					});

					Describe("when the inner BeforeEach() is ordered after BeforeAll()", [=, this]
					{
						BeforeAll([=, this]
						{
							++this->BeforeAllRunCount6;
							this->BeforeAllRunOrder6 += TEXT("F");
						});

						BeforeEach([=, this]
						{
							this->BeforeEachRunOrder6 += TEXT("H");
						});

						It("evaluates BeforeAll() in the outer scope, BeforeAll() in the inner scope, BeforeEach() in the outer scope, and then BeforeEach() in the outer scope", [=, this]
						{
							TestEqual("RunOrder", this->BeforeEachRunOrder6, TEXT("EFGH"));
						});

						It("only runs each BeforeAll() once", [=, this]
						{
							TestEqual("RunCount", this->BeforeAllRunCount6, 1);
						});
					});
				});
			});

			Describe("when the outer BeforeEach() is ordered after BeforeAll()", [=, this]
			{
				BeforeAll([=, this]
				{
					++this->BeforeAllRunCount7;
					this->BeforeAllRunOrder7 = TEXT("X");
					this->BeforeAllRunOrder8 = TEXT("A");
					this->BeforeAllRunOrder9 = TEXT("E");
				});

				BeforeEach([=, this]
				{
					this->BeforeEachRunOrder7 = this->BeforeAllRunOrder7 + TEXT("Z");
					this->BeforeEachRunOrder8 = this->BeforeAllRunOrder8 + TEXT("C");
					this->BeforeEachRunOrder9 = this->BeforeAllRunOrder9 + TEXT("G");
				});

				Describe("when the spec uses BeforeAll() in an inner scope", [=, this]
				{
					BeforeAll([=, this]
					{
						this->BeforeAllRunOrder7 += TEXT("Y");
					});

					It("evaluates BeforeAll() in the outer scope, BeforeAll() in the inner scope, and then BeforeEach() in the outer scope", [=, this]
					{
						TestEqual("RunOrder", this->BeforeEachRunOrder7, TEXT("XYZ"));
					});

					It("only runs each BeforeAll() once", [=, this]
					{
						TestEqual("RunCount", this->BeforeAllRunCount7, 1);
					});
				});

				Describe("when the spec uses both BeforeAll() and BeforeEach() in the inner scope", [=, this]
				{
					Describe("when the inner BeforeEach() is ordered before BeforeAll()", [=, this]
					{
						BeforeEach([=, this]
						{
							this->BeforeEachRunOrder8 += TEXT("D");
						});

						BeforeAll([=, this]
						{
							++this->BeforeAllRunCount8;
							this->BeforeAllRunOrder8 += TEXT("B");
						});

						It("evaluates BeforeAll() in the outer scope, BeforeAll() in the inner scope, BeforeEach() in the outer scope, and then BeforeEach() in the outer scope", [=, this]
						{
							TestEqual("RunOrder", this->BeforeEachRunOrder8, TEXT("ABCD"));
						});

						It("only runs each BeforeAll() once", [=, this]
						{
							TestEqual("RunCount", this->BeforeAllRunCount8, 1);
						});
					});

					Describe("when the inner BeforeEach() is ordered after BeforeAll()", [=, this]
					{
						BeforeAll([=, this]
						{
							++this->BeforeAllRunCount9;
							this->BeforeAllRunOrder9 += TEXT("F");
						});

						BeforeEach([=, this]
						{
							this->BeforeEachRunOrder9 += TEXT("H");
						});

						It("evaluates BeforeAll() in the outer scope, BeforeAll() in the inner scope, BeforeEach() in the outer scope, and then BeforeEach() in the outer scope", [=, this]
						{
							TestEqual("RunOrder", this->BeforeEachRunOrder9, TEXT("EFGH"));
						});

						It("only runs each BeforeAll() once", [=, this]
						{
							TestEqual("RunCount", this->BeforeAllRunCount9, 1);
						});
					});
				});
			});
		});
	});
}
