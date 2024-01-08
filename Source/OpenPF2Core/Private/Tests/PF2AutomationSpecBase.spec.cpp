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
END_DEFINE_PF_SPEC(FPF2AutomationSpecBaseSpec)

void FPF2AutomationSpecBaseSpec::Define()
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
}
