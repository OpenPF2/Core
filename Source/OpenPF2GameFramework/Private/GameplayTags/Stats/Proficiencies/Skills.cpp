//  OpenPF2 Game Framework for Unreal Engine, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
//  Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//    - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//    - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//    - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
//
//  Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
//  as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
//  file other than the material designated as Open Game Content may be reproduced in any form without written
//  permission.

#pragma once

#include "GameplayTags/Stats/Proficiencies/Skills.h"

// =====================================================================================================================
// Skills - The character’s training and experience at performing certain tasks.
// =====================================================================================================================
// Sources:
//   - Pathfinder 2E Core Rulebook, Chapter 1: "Introduction", page 11, "Playing the Game".
//   - Pathfinder 2E Core Rulebook, Chapter 1: "Introduction", page 13, "Proficiency".
//   - Pathfinder 2E Core Rulebook, Chapter 4: "Skills", pages 233-253.
//   - Pathfinder 2E Core Rulebook, Appendix, page 624, "Skills".

// === Acrobatics
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillAcrobaticsUntrained,
    "Skill.Acrobatics.Untrained",
    "Character is Untrained in Acrobatics."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillAcrobaticsTrained,
    "Skill.Acrobatics.Trained",
    "Character is Trained in Acrobatics."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillAcrobaticsExpert,
    "Skill.Acrobatics.Expert",
    "Character is Expert in Acrobatics."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillAcrobaticsMaster,
    "Skill.Acrobatics.Master",
    "Character is Master in Acrobatics."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillAcrobaticsLegendary,
    "Skill.Acrobatics.Legendary",
    "Character is Legendary in Acrobatics."
)

// === Arcana
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillArcanaUntrained,
    "Skill.Arcana.Untrained",
    "Character is Untrained in Arcana."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillArcanaTrained,
    "Skill.Arcana.Trained",
    "Character is Trained in Arcana."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillArcanaExpert,
    "Skill.Arcana.Expert",
    "Character is Expert in Arcana."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillArcanaMaster,
    "Skill.Arcana.Master",
    "Character is Master in Arcana."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillArcanaLegendary,
    "Skill.Arcana.Legendary",
    "Character is Legendary in Arcana."
)

// === Athletics
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillAthleticsUntrained,
    "Skill.Athletics.Untrained",
    "Character is Untrained in Athletics."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillAthleticsTrained,
    "Skill.Athletics.Trained",
    "Character is Trained in Athletics."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillAthleticsExpert,
    "Skill.Athletics.Expert",
    "Character is Expert in Athletics."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillAthleticsMaster,
    "Skill.Athletics.Master",
    "Character is Master in Athletics."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillAthleticsLegendary,
    "Skill.Athletics.Legendary",
    "Character is Legendary in Athletics."
)

// === Crafting
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillCraftingUntrained,
    "Skill.Crafting.Untrained",
    "Character is Untrained in Crafting."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillCraftingTrained,
    "Skill.Crafting.Trained",
    "Character is Trained in Crafting."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillCraftingExpert,
    "Skill.Crafting.Expert",
    "Character is Expert in Crafting."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillCraftingMaster,
    "Skill.Crafting.Master",
    "Character is Master in Crafting."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillCraftingLegendary,
    "Skill.Crafting.Legendary",
    "Character is Legendary in Crafting."
)

// === Deception
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillDeceptionUntrained,
    "Skill.Deception.Untrained",
    "Character is Untrained in Deception."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillDeceptionTrained,
    "Skill.Deception.Trained",
    "Character is Trained in Deception."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillDeceptionExpert,
    "Skill.Deception.Expert",
    "Character is Expert in Deception."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillDeceptionMaster,
    "Skill.Deception.Master",
    "Character is Master in Deception."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillDeceptionLegendary,
    "Skill.Deception.Legendary",
    "Character is Legendary in Deception."
)

// === Diplomacy
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillDiplomacyUntrained,
    "Skill.Diplomacy.Untrained",
    "Character is Untrained in Diplomacy."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillDiplomacyTrained,
    "Skill.Diplomacy.Trained",
    "Character is Trained in Diplomacy."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillDiplomacyExpert,
    "Skill.Diplomacy.Expert",
    "Character is Expert in Diplomacy."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillDiplomacyMaster,
    "Skill.Diplomacy.Master",
    "Character is Master in Diplomacy."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillDiplomacyLegendary,
    "Skill.Diplomacy.Legendary",
    "Character is Legendary in Diplomacy."
)

// === Intimidation
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillIntimidationUntrained,
    "Skill.Intimidation.Untrained",
    "Character is Untrained in Intimidation."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillIntimidationTrained,
    "Skill.Intimidation.Trained",
    "Character is Trained in Intimidation."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillIntimidationExpert,
    "Skill.Intimidation.Expert",
    "Character is Expert in Intimidation."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillIntimidationMaster,
    "Skill.Intimidation.Master",
    "Character is Master in Intimidation."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillIntimidationLegendary,
    "Skill.Intimidation.Legendary",
    "Character is Legendary in Intimidation."
)

// === Lore1
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillLore1Untrained,
    "Skill.Lore1.Untrained",
    "Character is Untrained in Lore1."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillLore1Trained,
    "Skill.Lore1.Trained",
    "Character is Trained in Lore1."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillLore1Expert,
    "Skill.Lore1.Expert",
    "Character is Expert in Lore1."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillLore1Master,
    "Skill.Lore1.Master",
    "Character is Master in Lore1."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillLore1Legendary,
    "Skill.Lore1.Legendary",
    "Character is Legendary in Lore1."
)

