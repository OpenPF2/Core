// OpenPF2 for UE Game Logic, Copyright 2021, Guy Elsmore-Paddock. All Rights Reserved.
//
// Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//   - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//   - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//   - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
// Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
// as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
// file other than the material designated as Open Game Content may be reproduced in any form without written
// permission.

#pragma once

#include <GameFramework/Character.h>
#include <AbilitySystemInterface.h>
#include <UObject/ConstructorHelpers.h>

#include "Abilities/PF2AbilityAttributes.h"
#include "Abilities/PF2AbilitySystemComponent.h"
#include "Abilities/PF2AttributeSet.h"
#include "PF2AncestryAndHeritageGameplayEffectBase.h"
#include "PF2BackgroundGameplayEffectBase.h"
#include "PF2CharacterAbilityBoostCount.h"
#include "PF2CharacterConstants.h"
#include "PF2ClassGameplayEffectBase.h"
#include "PF2CharacterBase.generated.h"

// Forward declaration; this is defined at the end of the file.
template<class AscType, class AttributeSetType>
class TPF2CharacterComponentFactory;

/**
 * Base class for both playable and non-playable characters in OpenPF2.
 *
 * PF2-based games must extend this class if they have custom character attributes or abilities.
 */
UCLASS()
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2CharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for APF2CharacterBase.
	 */
	explicit APF2CharacterBase();

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods - IAbilitySystemInterface Implementation
	// =================================================================================================================
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// =================================================================================================================
	// Public Methods - Blueprint Callable
	// =================================================================================================================
	/**
	 * Gets the current level of this character.
	 *
	 * The character level impacts the character's stats and how many boosts and feats the character can have.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 31, "Leveling Up":
	 * "Each level grants greater skill, increased resiliency, and new capabilities, allowing your character to face
	 * even greater challenges and go on to earn even more impressive rewards."
	 */
	UFUNCTION(BlueprintCallable)
	virtual int32 GetCharacterLevel() const;

	/**
	 * Sets the current level of this character.
	 *
	 * The character level impacts the character's stats and how many boosts and feats the character can have.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 31, "Leveling Up":
	 * "Each level grants greater skill, increased resiliency, and new capabilities, allowing your character to face
	 * even greater challenges and go on to earn even more impressive rewards."
	 *
	 * @param NewLevel
	 *	The new level for the character. Must be greater than 0.
	 *
	 * @return
	 *	true if the level was valid and changed; or, false, if the level was either invalid or did not change (the
	 *	character was already the specified level).
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool SetCharacterLevel(int32 NewLevel);

protected:
	// =================================================================================================================
	// Protected Properties - Blueprint Accessible
	// =================================================================================================================
	/**
	 * The Ability System Component (ASC) used for interfacing this character with the Gameplay Abilities System (GAS).
	 */
	UPROPERTY()
	UPF2AbilitySystemComponent* AbilitySystemComponent;

	/**
	 * The attributes of this character.
	 */
	UPROPERTY()
	UPF2AttributeSet* AttributeSet;

	/**
	 * The human-friendly name of this character.
	 *
	 * For playable characters, this may come from the player themselves unless the story dictates that the character
	 * has a specific name because it is important for the plot.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Character")
	FText CharacterName;

	/**
	 * The current level of this character.
	 */
	UPROPERTY(EditAnywhere, Replicated, meta=(ClampMin=1), Category="Character")
	int32 CharacterLevel;

	/**
	 * The ancestry and heritage of this character.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 33:
	 * "A character has one ancestry and one background, both of which you select during character creation ...
	 * Ancestries express the culture your character hails from. Within many ancestries are heritages—subgroups that
	 * each have their own characteristics. An ancestry provides ability boosts (and perhaps ability flaws), Hit Points,
	 * ancestry feats, and sometimes additional abilities."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 33, "Heritages":
	 * "You select a heritage at 1st level to reflect abilities passed down to you from your ancestors or common among
	 * those of your ancestry in the environment where you were born or grew up. You have only one heritage and can’t
	 * change it later. A heritage is not the same as a culture or ethnicity, though some cultures or ethnicities might
	 * have more or fewer members from a particular heritage."
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Character")
	TSubclassOf<UPF2AncestryAndHeritageGameplayEffectBase> AncestryAndHeritage;

	/**
	 * The background of this character.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 33:
	 * "Backgrounds ... describe training or environments your character experienced before becoming an adventurer. Your
	 * character’s background provides ability boosts, skill training, and a skill feat."
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Character")
	TSubclassOf<UPF2BackgroundGameplayEffectBase> Background;

	/**
	 * The class of this character.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 67:
	 * "Just as your character’s ancestry plays a key role in expressing their identity and worldview, their class
	 * indicates the training they have and will improve upon as an adventurer. Choosing your character’s class is
	 * perhaps the most important decision you will make for them. Groups of players often create characters whose
	 * skills and abilities complement each other mechanically—for example, ensuring your party includes a healer, a
	 * combat-oriented character, a stealthy character, and someone with command over magic—so you may wish to discuss
	 * options with your group before deciding."
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Character")
	TSubclassOf<UPF2ClassGameplayEffectBase> Class;

	/**
	 * The alignment of this character.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 28:
	 * "Your character’s alignment is an indicator of their morality and personality. There are nine possible alignments
	 * in Pathfinder, as shown on Table 1–2: The Nine Alignments. If your alignment has any components other than
	 * neutral, your character gains the traits of those alignment components. This might affect the way various spells,
	 * items, and creatures interact with your character.
	 *
	 * Your character’s alignment is measured by two pairs of opposed values: the axis of good and evil and the axis of
	 * law and chaos. A character who isn’t committed strongly to either side is neutral on that axis. Keep in mind that
	 * alignment is a complicated subject, and even acts that might be considered good can be used for nefarious
	 * purposes, and vice versa."
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="CreatureAlignment"), Category="Character")
	FGameplayTag Alignment;

	/**
	 * Additional languages that this character has learned beyond those known at 1st level.
	 *
	 * Languages granted by ancestry do not need to be duplicated here.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="Language"), Category="Character")
	FGameplayTagContainer AdditionalLanguages;

	/**
	 * Ability boosts manually applied by a game designer or player to this character's abilities.
	 *
	 * Because game rules tend to be very complex, no validation is done to ensure that the kind and number of boosts
	 * you apply here are legal. Therefore, do not apply more boosts than the boost cap at the character's current
	 * level, and pay close attention to ancestry, class, and other gameplay rules and restrictions to avoid creating a
	 * character that is overpowered. In addition, the character's key ability boost (as specified by their class) is
	 * applied automatically, so you should not add an additional boost for the character's key ability.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 20, "Ability Score Overview":
	 * "Each ability score starts at 10, representing human average, but as you make character choices, you’ll adjust
	 * these scores by applying ability boosts, which increase a score..."
	 *
	 * "Each ancestry provides ability boosts ... Your character’s background provides two ability boosts. Your
	 * character’s class provides an ability boost to the ability score most important to your class, called your key
	 * ability score."
	 *
	 * "When your character receives an ability boost, the rules indicate whether it must be applied to a specific
	 * ability score or to one of two specific ability scores, or whether it is a “free” ability boost that can be
	 * applied to any ability score of your choice. However, when you gain multiple ability boosts at the same time,
	 * you must apply each one to a different score."
	 */
	UPROPERTY(EditAnywhere, EditFixedSize, meta=(EditFixedOrder), Category="Ability Scores")
	TArray<FPF2CharacterAbilityBoostCount> AdditionalAbilityBoosts;

	/**
	 * The name(s) of lore sub-categories that this character is knowledgeable about.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Skills")
	TArray<FString> LoreNames;

	/**
	 * Proficiency ranks manually applied by a game designer or player to this character's skills.
	 *
	 * Higher proficiencies override lower proficiencies, so if a character is, for example, "Master" in "Survival"
	 * skill, you only need to select "Skill.Survival.Master", and do not need to also select the lower proficiency tags
	 * of "Skill.Survival.Trained" and "Skill.Survival.Untrained".
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="Skill"), Category="Skills")
	FGameplayTagContainer AdditionalSkillProficiencies;

	/**
	 * The Gameplay Effects used to boost abilities.
	 */
	UPROPERTY()
	TMap<FString, TSubclassOf<UGameplayEffect>> AbilityBoostEffects;

	/**
	 * Additional Gameplay Effects (GEs) that are always passively applied to the character, not dependent on the
	 * environment or activated abilities.
	 *
	 * This list is combined with the full list of passive GEs that are active on this character. GEs are automatically
	 * added to apply calculations for the other values that are set on this character (e.g. ancestry, class, boosts,
	 * etc); you should not add GEs for these "managed" passive GEs.
	 *
	 * Core GEs that initialize base character stats are evaluated before these additional GEs, while Core GEs that
	 * depend on ability scores (e.g. ability modifier calculation) are evaluated after these additional GEs.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character")
	TArray<TSubclassOf<UGameplayEffect>> AdditionalPassiveGameplayEffects;

	/**
	 * Whether or not managed passive Gameplay Effects have been generated for this character.
	 */
	UPROPERTY()
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	int32 bManagedPassiveEffectsGenerated;

	/**
	 * Whether or not passive Gameplay Effects have been activated on this character.
	 */
	UPROPERTY()
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	int32 bPassiveEffectsActivated;

	/**
	 * The core Gameplay Effects that drive stats for every character.
	 */
	TMultiMap<int32, TSubclassOf<UGameplayEffect>> CoreGameplayEffects;

	/**
	 * The list of passive Gameplay Effects (GEs) that are generated from other values specified on this character.
	 *
	 * Each value is a gameplay effect and the key is the weight of that GE. The weight controls the order that all GEs
	 * are applied. Lower weights are applied earlier than higher weights.
	 */
	TMultiMap<int32, TSubclassOf<UGameplayEffect>> ManagedGameplayEffects;

	/**
	 * The full list of Gameplay Effects (GEs) that are always passively applied to the character.
	 *
	 * This is a superset of the character's managed passive GEs and additional passive GEs. Each value is a gameplay
	 * effect and the key is the weight of that GE. The weight controls the order that all GEs are applied. Lower
	 * weights are applied earlier than higher weights.
	 */
	TMultiMap<int32, TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	// =================================================================================================================
	// Protected Constructors
	// =================================================================================================================
	/**
	 * Constructor for sub-classes to specify the type of the ASC and Attribute Set.
	 *
	 * @param ComponentFactory
	 *   The factory component to use for generating the ASC and Attribute Set.
	 */
	template<class AscType, class AttributeSetType>
	explicit APF2CharacterBase(TPF2CharacterComponentFactory<AscType, AttributeSetType> ComponentFactory) :
		CharacterName(FText::FromString(TEXT("Character"))),
		CharacterLevel(1),
		bManagedPassiveEffectsGenerated(false),
		bPassiveEffectsActivated(false)
	{
		this->AbilitySystemComponent = ComponentFactory.CreateAbilitySystemComponent(this);
		this->AttributeSet           = ComponentFactory.CreateAttributeSet(this);

		for (const auto& GeCoreBlueprintPath : PF2CharacterConstants::GeCoreBlueprintPaths)
		{
			const FString EffectName = GeCoreBlueprintPath.Key;
			const int32   Weight     = GeCoreBlueprintPath.Value;
			const FString EffectPath = GetBlueprintPath(EffectName);

			const ConstructorHelpers::FObjectFinder<UClass> EffectBP(*EffectPath);

			this->CoreGameplayEffects.Add(Weight, EffectBP.Object);
		}

		for (const auto& AbilityName : FPF2AbilityAttributes::GetInstance().GetAbilityNames())
		{
			const FString Filename =
				GetBlueprintPath(FString::Format(*PF2CharacterConstants::GeBlueprintBoostNameFormat, {AbilityName}));

			const ConstructorHelpers::FObjectFinder<UClass> EffectBP(*Filename);

			// Allow boost effects to be looked-up by ability name later.
			this->AbilityBoostEffects.Add(AbilityName, EffectBP.Object);

			// Meanwhile, give game designers an easy way to set boosts on a per-ability basis.
			this->AdditionalAbilityBoosts.Add(FPF2CharacterAbilityBoostCount(AbilityName, 0));
		}
	}

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Returns the path to the Blueprint having the given name.
	 *
	 * @param Name
	 *	The name of the blueprint for which a path is desired.
	 *
	 * @return
	 *	The path to the blueprint.
	 */
	static FORCEINLINE FString GetBlueprintPath(FString Name)
	{
		return FString::Format(TEXT("{0}{1}.{1}_C"), { PF2CharacterConstants::BlueprintBasePath, Name });
	}

	/**
	 * Gets whether the local machine has authoritative control over this character actor.
	 *
	 * Only the authoritative machine (e.g. the server) should make changes to GEs to ensure that changes to them
	 * replicate properly.
	 *
	 * @return
	 *	true if the local machine is the server that is authoritative about this character; or, false if the local
	 *	machine is a client doing simulation or prediction.
	 */
	bool IsAuthorityForEffects() const;

	/**
	 * Activates Gameplay Effects that are always passively applied to the character.
	 */
	void ActivatePassiveGameplayEffects();

	/**
	 * Populates the full list of passive Gameplay Effects, sorted by weight.
	 */
	void PopulatePassiveGameplayEffects();

	/**
	 * Adds tags that were set on this character to the provided Gameplay Effect spec, as dynamic tags.
	 *
	 * This includes tags like ancestry, additional languages, and skill proficiencies.
	 */
	void ApplyDynamicTags(FGameplayEffectSpec* GameplayEffectSpec) const;

	/**
	 * Removes all passive Gameplay Effects that were previously activated for this character.
	 */
	void DeactivatePassiveGameplayEffects();

	/**
	 * Populates the list of passive Gameplay Effects based on the settings in this blueprint.
	 *
	 * This method is idempotent. If the list is already populated, this method has no effect.
	 */
	void GenerateManagedPassiveGameplayEffects();

	/**
	 * Clear the list of managed, passive Gameplay Effects (GEs) so that it can be regenerated.
	 *
	 * This should not be called if passive GEs are already applied to the character. If GEs are already applied, you
	 * must call DeactivatePassiveGameplayEffects() first.
	 */
	void ClearManagedPassiveGameplayEffects();

	/**
	 * Callback invoked when a character's level has changed, to allow logic that depends on levels to be refreshed.
	 *
	 * @param OldLevel
	 *	The previous level of the character.
	 * @param NewLevel
	 *	The new level for the character.
	 */
	virtual void HandleCharacterLevelChanged(float OldLevel, float NewLevel);

	// =================================================================================================================
	// Blueprint Event Callbacks
	// =================================================================================================================
	/**
	 * BP event invoked when a character's level has changed, to allow logic that depends on levels to be refreshed.
	 *
	 * @param OldLevel
	 *	The previous level of the character.
	 * @param NewLevel
	 *	The new level for the character.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnCharacterLevelChanged(float OldLevel, float NewLevel);
};

/**
 * Type of object for instantiating the ASC and attribute set for this type of character.
 *
 * This is in a separate object to allow these types to be parameterized/templated so sub-classes can swap out the type
 * of ASC and attribute set just by supplying different types in their constructors.
 */
template<class AscType, class AttributeSetType>
class TPF2CharacterComponentFactory
{
public:
	/**
	 * Creates an Ability System Component (ASC) for this character.
	 *
	 * The ASC is automatically created as a default sub-object of the character, with the name
	 * "AbilitySystemComponent".
	 *
	 * @param Character
	 *	The character for which the ASC will be created.
	 *
	 * @return
	 *	The new ASC.
	 */
	static AscType* CreateAbilitySystemComponent(APF2CharacterBase* Character)
	{
		AscType* AbilitySystemComponent = Character->CreateDefaultSubobject<AscType>(TEXT("AbilitySystemComponent"));

		AbilitySystemComponent->SetIsReplicated(true);

		return AbilitySystemComponent;
	}

	/**
	 * Creates an Attribute Set for this character.
	 *
	 * The attribute set is automatically created as a default sub-object of the character, with the name
	 * "AttributeSet".
	 *
	 * @param Character
	 *	The character for which the attribute set will be created.
	 *
	 * @return
	 *	The new attribute set.
	 */
	static AttributeSetType* CreateAttributeSet(APF2CharacterBase* Character)
	{
		return Character->CreateDefaultSubobject<AttributeSetType>(TEXT("AttributeSet"));
	}
};