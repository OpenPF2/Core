// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//   - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//   - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//   - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
//
// Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
// as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
// file other than the material designated as Open Game Content may be reproduced in any form without written
// permission.

#pragma once

#include <AttributeSet.h>
#include <AbilitySystemComponent.h>

#include "Abilities/PF2AttributeSetMacros.h"

#include "PF2CharacterAttributeSet.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * This holds all of the attributes used by abilities. A copy of this is instantiated on every character.
 */
UCLASS()
class OPENPF2CORE_API UPF2CharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Static Methods
	// =================================================================================================================
	/**
	 * Gets the source tags from the given Gameplay Effect (GE) specification.
	 *
	 * @param EffectSpec
	 *	The specification from which source tags will be obtained.
	 *
	 * @return
	 *	The source tags.
	 */
	FORCEINLINE static const FGameplayTagContainer* GetSourceTags(const FGameplayEffectSpec& EffectSpec)
	{
		return EffectSpec.CapturedSourceTags.GetAggregatedTags();
	}

public:
	// =================================================================================================================
	// Attributes - Stats Shared by Both PCs and NPCs
	// =================================================================================================================
	// Experience ------------------------------------------------------------------------------------------------------
	/**
	 * Experience Points (XP) track the knowledge a character has earned from facing beasts and traps.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 8, "Defining Characteristics":
	 * "Every time a character amasses 1,000 XP, they go up a level, gaining new abilities so they can take on even
	 * greater challenges. A 1st-level PC might face off against a giant rat or a group of bandits, but at 20th level,
	 * that same character might be able to bring ruin to an entire city with a single spell."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 507, "Experience Points":
	 * "As characters adventure, they earn Experience Points (XP). These awards come from achieving goals, completing
	 * social encounters, exploring new places, fighting monsters, overcoming hazards, and other sorts of deeds.
	 * ...
	 * Normally, when a player character reaches 1,000 XP or more, they level up, reduce their XP by 1,000, and start
	 * progressing toward the next level."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Experience", ReplicatedUsing=OnRep_Experience)
	FGameplayAttributeData Experience;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, Experience)

	// Ability Scores --------------------------------------------------------------------------------------------------
	/**
	 * The number of ability boosts that this character currently has applied.
	 *
	 * Capped by AbBoostLimit.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbBoostCount)
	FGameplayAttributeData AbBoostCount;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbBoostCount)

	/**
	 * The limit on how many ability boosts this character can apply.
	 *
	 * This caps AbBoostCount, but can be a larger number than AbBoostCount if the character is eligible for additional
	 * ability boosts that the player or story (for NPCs) has not yet applied.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbBoostLimit)
	FGameplayAttributeData AbBoostLimit;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbBoostLimit)

	/**
	 * Strength measures a character’s physical power.
	 *
	 * Strength is important for characters who engage in hand-to-hand combat. The Strength modifier gets added to melee
	 * damage rolls and determines how much a character can carry. (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbStrength)
	FGameplayAttributeData AbStrength;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbStrength)

	/**
	 * Strength measures a character’s physical power.
	 *
	 * Strength is important for characters who engage in hand-to-hand combat. The Strength modifier gets added to melee
	 * damage rolls and determines how much a character can carry.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbStrengthModifier)
	FGameplayAttributeData AbStrengthModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbStrengthModifier)

	/**
	 * Dexterity measures a character’s agility, balance, and reflexes.
	 *
	 * Dexterity is important if a character makes attacks with ranged weapons or uses stealth to surprise others. The
	 * Dexterity modifier is also added to a character’s AC and Reflex saving throws.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbDexterity)
	FGameplayAttributeData AbDexterity;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbDexterity)

	/**
	 * Dexterity measures a character’s agility, balance, and reflexes.
	 *
	 * Dexterity is important if a character makes attacks with ranged weapons or uses stealth to surprise others. The
	 * Dexterity modifier is also added to a character’s AC and Reflex saving throws.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbDexterityModifier)
	FGameplayAttributeData AbDexterityModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbDexterityModifier)

	/**
	 * Constitution measures a character’s overall health and stamina.
	 *
	 * Constitution is an important statistic for all characters, especially those who fight in close combat. The
	 * Constitution modifier is added to its Hit Points and Fortitude saving throws.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbConstitution)
	FGameplayAttributeData AbConstitution;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbConstitution)

	/**
	 * Constitution measures a character’s overall health and stamina.
	 *
	 * Constitution is an important statistic for all characters, especially those who fight in close combat. The
	 * Constitution modifier is added to its Hit Points and Fortitude saving throws.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbConstitutionModifier)
	FGameplayAttributeData AbConstitutionModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbConstitutionModifier)

	/**
	 * Intelligence measures how well a character can learn and reason.
	 *
	 * A high Intelligence allows a character to analyze situations and understand patterns, and unlocks the ability to
	 * become trained in additional skills and the ability to master additional languages.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbIntelligence)
	FGameplayAttributeData AbIntelligence;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbIntelligence)

	/**
	 * Intelligence measures how well a character can learn and reason.
	 *
	 * A high Intelligence allows a character to analyze situations and understand patterns, and unlocks the ability to
	 * become trained in additional skills and the ability to master additional languages.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbIntelligenceModifier)
	FGameplayAttributeData AbIntelligenceModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbIntelligenceModifier)

	/**
	 * Wisdom measures a character’s common sense, awareness, and intuition.
	 *
	 * The Wisdom modifier is added to its Perception and Will saving throws.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbWisdom)
	FGameplayAttributeData AbWisdom;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbWisdom)

	/**
	 * Wisdom measures a character’s common sense, awareness, and intuition.
	 *
	 * The Wisdom modifier is added to its Perception and Will saving throws.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbWisdomModifier)
	FGameplayAttributeData AbWisdomModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbWisdomModifier)

	/**
	 * Charisma measures a character’s personal magnetism and strength of personality.
	 *
	 * A high Charisma score helps a character influence the thoughts and moods of others.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbCharisma)
	FGameplayAttributeData AbCharisma;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbCharisma)

	/**
	 * Charisma measures a character’s personal magnetism and strength of personality.
	 *
	 * A high Charisma score helps a character influence the thoughts and moods of others.
	 * (Pathfinder 2E Core Rulebook, page 19)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Ability Scores", ReplicatedUsing=OnRep_AbCharismaModifier)
	FGameplayAttributeData AbCharismaModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, AbCharismaModifier)

	// Class DC --------------------------------------------------------------------------------------------------------
	/**
	 * The Difficulty Class (DC) modifier for this character.
	 *
	 * This controls how hard or easy certain types of tasks are for this character.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Class DC", ReplicatedUsing=OnRep_ClassDifficultyClass)
	FGameplayAttributeData ClassDifficultyClass;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, ClassDifficultyClass)

	// Speed -----------------------------------------------------------------------------------------------------------
	/**
	 * How fast this character can move (in centimeters per second).
	 */
	UPROPERTY(BlueprintReadOnly, Category="Speed", ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, Speed)

	/**
	 * The maximum speed of this character (in centimeters per second).
	 */
	UPROPERTY(BlueprintReadOnly, Category="Speed", ReplicatedUsing = OnRep_MaxSpeed)
	FGameplayAttributeData MaxSpeed;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, MaxSpeed)

	// Reach -----------------------------------------------------------------------------------------------------------
	/**
	 * The distance (in centimeters) you can physically reach with your body or a weapon.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 455, "Range and Reach":
	 * "Reach is how far you can physically reach with your body or a weapon. Melee Strikes rely on reach. Your reach
	 * also creates an area around your space where other creatures could trigger your reactions. Your reach is
	 * typically [1.5 meters], but weapons with the reach trait can extend this."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Reach", ReplicatedUsing = OnRep_Reach)
	FGameplayAttributeData Reach;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, Reach)

	// Armor Class -----------------------------------------------------------------------------------------------------
	/**
	 * The Armor Class, which represents how hard it is to hit and damage a creature.
	 *
	 * This serves as the Difficulty Class for hitting a creature with an attack.
	 * (Pathfinder 2E Core Rulebook, page 12)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Armor Class", ReplicatedUsing = OnRep_ArmorClass)
	FGameplayAttributeData ArmorClass;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, ArmorClass)

	// Saving Throws ---------------------------------------------------------------------------------------------------
	/**
	 * Fortitude saving throws reduce the effects of abilities and afflictions that can debilitate the body.
	 * (Pathfinder 2E Core Rulebook, page 449)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Saving Throws", ReplicatedUsing = OnRep_StFortitudeModifier)
	FGameplayAttributeData StFortitudeModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, StFortitudeModifier)

	/**
	 * Reflex saving throws measure how quickly and gracefully a character responds to a situation.
	 * (Pathfinder 2E Core Rulebook, page 449)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Saving Throws", ReplicatedUsing = OnRep_StReflexModifier)
	FGameplayAttributeData StReflexModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, StReflexModifier)

	/**
	 * Will saving throws measure how well a character resists attacks to mind and spirit.
	 * (Pathfinder 2E Core Rulebook, page 449)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Saving Throws", ReplicatedUsing = OnRep_StWillModifier)
	FGameplayAttributeData StWillModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, StWillModifier)

	// Hit Points ------------------------------------------------------------------------------------------------------
	/**
	 * All creatures and objects have Hit Points (HP).
	 *
	 * From the Pathfinder 2E Core Rulebook, page 459, "Knocked Out and Dying":
	 * "Creatures cannot be reduced to fewer than 0 Hit Points. When most creatures reach 0 Hit Points, they die and are
	 * removed from play unless the attack was nonlethal, in which case they are instead knocked out for a significant
	 * amount of time (usually 1 minute or more).
	 *
	 * Player characters, their companions, and other significant characters and creatures don’t automatically die when
	 * they reach 0 Hit Points. Instead, they are knocked out and are at risk of death."
	 *
	 * Capped by MaxHitPoints.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_HitPoints)
	FGameplayAttributeData HitPoints;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, HitPoints)

	/**
	 * The maximum number of hit points for this character.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_MaxHitPoints)
	FGameplayAttributeData MaxHitPoints;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, MaxHitPoints)

	/**
	 * The character's resistance to Bludgeoning damage (DamageType.Physical.Bludgeoning).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Bludgeoning (B) damage comes from weapons and hazards that deal blunt-force trauma, like a hit from a club or
	 * being dashed against rocks."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstPhysicalBludgeoning)
	FGameplayAttributeData RstPhysicalBludgeoning;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstPhysicalBludgeoning)

	/**
	 * The character's resistance to Piercing damage (DamageType.Physical.Piercing).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Piercing (P) damage is dealt from stabs and punctures, whether from a dragon's fangs or the thrust of a spear."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstPhysicalPiercing)
	FGameplayAttributeData RstPhysicalPiercing;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstPhysicalPiercing)

	/**
	 * The character's resistance to Slashing damage (DamageType.Physical.Slashing).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Slashing (S) damage is delivered by a cut, be it the swing of the sword or the blow from a scythe blades trap."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstPhysicalSlashing)
	FGameplayAttributeData RstPhysicalSlashing;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstPhysicalSlashing)

	/**
	 * The character's resistance to Acid damage (DamageType.Energy.Acid).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Acid damage can be delivered by gases, liquids, and certain solids that dissolve flesh, and sometimes harder
	 * materials."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstEnergyAcid)
	FGameplayAttributeData RstEnergyAcid;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstEnergyAcid)

	/**
	 * The character's resistance to Cold damage (DamageType.Energy.Cold).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Cold damage freezes material by way of contact with chilling gases and ice. Electricity damage comes from the
	 * discharge of powerful lightning and sparks."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstEnergyCold)
	FGameplayAttributeData RstEnergyCold;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstEnergyCold)

	/**
	 * The character's resistance to Electricity damage (DamageType.Energy.Electricity).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Electricity comes from the discharge of powerful lightning and sparks."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstEnergyElectricity)
	FGameplayAttributeData RstEnergyElectricity;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstEnergyElectricity)

	/**
	 * The character's resistance to Fire damage (DamageType.Energy.Fire).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Fire damage burns through heat and combustion."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstEnergyFire)
	FGameplayAttributeData RstEnergyFire;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstEnergyFire)

	/**
	 * The character's resistance to Sonic damage (DamageType.Energy.Sonic).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Sonic damage assaults matter with high-frequency vibration and sound waves."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstEnergySonic)
	FGameplayAttributeData RstEnergySonic;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstEnergySonic)

	/**
	 * The character's resistance to Positive damage (DamageType.Energy.Positive).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Positive damage harms only undead creatures, withering undead bodies and disrupting incorporeal undead."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstEnergyPositive)
	FGameplayAttributeData RstEnergyPositive;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstEnergyPositive)

	/**
	 * The character's resistance to Negative damage (DamageType.Energy.Negative).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Negative damage saps life, damaging only living creatures."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstEnergyNegative)
	FGameplayAttributeData RstEnergyNegative;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstEnergyNegative)

	/**
	 * The character's resistance to Force damage (DamageType.Energy.Force).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Powerful and pure magical energy can manifest itself as force damage. Few things can resist this type of
	 * damage—not even incorporeal creatures such as ghosts and wraiths."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstEnergyForce)
	FGameplayAttributeData RstEnergyForce;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstEnergyForce)

	/**
	 * The character's resistance to Chaotic damage (DamageType.Alignment.Chaotic).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Chaotic damage harms only lawful creatures."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstAlignmentChaotic)
	FGameplayAttributeData RstAlignmentChaotic;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstAlignmentChaotic)

	/**
	 * The character's resistance to Evil damage (DamageType.Alignment.Evil).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Evil damage harms only good creatures."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstAlignmentEvil)
	FGameplayAttributeData RstAlignmentEvil;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstAlignmentEvil)

	/**
	 * The character's resistance to Good damage (DamageType.Alignment.Good).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Good damage harms only evil creatures."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstAlignmentGood)
	FGameplayAttributeData RstAlignmentGood;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstAlignmentGood)

	/**
	 * The character's resistance to Lawful damage (DamageType.Alignment.Lawful).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Lawful damage harms only chaotic creatures."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstAlignmentLawful)
	FGameplayAttributeData RstAlignmentLawful;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstAlignmentLawful)

	/**
	 * The character's resistance to Mental damage (DamageType.Mental).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Sometimes an effect can target the mind with enough psychic force to actually deal damage to the creature. When
	 * it does, it deals mental damage. Mindless creatures and those with only programmed or rudimentary intelligence
	 * are often immune to mental damage and effects."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstMental)
	FGameplayAttributeData RstMental;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstMental)

	/**
	 * The character's resistance to Poison damage (DamageType.Poison).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "Venoms, toxins and the like can deal poison damage, which affects creatures by way of contact, ingestion,
	 * inhalation, or injury. In addition to coming from monster attacks, alchemical items, and spells, poison damage is
	 * often caused by ongoing afflictions, which follow special rules."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstPoison)
	FGameplayAttributeData RstPoison;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstPoison)

	/**
	 * The character's resistance to Bleed damage (DamageType.Bleed).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "This is persistent damage that represents loss of blood. As such, it has no effect on nonliving creatures or
	 * living creatures that don't need blood to live."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstBleed)
	FGameplayAttributeData RstBleed;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstBleed)

	/**
	 * The character's resistance to Precision damage (DamageType.Precision).
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 452, "Damage Types":
	 * "When a character hits with an ability that grants precision damage, the character increase the attack's listed
	 * damage, using the same damage type, rather than tracking a separate pool of damage."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Hit Points", ReplicatedUsing=OnRep_RstPrecision)
	FGameplayAttributeData RstPrecision;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, RstPrecision)

	// Perception ------------------------------------------------------------------------------------------------------
	/**
	 * Perception measures a character's ability to notice hidden objects or unusual situations and affects initiative.
	 * (Pathfinder 2E Core Rulebook, page 448)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Perception", ReplicatedUsing = OnRep_PerceptionModifier)
	FGameplayAttributeData PerceptionModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, PerceptionModifier)

	// Skills ------------------------------------------------------------------------------------------------------
	/**
	 * Acrobatics measures a character's ability to perform tasks requiring coordination and grace.
	 * (Pathfinder 2E Core Rulebook, page 240)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkAcrobaticsModifier)
	FGameplayAttributeData SkAcrobaticsModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkAcrobaticsModifier)

	/**
	 * Arcana measures how much a character knows about arcane magic and creatures.
	 * (Pathfinder 2E Core Rulebook, page 241)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkArcanaModifier)
	FGameplayAttributeData SkArcanaModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkArcanaModifier)

	/**
	 * Athletics allows a character to perform deeds of physical prowess.
	 * (Pathfinder 2E Core Rulebook, page 241)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkAthleticsModifier)
	FGameplayAttributeData SkAthleticsModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkAthleticsModifier)

	/**
	 * Crafting allows a character to create and repair items.
	 * (Pathfinder 2E Core Rulebook, page 243)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkCraftingModifier)
	FGameplayAttributeData SkCraftingModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkCraftingModifier)

	/**
	 * Deception allows a character to trick and mislead others using disguises, lies, and other forms of subterfuge.
	 * (Pathfinder 2E Core Rulebook, page 245)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkDeceptionModifier)
	FGameplayAttributeData SkDeceptionModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkDeceptionModifier)

	/**
	 * Diplomacy allows a character to influence others through negotiation and flattery.
	 * (Pathfinder 2E Core Rulebook, page 245)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkDiplomacyModifier)
	FGameplayAttributeData SkDiplomacyModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkDiplomacyModifier)

	/**
	 * Intimidation allows a character to bend others to their will using threats.
	 * (Pathfinder 2E Core Rulebook, page 247)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkIntimidationModifier)
	FGameplayAttributeData SkIntimidationModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkIntimidationModifier)

	/**
	 * Lore gives a character specialized information on a narrow topic.
	 * (Pathfinder 2E Core Rulebook, page 247)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkLore1Modifier)
	FGameplayAttributeData SkLore1Modifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkLore1Modifier)

	/**
	 * Lore gives a character specialized information on a narrow topic.
	 * (Pathfinder 2E Core Rulebook, page 247)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkLore2Modifier)
	FGameplayAttributeData SkLore2Modifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkLore2Modifier)

	/**
	 * Medicine allows a character to patch up wounds and help people recover from diseases and poisons.
	 * (Pathfinder 2E Core Rulebook, page 248)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkMedicineModifier)
	FGameplayAttributeData SkMedicineModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkMedicineModifier)

	/**
	 * Nature gives a character knowledge about the natural world, including commanding and training animals and beasts.
	 * (Pathfinder 2E Core Rulebook, page 249)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkNatureModifier)
	FGameplayAttributeData SkNatureModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkNatureModifier)

	/**
	 * Occultism gives a character knowledge about ancient philosophies, esoteric lore, obscure mysticism, and
	 * supernatural creatures.
	 *
	 * (Pathfinder 2E Core Rulebook, page 249)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkOccultismModifier)
	FGameplayAttributeData SkOccultismModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkOccultismModifier)

	/**
	 * Performance gives a character skill impressing crowds by performing live.
	 * (Pathfinder 2E Core Rulebook, page 250)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkPerformanceModifier)
	FGameplayAttributeData SkPerformanceModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkPerformanceModifier)

	/**
	 * Religion gives a character knowledge of the secrets of deities, dogma, faith, and the realms of divine
	 * creatures -- both sublime and sinister. (Pathfinder 2E Core Rulebook, page 250)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkReligionModifier)
	FGameplayAttributeData SkReligionModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkReligionModifier)

	/**
	 * Society gives a character an understanding of the people and systems that make civilization run, including the
	 * historical events that make societies what they are today. (Pathfinder 2E Core Rulebook, page 250)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkSocietyModifier)
	FGameplayAttributeData SkSocietyModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkSocietyModifier)

	/**
	 * Stealth gives a character the ability to avoid detection, slip past foes, hide, and conceal items.
	 * (Pathfinder 2E Core Rulebook, page 251)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkStealthModifier)
	FGameplayAttributeData SkStealthModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkStealthModifier)

	/**
	 * Survival gives a character aptitude to live in the wilderness, foraging for food, and building shelter.
	 * (Pathfinder 2E Core Rulebook, page 252)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkSurvivalModifier)
	FGameplayAttributeData SkSurvivalModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkSurvivalModifier)

	/**
	 * Thievery gives a character training in the particular set of skills favored by thieves and miscreants.
	 * (Pathfinder 2E Core Rulebook, page 253)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Skills", ReplicatedUsing = OnRep_SkThieveryModifier)
	FGameplayAttributeData SkThieveryModifier;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SkThieveryModifier)

	/**
	 * A measure of how potent a character's spells are against the defenses of other creatures.
	 * (Pathfinder 2E Core Rulebook, "Spell Attack Roll and Spell DC", page 298)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Spells", ReplicatedUsing=OnRep_SpellAttackRoll)
	FGameplayAttributeData SpellAttackRoll;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SpellAttackRoll)

	/**
	 * How hard it is to resist a character's spells with saving throws, or to counteract them.
	 * (Pathfinder 2E Core Rulebook, page 636)
	 */
	UPROPERTY(BlueprintReadOnly, Category="Spells", ReplicatedUsing=OnRep_SpellDifficultyClass)
	FGameplayAttributeData SpellDifficultyClass;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, SpellDifficultyClass)

	// Feats -----------------------------------------------------------------------------------------------------------
	/**
	 * The number of ancestry feats that this character currently has applied.
	 *
	 * Capped by FeAncestryFeatLimit.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Feats", ReplicatedUsing=OnRep_FeAncestryFeatCount)
	FGameplayAttributeData FeAncestryFeatCount;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, FeAncestryFeatCount)

	/**
	 * The limit on how many ancestry feats this character can apply.
	 *
	 * This caps FeAncestryFeatCount, but can be a larger number than FeAncestryFeatCount if the character is eligible
	 * for additional ancestry feats that the player or story (for NPCs) has not yet applied.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Feats", ReplicatedUsing=OnRep_FeAncestryFeatLimit)
	FGameplayAttributeData FeAncestryFeatLimit;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, FeAncestryFeatLimit)

	// Encounters ------------------------------------------------------------------------------------------------------
	/**
	 * The number of action points this character has available in the current encounter.
	 *
	 * The action points get automatically reset to 3 at the start of this character's next turn.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 468-469, "Step 1: Start Your Turn":
	 * "The last step of starting your turn is always the same.
	 * Regain your 3 actions and 1 reaction. If you haven't spent your reaction from your last turn, you lose it—you
	 * can't "save" actions or reactions from one turn to use during the next turn."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Encounters", ReplicatedUsing=OnRep_EncActionPoints)
	FGameplayAttributeData EncActionPoints;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, EncActionPoints)

	/**
	 * The maximum number of action points a character can have at any time during an encounter.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Encounters", ReplicatedUsing=OnRep_EncMaxActionPoints)
	FGameplayAttributeData EncMaxActionPoints;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, EncMaxActionPoints)

	/**
	 * The number of reaction points this character has available in the current encounter.
	 *
	 * Reaction points get automatically reset to 1 at the start of this character's next turn.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 468-469, "Step 1: Start Your Turn":
	 * "Many things happen automatically at the start of your turn...
	 *
	 * You can use 1 [...] reaction with a trigger of “Your turn begins” or something similar."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Encounters", ReplicatedUsing=OnRep_EncReactionPoints)
	FGameplayAttributeData EncReactionPoints;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, EncReactionPoints)

	/**
	 * The maximum number of reaction points a character can have at any time during an encounter.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Encounters", ReplicatedUsing=OnRep_EncMaxReactionPoints)
	FGameplayAttributeData EncMaxReactionPoints;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, EncMaxReactionPoints)

	/**
	 * The current multiple attack penalty applied to this character.
	 *
	 * This should be either zero or a negative value.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 446, "Multiple Attack Penalty":
	 * "The more attacks you make beyond your first in a single turn, the less accurate you become, represented by the
	 * multiple attack penalty. The second time you use an attack action during your turn, you take a –5 penalty to your
	 * attack roll. The third time you attack, and on any subsequent attacks, you take a –10 penalty to your attack
	 * roll. Every check that has the attack trait counts toward your multiple attack penalty, including Strikes, spell
	 * attack rolls, certain skill actions like Shove, and many others. Some weapons and abilities reduce multiple
	 * attack penalties, such as agile weapons, which reduce these penalties to –4 on the second attack or –8 on further
	 * attacks."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Encounters", ReplicatedUsing=OnRep_EncMultipleAttackPenalty)
	FGameplayAttributeData EncMultipleAttackPenalty;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, EncMultipleAttackPenalty)

	/**
	 * The maximum multiple attack penalty that can be applied to this character.
	 *
	 * This should be a negative value that is greater (in magnitude) than zero. For example, -10, -8, etc. This value
	 * clamps how negative the multiple attack penalty stat can get during a single encounter turn.
	 *
	 * From the Pathfinder 2E Core Rulebook, Chapter 9, page 446, "Multiple Attack Penalty":
	 * "The more attacks you make beyond your first in a single turn, the less accurate you become, represented by the
	 * multiple attack penalty. The second time you use an attack action during your turn, you take a –5 penalty to your
	 * attack roll. The third time you attack, and on any subsequent attacks, you take a –10 penalty to your attack
	 * roll. Every check that has the attack trait counts toward your multiple attack penalty, including Strikes, spell
	 * attack rolls, certain skill actions like Shove, and many others. Some weapons and abilities reduce multiple
	 * attack penalties, such as agile weapons, which reduce these penalties to –4 on the second attack or –8 on further
	 * attacks."
	 */
	UPROPERTY(BlueprintReadOnly, Category="Encounters", ReplicatedUsing=OnRep_EncMaxMultipleAttackPenalty)
	FGameplayAttributeData EncMaxMultipleAttackPenalty;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, EncMaxMultipleAttackPenalty)

	// Transient/Temporary Attributes ----------------------------------------------------------------------------------
	/**
	 * A temporary attribute for tracking damage that the owner of this set is receiving from an instant damage GE.
	 *
	 * This value exists only on the server; it is not replicated. At the end of execution for a damage GE, this turns
	 * into -HitPoints (i.e., it gets subtracted from the character's health). This allows other effects (e.g., passive
	 * protection effects or armor) to lessen the impact of the damage.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Temporary Attributes")
	FGameplayAttributeData TmpDamageIncoming;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, TmpDamageIncoming)

	/**
	 * A temporary attribute for tracking the result of the last damage roll against the owner of this set.
	 *
	 * This value exists only on the server; it is not replicated. It exists only so that other effects can react
	 * accordingly to the outcome.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Temporary Attributes")
	FGameplayAttributeData TmpLastIncomingAttackDegreeOfSuccess;
	ATTRIBUTE_ACCESSORS(UPF2CharacterAttributeSet, TmpLastIncomingAttackDegreeOfSuccess)

	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	explicit UPF2CharacterAttributeSet();

	// =================================================================================================================
	// Public Methods - UAttributeSet Overrides
	// =================================================================================================================
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// =================================================================================================================
	// Public Methods - Replication Callbacks
	// =================================================================================================================
	// These exist to make sure that the ability system internal representations are synchronized properly during
	// replication.
	UFUNCTION()
    virtual void OnRep_Experience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AbBoostCount(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AbBoostLimit(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AbStrength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AbStrengthModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_AbDexterity(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AbDexterityModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_AbConstitution(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AbConstitutionModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_AbIntelligence(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AbIntelligenceModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_AbWisdom(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AbWisdomModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_AbCharisma(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AbCharismaModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ClassDifficultyClass(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Speed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Reach(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ArmorClass(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_StFortitudeModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_StReflexModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_StWillModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_HitPoints(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHitPoints(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstPhysicalBludgeoning(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstPhysicalPiercing(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstPhysicalSlashing(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstEnergyAcid(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstEnergyCold(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstEnergyElectricity(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstEnergyFire(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstEnergySonic(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstEnergyPositive(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstEnergyNegative(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstEnergyForce(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstAlignmentChaotic(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstAlignmentEvil(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstAlignmentGood(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstAlignmentLawful(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstMental(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstPoison(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstBleed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RstPrecision(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_PerceptionModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkAcrobaticsModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkArcanaModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkAthleticsModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkCraftingModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkDeceptionModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkDiplomacyModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkIntimidationModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkLore1Modifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkLore2Modifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkMedicineModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkNatureModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkOccultismModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkPerformanceModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkReligionModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkSocietyModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkStealthModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkSurvivalModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_SkThieveryModifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_FeAncestryFeatCount(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_FeAncestryFeatLimit(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SpellAttackRoll(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SpellDifficultyClass(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_EncActionPoints(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_EncMaxActionPoints(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_EncReactionPoints(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_EncMaxReactionPoints(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_EncMultipleAttackPenalty(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_EncMaxMultipleAttackPenalty(const FGameplayAttributeData& OldValue);

protected:
	// =================================================================================================================
	// Protected Constants
	// =================================================================================================================
	/**
	 * The name of the gameplay tag for events to notify passive condition check GAs about incoming damage.
	 */
	inline static const FName DamageReceivedEventTagName = TEXT("GameplayAbility.GameplayEvent.DamageReceived");

	/**
	 * The name of the gameplay tag for events to notify passive condition check GAs about changes in hit points.
	 */
	inline static const FName HitPointsChangedEventTagName = TEXT("GameplayAbility.GameplayEvent.HitPointsChanged");

	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The gameplay tag for events to notify passive condition check GAs about changes in hit points.
	 */
	FGameplayTag DamageReceivedEventTag;

	/**
	 * The gameplay tag for events to notify passive condition check GAs about changes in hit points.
	 */
	FGameplayTag HitPointsChangedEventTag;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Notifies the ASC of the target about a change to an attribute.
	 *
	 * This is typically used to enable passive condition check GAs an opportunity to update condition statuses in
	 * response to changes in stats.
	 *
	 * @param EventTag
	 *	The unique gameplay tag that corresponds to the desired event type.
	 * @param EventMagnitude
	 *	The delta for which an event is being communicated (e.g., the amount of damage received or the amount of health
	 *	lost or gained).
	 * @param TargetCharacter
	 *	The character receiving the hit point change. This is usually the same as the character who owns this ASC.
	 * @param SourceEffectSpec
	 *	Specifications and context about the original gameplay effect execution that prompted this event to be emitted.
	 */
	void EmitGameplayEvent(const FGameplayTag&        EventTag,
	                       float                      EventMagnitude,
	                       IPF2CharacterInterface*    TargetCharacter,
	                       const FGameplayEffectSpec& SourceEffectSpec) const;

	// =================================================================================================================
	// Protected Native Event Callbacks
	// =================================================================================================================
	/**
	 * Notifies this ASC that the incoming damage attribute has been changed (typically by a Gameplay Effect).
	 *
	 * This is called after the change has already occurred. This applies damage to the target, resets the incoming
	 * damage to zero, and then dispatches appropriate damage notifications to the character.
	 *
	 * @param SourceEffectSpec
	 *	Specifications and context about the gameplay effect activation that applied the incoming damage.
	 * @param TargetCharacter
	 *	The character receiving the damage. This is usually the same as the character who owns this ASC.
	 */
	void Native_OnDamageIncomingChanged(const FGameplayEffectSpec&   SourceEffectSpec,
	                                    IPF2CharacterInterface*      TargetCharacter);

	/**
	 * Notifies this ASC that the hit points attribute has been changed (typically by a Gameplay Effect).
	 *
	 * This is called after the change has already occurred. This clamps the value to the allowed range and then
	 * dispatches appropriate damage notifications to the character.
	 *
	 * @param SourceEffectSpec
	 *	Specifications and context about the gameplay effect activation that adjusted the hit points.
	 * @param TargetCharacter
	 *	The character receiving the hit point change. This is usually the same as the character who owns this ASC.
	 * @param ValueDelta
	 *	The amount of the change.
	 */
	void Native_OnHitPointsChanged(const FGameplayEffectSpec& SourceEffectSpec,
	                               IPF2CharacterInterface*    TargetCharacter,
	                               const float                ValueDelta);

	/**
	 * Notifies this ASC that the speed attribute has been changed (typically by a Gameplay Effect).
	 *
	 * This is called after the change has already occurred. This clamps the value to the allowed range and then
	 * dispatches appropriate damage notifications to the character.
	 *
	 * @param SourceEffectSpec
	 *	Specifications and context about the gameplay effect activation that adjusted the speed.
	 * @param TargetCharacter
	 *	The character receiving the speed change. This is usually the same as the character who owns this ASC.
	 * @param ValueDelta
	 *	The amount of the change.
	 */
	void Native_OnSpeedChanged(const FGameplayEffectSpec& SourceEffectSpec,
	                           IPF2CharacterInterface*    TargetCharacter,
	                           const float                ValueDelta);

	/**
	 * Notifies this ASC that the multiple attack penalty attribute has been changed (typically by a Gameplay Effect).
	 *
	 * This is called after the change has already occurred. This clamps the value to the allowed range.
	 *
	 * @param SourceEffectSpec
	 *	Specifications and context about the gameplay effect activation that adjusted the multiple attack penalty.
	 * @param TargetCharacter
	 *	The character receiving the multiple attack penalty change. This is usually the same as the character who owns
	 *	this ASC.
	 * @param ValueDelta
	 *	The amount of the change.
	 */
	void Native_OnMultipleAttackPenaltyChanged(const FGameplayEffectSpec&    SourceEffectSpec,
	                                           const IPF2CharacterInterface* TargetCharacter,
	                                           const float                   ValueDelta);

};
