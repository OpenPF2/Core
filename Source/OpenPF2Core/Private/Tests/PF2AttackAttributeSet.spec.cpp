// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2AttackAttributeSet.h"
#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2AttackAttributeSetSpec,
                     "OpenPF2.AttackAttributeSet",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
END_DEFINE_PF_SPEC(FPF2AttackAttributeSetSpec)

void FPF2AttackAttributeSetSpec::Define()
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

	Describe("At UPF2AttackAttributeSet construction time", [this]()
	{
		It("has default values for all defined attributes", [this]()
		{
			const UPF2AttackAttributeSet* AttributeSet = this->TestPawnAsc->GetSet<UPF2AttackAttributeSet>();

			TestEqual(TEXT("TmpAttackRollCount"),            AttributeSet->GetTmpAttackRollCount(),            1.0f);
			TestEqual(TEXT("TmpAttackRollSize"),             AttributeSet->GetTmpAttackRollSize(),            20.0f);
			TestEqual(TEXT("TmpAttackDegreeOfSuccess"),      AttributeSet->GetTmpAttackDegreeOfSuccess(),      0.0f);
			TestEqual(TEXT("TmpDmgRollCount"),               AttributeSet->GetTmpDmgRollCount(),               0.0f);
			TestEqual(TEXT("TmpDmgRollSize"),                AttributeSet->GetTmpDmgRollSize(),                0.0f);
			TestEqual(TEXT("TmpDmgTypePhysicalBludgeoning"), AttributeSet->GetTmpDmgTypePhysicalBludgeoning(), 0.0f);
			TestEqual(TEXT("TmpDmgTypePhysicalPiercing"),    AttributeSet->GetTmpDmgTypePhysicalPiercing(),    0.0f);
			TestEqual(TEXT("TmpDmgTypePhysicalSlashing"),    AttributeSet->GetTmpDmgTypePhysicalSlashing(),    0.0f);
			TestEqual(TEXT("TmpDmgTypeEnergyAcid"),          AttributeSet->GetTmpDmgTypeEnergyAcid(),          0.0f);
			TestEqual(TEXT("TmpDmgTypeEnergyCold"),          AttributeSet->GetTmpDmgTypeEnergyCold(),          0.0f);
			TestEqual(TEXT("TmpDmgTypeEnergyElectricity"),   AttributeSet->GetTmpDmgTypeEnergyElectricity(),   0.0f);
			TestEqual(TEXT("TmpDmgTypeEnergyFire"),          AttributeSet->GetTmpDmgTypeEnergyFire(),          0.0f);
			TestEqual(TEXT("TmpDmgTypeEnergySonic"),         AttributeSet->GetTmpDmgTypeEnergySonic(),         0.0f);
			TestEqual(TEXT("TmpDmgTypeEnergyPositive"),      AttributeSet->GetTmpDmgTypeEnergyPositive(),      0.0f);
			TestEqual(TEXT("TmpDmgTypeEnergyNegative"),      AttributeSet->GetTmpDmgTypeEnergyNegative(),      0.0f);
			TestEqual(TEXT("TmpDmgTypeEnergyForce"),         AttributeSet->GetTmpDmgTypeEnergyForce(),         0.0f);
			TestEqual(TEXT("TmpDmgTypeAlignmentChaotic"),    AttributeSet->GetTmpDmgTypeAlignmentChaotic(),    0.0f);
			TestEqual(TEXT("TmpDmgTypeAlignmentEvil"),       AttributeSet->GetTmpDmgTypeAlignmentEvil(),       0.0f);
			TestEqual(TEXT("TmpDmgTypeAlignmentGood"),       AttributeSet->GetTmpDmgTypeAlignmentGood(),       0.0f);
			TestEqual(TEXT("TmpDmgTypeAlignmentLawful"),     AttributeSet->GetTmpDmgTypeAlignmentLawful(),     0.0f);
			TestEqual(TEXT("TmpDmgTypeMental"),              AttributeSet->GetTmpDmgTypeMental(),              0.0f);
			TestEqual(TEXT("TmpDmgTypePoison"),              AttributeSet->GetTmpDmgTypePoison(),              0.0f);
			TestEqual(TEXT("TmpDmgTypeBleed"),               AttributeSet->GetTmpDmgTypeBleed(),               0.0f);
			TestEqual(TEXT("TmpDmgTypePrecision"),           AttributeSet->GetTmpDmgTypePrecision(),           0.0f);
		});
	});
}
