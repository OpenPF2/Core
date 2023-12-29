// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2CharacterAttributeSet.h"
#include "Tests/PF2SpecBase.h"

BEGIN_DEFINE_PF_SPEC(FPF2CharacterAttributeSetSpec,
                     "OpenPF2.CharacterAttributeSet",
                     EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
END_DEFINE_PF_SPEC(FPF2CharacterAttributeSetSpec)

void FPF2CharacterAttributeSetSpec::Define()
{
	BeforeEach([=, this]()
	{
		this->SetupWorld();
		this->SetupPawn();

		this->BeginPlay();
	});

	AfterEach([=, this]()
	{
		this->DestroyPawn();
		this->DestroyWorld();
	});

	Describe("At UPF2CharacterAttributeSet construction time", [this]()
	{
		It("has default values for all defined attributes", [this]()
		{
			const UPF2CharacterAttributeSet* AttributeSet = this->TestPawnAsc->GetSet<UPF2CharacterAttributeSet>();

			TestEqual(TEXT("Experience"),                           AttributeSet->GetExperience(),                  0.0f);
			TestEqual(TEXT("AbBoostCount"),                         AttributeSet->GetAbBoostCount(),                0.0f);
			TestEqual(TEXT("AbBoostLimit"),                         AttributeSet->GetAbBoostLimit(),                0.0f);
			TestEqual(TEXT("AbStrength"),                           AttributeSet->GetAbStrength(),                 10.0f);
			TestEqual(TEXT("AbStrengthModifier"),                   AttributeSet->GetAbStrengthModifier(),          0.0f);
			TestEqual(TEXT("AbDexterity"),                          AttributeSet->GetAbDexterity(),                10.0f);
			TestEqual(TEXT("AbDexterityModifier"),                  AttributeSet->GetAbDexterityModifier(),         0.0f);
			TestEqual(TEXT("AbConstitution"),                       AttributeSet->GetAbConstitution(),             10.0f);
			TestEqual(TEXT("AbConstitutionModifier"),               AttributeSet->GetAbConstitutionModifier(),      0.0f);
			TestEqual(TEXT("AbIntelligence"),                       AttributeSet->GetAbIntelligence(),             10.0f);
			TestEqual(TEXT("AbIntelligenceModifier"),               AttributeSet->GetAbIntelligenceModifier(),      0.0f);
			TestEqual(TEXT("AbWisdom"),                             AttributeSet->GetAbWisdom(),                   10.0f);
			TestEqual(TEXT("AbWisdomModifier"),                     AttributeSet->GetAbWisdomModifier(),            0.0f);
			TestEqual(TEXT("AbCharisma"),                           AttributeSet->GetAbCharisma(),                 10.0f);
			TestEqual(TEXT("AbCharismaModifier"),                   AttributeSet->GetAbCharismaModifier(),          0.0f);
			TestEqual(TEXT("ClassDifficultyClass"),                 AttributeSet->GetClassDifficultyClass(),        0.0f);
			TestEqual(TEXT("Speed"),                                AttributeSet->GetSpeed(),                       1.0f);
			TestEqual(TEXT("MaxSpeed"),                             AttributeSet->GetMaxSpeed(),                    1.0f);
			TestEqual(TEXT("Reach"),                                AttributeSet->GetReach(),                     150.0f);
			TestEqual(TEXT("ArmorClass"),                           AttributeSet->GetArmorClass(),                 10.0f);
			TestEqual(TEXT("StFortitudeModifier"),                  AttributeSet->GetStFortitudeModifier(),         0.0f);
			TestEqual(TEXT("StReflexModifier"),                     AttributeSet->GetStReflexModifier(),            0.0f);
			TestEqual(TEXT("StWillModifier"),                       AttributeSet->GetStWillModifier(),              0.0f);
			TestEqual(TEXT("HitPoints"),                            AttributeSet->GetHitPoints(),                   1.0f);
			TestEqual(TEXT("MaxHitPoints"),                         AttributeSet->GetMaxHitPoints(),                1.0f);
			TestEqual(TEXT("RstPhysicalBludgeoning"),               AttributeSet->GetRstPhysicalBludgeoning(),      0.0f);
			TestEqual(TEXT("RstPhysicalPiercing"),                  AttributeSet->GetRstPhysicalPiercing(),         0.0f);
			TestEqual(TEXT("RstPhysicalSlashing"),                  AttributeSet->GetRstPhysicalSlashing(),         0.0f);
			TestEqual(TEXT("RstEnergyAcid"),                        AttributeSet->GetRstEnergyAcid(),               0.0f);
			TestEqual(TEXT("RstEnergyCold"),                        AttributeSet->GetRstEnergyCold(),               0.0f);
			TestEqual(TEXT("RstEnergyElectricity"),                 AttributeSet->GetRstEnergyElectricity(),        0.0f);
			TestEqual(TEXT("RstEnergyFire"),                        AttributeSet->GetRstEnergyFire(),               0.0f);
			TestEqual(TEXT("RstEnergySonic"),                       AttributeSet->GetRstEnergySonic(),              0.0f);
			TestEqual(TEXT("RstEnergyPositive"),                    AttributeSet->GetRstEnergyPositive(),           0.0f);
			TestEqual(TEXT("RstEnergyNegative"),                    AttributeSet->GetRstEnergyNegative(),           0.0f);
			TestEqual(TEXT("RstEnergyForce"),                       AttributeSet->GetRstEnergyForce(),              0.0f);
			TestEqual(TEXT("RstAlignmentChaotic"),                  AttributeSet->GetRstAlignmentChaotic(),         0.0f);
			TestEqual(TEXT("RstAlignmentEvil"),                     AttributeSet->GetRstAlignmentEvil(),            0.0f);
			TestEqual(TEXT("RstAlignmentGood"),                     AttributeSet->GetRstAlignmentGood(),            0.0f);
			TestEqual(TEXT("RstAlignmentLawful"),                   AttributeSet->GetRstAlignmentLawful(),          0.0f);
			TestEqual(TEXT("RstMental"),                            AttributeSet->GetRstMental(),                   0.0f);
			TestEqual(TEXT("RstPoison"),                            AttributeSet->GetRstPoison(),                   0.0f);
			TestEqual(TEXT("RstBleed"),                             AttributeSet->GetRstBleed(),                    0.0f);
			TestEqual(TEXT("RstPrecision"),                         AttributeSet->GetRstPrecision(),                0.0f);
			TestEqual(TEXT("PerceptionModifier"),                   AttributeSet->GetPerceptionModifier(),          0.0f);
			TestEqual(TEXT("SkAcrobaticsModifier"),                 AttributeSet->GetSkAcrobaticsModifier(),        0.0f);
			TestEqual(TEXT("SkArcanaModifier"),                     AttributeSet->GetSkArcanaModifier(),            0.0f);
			TestEqual(TEXT("SkAthleticsModifier"),                  AttributeSet->GetSkAthleticsModifier(),         0.0f);
			TestEqual(TEXT("SkCraftingModifier"),                   AttributeSet->GetSkCraftingModifier(),          0.0f);
			TestEqual(TEXT("SkDeceptionModifier"),                  AttributeSet->GetSkDeceptionModifier(),         0.0f);
			TestEqual(TEXT("SkDiplomacyModifier"),                  AttributeSet->GetSkDiplomacyModifier(),         0.0f);
			TestEqual(TEXT("SkIntimidationModifier"),               AttributeSet->GetSkIntimidationModifier(),      0.0f);
			TestEqual(TEXT("SkLore1Modifier"),                      AttributeSet->GetSkLore1Modifier(),             0.0f);
			TestEqual(TEXT("SkLore2Modifier"),                      AttributeSet->GetSkLore2Modifier(),             0.0f);
			TestEqual(TEXT("SkMedicineModifier"),                   AttributeSet->GetSkMedicineModifier(),          0.0f);
			TestEqual(TEXT("SkNatureModifier"),                     AttributeSet->GetSkNatureModifier(),            0.0f);
			TestEqual(TEXT("SkOccultismModifier"),                  AttributeSet->GetSkOccultismModifier(),         0.0f);
			TestEqual(TEXT("SkPerformanceModifier"),                AttributeSet->GetSkPerformanceModifier(),       0.0f);
			TestEqual(TEXT("SkReligionModifier"),                   AttributeSet->GetSkReligionModifier(),          0.0f);
			TestEqual(TEXT("SkSocietyModifier"),                    AttributeSet->GetSkSocietyModifier(),           0.0f);
			TestEqual(TEXT("SkStealthModifier"),                    AttributeSet->GetSkStealthModifier(),           0.0f);
			TestEqual(TEXT("SkSurvivalModifier"),                   AttributeSet->GetSkSurvivalModifier(),          0.0f);
			TestEqual(TEXT("SkThieveryModifier"),                   AttributeSet->GetSkThieveryModifier(),          0.0f);
			TestEqual(TEXT("SpellAttackRoll"),                      AttributeSet->GetSpellAttackRoll(),             0.0f);
			TestEqual(TEXT("SpellDifficultyClass"),                 AttributeSet->GetSpellDifficultyClass(),        0.0f);
			TestEqual(TEXT("FeAncestryFeatCount"),                  AttributeSet->GetFeAncestryFeatCount(),         0.0f);
			TestEqual(TEXT("FeAncestryFeatLimit"),                  AttributeSet->GetFeAncestryFeatLimit(),         0.0f);
			TestEqual(TEXT("EncActionPoints"),                      AttributeSet->GetEncActionPoints(),             0.0f);
			TestEqual(TEXT("EncMaxActionPoints"),                   AttributeSet->GetEncMaxActionPoints(),          0.0f);
			TestEqual(TEXT("EncReactionPoints"),                    AttributeSet->GetEncReactionPoints(),           0.0f);
			TestEqual(TEXT("EncMaxReactionPoints"),                 AttributeSet->GetEncMaxReactionPoints(),        0.0f);
			TestEqual(TEXT("EncMultipleAttackPenalty"),             AttributeSet->GetEncMultipleAttackPenalty(),    0.0f);
			TestEqual(TEXT("EncMaxMultipleAttackPenalty"),          AttributeSet->GetEncMaxMultipleAttackPenalty(), 0.0f);
			TestEqual(TEXT("TmpDamageIncoming"),                    AttributeSet->GetTmpDamageIncoming(),           0.0f);
			TestEqual(TEXT("TmpLastIncomingAttackDegreeOfSuccess"), AttributeSet->GetTmpDamageIncoming(),           0.0f);
		});
	});
}