// === Lore2
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillLore2Untrained,
    "Skill.Lore2.Untrained",
    "Character is Untrained in Lore2."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillLore2Trained,
    "Skill.Lore2.Trained",
    "Character is Trained in Lore2."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillLore2Expert,
    "Skill.Lore2.Expert",
    "Character is Expert in Lore2."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillLore2Master,
    "Skill.Lore2.Master",
    "Character is Master in Lore2."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillLore2Legendary,
    "Skill.Lore2.Legendary",
    "Character is Legendary in Lore2."
)

// === Medicine
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillMedicineUntrained,
    "Skill.Medicine.Untrained",
    "Character is Untrained in Medicine."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillMedicineTrained,
    "Skill.Medicine.Trained",
    "Character is Trained in Medicine."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillMedicineExpert,
    "Skill.Medicine.Expert",
    "Character is Expert in Medicine."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillMedicineMaster,
    "Skill.Medicine.Master",
    "Character is Master in Medicine."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillMedicineLegendary,
    "Skill.Medicine.Legendary",
    "Character is Legendary in Medicine."
)

// === Nature
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillNatureUntrained,
    "Skill.Nature.Untrained",
    "Character is Untrained in Nature."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillNatureTrained,
    "Skill.Nature.Trained",
    "Character is Trained in Nature."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillNatureExpert,
    "Skill.Nature.Expert",
    "Character is Expert in Nature."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillNatureMaster,
    "Skill.Nature.Master",
    "Character is Master in Nature."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillNatureLegendary,
    "Skill.Nature.Legendary",
    "Character is Legendary in Nature."
)

// === Occultism
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillOccultismUntrained,
    "Skill.Occultism.Untrained",
    "Character is Untrained in Occultism."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillOccultismTrained,
    "Skill.Occultism.Trained",
    "Character is Trained in Occultism."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillOccultismExpert,
    "Skill.Occultism.Expert",
    "Character is Expert in Occultism."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillOccultismMaster,
    "Skill.Occultism.Master",
    "Character is Master in Occultism."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillOccultismLegendary,
    "Skill.Occultism.Legendary",
    "Character is Legendary in Occultism."
)

// === Performance
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillPerformanceUntrained,
    "Skill.Performance.Untrained",
    "Character is Untrained in Performance."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillPerformanceTrained,
    "Skill.Performance.Trained",
    "Character is Trained in Performance."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillPerformanceExpert,
    "Skill.Performance.Expert",
    "Character is Expert in Performance."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillPerformanceMaster,
    "Skill.Performance.Master",
    "Character is Master in Performance."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillPerformanceLegendary,
    "Skill.Performance.Legendary",
    "Character is Legendary in Performance."
)

// === Religion
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillReligionUntrained,
    "Skill.Religion.Untrained",
    "Character is Untrained in Religion."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillReligionTrained,
    "Skill.Religion.Trained",
    "Character is Trained in Religion."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillReligionExpert,
    "Skill.Religion.Expert",
    "Character is Expert in Religion."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillReligionMaster,
    "Skill.Religion.Master",
    "Character is Master in Religion."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillReligionLegendary,
    "Skill.Religion.Legendary",
    "Character is Legendary in Religion."
)

// === Society
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillSocietyUntrained,
    "Skill.Society.Untrained",
    "Character is Untrained in Society."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillSocietyTrained,
    "Skill.Society.Trained",
    "Character is Trained in Society."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillSocietyExpert,
    "Skill.Society.Expert",
    "Character is Expert in Society."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillSocietyMaster,
    "Skill.Society.Master",
    "Character is Master in Society."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillSocietyLegendary,
    "Skill.Society.Legendary",
    "Character is Legendary in Society."
)

// === Stealth
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillStealthUntrained,
    "Skill.Stealth.Untrained",
    "Character is Untrained in Stealth."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillStealthTrained,
    "Skill.Stealth.Trained",
    "Character is Trained in Stealth."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillStealthExpert,
    "Skill.Stealth.Expert",
    "Character is Expert in Stealth."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillStealthMaster,
    "Skill.Stealth.Master",
    "Character is Master in Stealth."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillStealthLegendary,
    "Skill.Stealth.Legendary",
    "Character is Legendary in Stealth."
)

// === Survival
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillSurvivalUntrained,
    "Skill.Survival.Untrained",
    "Character is Untrained in Survival."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillSurvivalTrained,
    "Skill.Survival.Trained",
    "Character is Trained in Survival."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillSurvivalExpert,
    "Skill.Survival.Expert",
    "Character is Expert in Survival."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillSurvivalMaster,
    "Skill.Survival.Master",
    "Character is Master in Survival."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillSurvivalLegendary,
    "Skill.Survival.Legendary",
    "Character is Legendary in Survival."
)

// === Thievery
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillThieveryUntrained,
    "Skill.Thievery.Untrained",
    "Character is Untrained in Thievery."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillThieveryTrained,
    "Skill.Thievery.Trained",
    "Character is Trained in Thievery."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillThieveryExpert,
    "Skill.Thievery.Expert",
    "Character is Expert in Thievery."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillThieveryMaster,
    "Skill.Thievery.Master",
    "Character is Master in Thievery."
)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    Pf2TagSkillThieveryLegendary,
    "Skill.Thievery.Legendary",
    "Character is Legendary in Thievery."
)
