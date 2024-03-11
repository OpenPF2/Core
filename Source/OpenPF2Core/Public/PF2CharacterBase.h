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

#include <AbilitySystemInterface.h>

#include <GameFramework/Character.h>

#include <UObject/ConstructorHelpers.h>
#include <UObject/ScriptInterface.h>

#include "PF2CharacterConstants.h"
#include "PF2CharacterInterface.h"

#include "Abilities/Attacks/PF2AttackAttributeSet.h"

#include "Actors/Components/PF2AbilitySystemComponent.h"

#include "CharacterStats/PF2AncestryAndHeritageGameplayEffectBase.h"
#include "CharacterStats/PF2BackgroundGameplayEffectBase.h"
#include "CharacterStats/PF2CharacterAbilityScoreType.h"
#include "CharacterStats/PF2CharacterAttributeSet.h"
#include "CharacterStats/PF2ClassGameplayEffectBase.h"
#include "CharacterStats/AbilityBoosts/PF2AbilityBoostBase.h"

#include "Utilities/PF2GameplayAbilityUtilities.h"
#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2CharacterBase.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2CharacterCommandInterface;

template<class AscType, class CommandQueueType, class OwnerTrackerType, class AttributeSetType, class AttackAttributeSetType>
class TPF2CharacterComponentFactory;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Struct for representing the selection of what ability/abilities to boost when activating a specific boost GA.
 */
USTRUCT(BlueprintType)
struct OPENPF2CORE_API FPF2CharacterAbilityBoostSelection
{
	GENERATED_BODY()

	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Constructor for FPF2CharacterAbilityBoostSelection.
	 */
	explicit FPF2CharacterAbilityBoostSelection()
	{
	}

	/**
	 * Constructor for FPF2CharacterAbilityBoostSelection.
	 *
	 * @param BoostGameplayAbility
	 *	The "Boost GA" -- the Gameplay Ability for which ability score boost selections are being applied.
	 * @param SelectedAbilities
	 *	The ability scores that the player selected, out of the options offered by the Boost GA.
	 */
	explicit FPF2CharacterAbilityBoostSelection(
		const TSubclassOf<UPF2AbilityBoostBase>    BoostGameplayAbility,
		const TSet<EPF2CharacterAbilityScoreType>& SelectedAbilities) :
			BoostGameplayAbility(BoostGameplayAbility),
			SelectedAbilities(SelectedAbilities)
	{
	}

	/**
	 * The "Boost GA" -- the Gameplay Ability for which ability score boost selections are being applied.
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPF2AbilityBoostBase> BoostGameplayAbility;

	/**
	 * The ability scores that the player selected, out of the options offered by the Boost GA.
	 */
	UPROPERTY(EditAnywhere)
	TSet<EPF2CharacterAbilityScoreType> SelectedAbilities;
};

/**
 * Base class for both playable and non-playable characters in OpenPF2.
 *
 * OpenPF2-based games must extend this class if they have custom character attributes or abilities.
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2CharacterBase :
	public ACharacter,
	public IPF2EventEmitterInterface,
	public IPF2CharacterInterface,
	public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The events object used for binding Blueprint callbacks to events from this component.
	 */
	UPROPERTY(Transient)
	mutable UPF2CharacterInterfaceEvents* Events;

	/**
	 * The Ability System Component (ASC) used for interfacing this character with the Gameplay Abilities System (GAS).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAbilitySystemComponent* AbilitySystemComponent;

	/**
	 * The sub-component that tracks commands for this character queued during encounters.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UActorComponent* CommandQueue;

	/**
	 * The sub-component that tracks which player owns/controls this character.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UActorComponent* OwnerTracker;

	/**
	 * The attributes of this character.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPF2CharacterAttributeSet* AttributeSet;

	/**
	 * The transient attack stats of this character.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPF2AttackAttributeSet* AttackAttributeSet;

	UPROPERTY()
	bool bAreAbilitiesInitialized;

	/**
	 * Whether or not managed passive Gameplay Effects have been generated for this character.
	 */
	UPROPERTY()
	bool bManagedPassiveEffectsGenerated;

	/**
	 * The Gameplay Effects that drive stats for every character.
	 */
	TMultiMap<FName, TSubclassOf<UGameplayEffect>> CoreGameplayEffects;

	/**
	 * The list of passive Gameplay Effects (GEs) that are generated from other values specified on this character.
	 *
	 * Each value is a gameplay effect and the key is the weight group of that GE (sorted alphanumerically). The weight
	 * controls the order that all GEs are applied. Lower weights are applied earlier than higher weights.
	 *
	 * The names of each group are exposed as tags in the "GameplayEffect.WeightGroup" tag list so that they can be
	 * applied to GEs by game designers to control the default group that a GE gets added to. A GE can also be
	 * explicitly added to a group via the AddPassiveGameplayEffectWithWeight() method on the Character ASC.
	 */
	TMultiMap<FName, TSubclassOf<UGameplayEffect>> ManagedGameplayEffects;

	/**
	 * The ability boost selections that have already been applied to this character.
	 *
	 * This is used to keep track of which boosts to disregard in the event that a character's passive GEs are disabled
	 * and then re-enabled (as happens during character leveling). Without this, when a passive GE that grants an
	 * ability boost is re-activated, the player would have the option to choose another set of ability boosts even
	 * though their prior selections are still in effect on the player's character.
	 */
	UPROPERTY()
	TArray<FPF2CharacterAbilityBoostSelection> AppliedAbilityBoostSelections;

	// =================================================================================================================
	// Protected Fields - Blueprint Accessible
	// =================================================================================================================
	/**
	 * The human-friendly name of this character.
	 *
	 * For playable characters, this may come from the player themselves unless the story dictates that the character
	 * has a specific name because it is important for the plot.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="OpenPF2 - Character")
	FText CharacterName;

	/**
	 * The visual portrait of this character, to represent them in the UI to players/users.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="OpenPF2 - Character")
	UTexture2D* CharacterPortrait;

	/**
	 * The current level of this character.
	 */
	UPROPERTY(EditAnywhere, Replicated, meta=(ClampMin=1), Category="OpenPF2 - Character")
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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="OpenPF2 - Character")
	TSubclassOf<UPF2AncestryAndHeritageGameplayEffectBase> AncestryAndHeritage;

	/**
	 * The background of this character.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 33:
	 * "Backgrounds ... describe training or environments your character experienced before becoming an adventurer. Your
	 * character’s background provides ability boosts, skill training, and a skill feat."
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="OpenPF2 - Character")
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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="OpenPF2 - Character")
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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="CreatureAlignment"), Category="OpenPF2 - Character")
	FGameplayTag Alignment;

	/**
	 * Additional languages that this character has learned beyond those known at 1st level.
	 *
	 * Languages granted by ancestry do not need to be duplicated here.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="Language"), Category="OpenPF2 - Character")
	FGameplayTagContainer AdditionalLanguages;

	/**
	 * The name(s) of lore sub-categories that this character is knowledgeable about.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="OpenPF2 - Skills")
	TArray<FString> LoreNames;

	/**
	 * Proficiency ranks manually applied by a game designer or player to this character's skills.
	 *
	 * Higher proficiencies override lower proficiencies, so if a character is, for example, "Master" in "Survival"
	 * skill, you only need to select "Skill.Survival.Master", and do not need to also select the lower proficiency tags
	 * of "Skill.Survival.Trained" and "Skill.Survival.Untrained".
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="Skill"), Category="OpenPF2 - Skills")
	FGameplayTagContainer AdditionalSkillProficiencies;

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="OpenPF2 - Character")
	TArray<TSubclassOf<UGameplayEffect>> AdditionalPassiveGameplayEffects;

	/**
	 * Additional Gameplay Abilities (GAs) that are granted to the character at the start of play.
	 *
	 * This list is combined with any abilities that are separately granted by the character's ancestry, background,
	 * heritage, or skills. You should only grant custom abilities here that are needed for story or special character
	 * interactions; otherwise, abilities should only be granted through the aforementioned, more standard means.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="OpenPF2 - Character")
	TArray<TSubclassOf<UGameplayAbility>> AdditionalGameplayAbilities;

	/**
	 * The abilities to boost, as chosen by the player or a game designer, out of what ability boosts are pending.
	 *
	 * At the start of play, or upon a call to ApplyAbilityBoostSelections(), an attempt is made to match up
	 * each selection in this property to a boost ability granted to this character. Upon a match, the matching GA is
	 * activated, and the selection is removed from this property. Boost GAs are single-shot abilities that remove
	 * themselves once they've applied an ability score boost. So, any selections added to this property "consume"
	 * pending ability boosts for this character. Each selection is matched-up and evaluated in the order it appears in
	 * this property, so multiple selections can target pending ability boosts of the same boost GA type without there
	 * being a conflict.
	 *
	 * Each boost GA is granted to the character by background, ancestry, heritage, or class. This approach to
	 * matching-up ability boost selections to pending ability boosts helps to ensure that the kind and number of
	 * boosts that get applied are legal according to game rules, unless a game designer manually adds boost GEs to the
	 * character.
	 *
	 * If ability boosts are added manually via GEs, take care not to apply more boosts than the boost cap at the
	 * character's current level, and pay close attention to ancestry, class, and other gameplay rules and restrictions
	 * to avoid creating a character that is overpowered. In addition, the character's key ability boost (as specified
	 * by their class) is applied automatically, so you should not add an additional boost for the character's key
	 * ability.
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
	UPROPERTY(EditAnywhere, Category="OpenPF2 - Ability Boosts")
	TArray<FPF2CharacterAbilityBoostSelection> AbilityBoostSelections;

	/**
	 * Handles for all additional abilities that have been granted to this character.
	 *
	 * This will be empty if the additional gameplay abilities are yet to be granted.
	 *
	 * @see AdditionalGameplayAbilities
	 */
	UPROPERTY(BlueprintReadOnly)
	TMap<TSubclassOf<UGameplayAbility>, FGameplayAbilitySpecHandle> GrantedAdditionalAbilities;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for APF2CharacterBase.
	 */
	explicit APF2CharacterBase();

protected:
	// =================================================================================================================
	// Protected Constructors
	// =================================================================================================================
	/**
	 * Constructor for sub-classes to specify the type of the ASC and Attribute Set.
	 *
	 * @param ComponentFactory
	 *   The factory component to use for generating the ASC and Attribute Set.
	 *
	 * @tparam AscType
	 *	The class type to use for the Ability System Component (ASC).
	 * @tparam CommandQueueType
	 *	The class type to use for the command queue component (for encounters).
	 * @tparam OwnerTrackerType
	 *	The class type to use for the owner tracking component.
	 * @tparam AttributeSetType
	 *	The class type to use for the character attribute set.
	 * @tparam AttackAttributeSetType
	 *	The class type to use for the transient attack attribute set.
	 */
	template<class AscType, class CommandQueueType, class OwnerTrackerType, class AttributeSetType, class AttackAttributeSetType>
	explicit APF2CharacterBase(
		TPF2CharacterComponentFactory<AscType, CommandQueueType, OwnerTrackerType, AttributeSetType, AttackAttributeSetType> ComponentFactory) :
		Events(nullptr),
		bAreAbilitiesInitialized(false),
		bManagedPassiveEffectsGenerated(false),
		CharacterName(FText::FromString(TEXT("Character"))),
		CharacterLevel(1)
	{
		this->AbilitySystemComponent = ComponentFactory.CreateAbilitySystemComponent(this);
		this->CommandQueue           = ComponentFactory.CreateCommandQueue(this);
		this->OwnerTracker           = ComponentFactory.CreateOwnerTracker(this);
		this->AttributeSet           = ComponentFactory.CreateAttributeSet(this);
		this->AttackAttributeSet	 = ComponentFactory.CreateAttackAttributeSet(this);

		for (const TTuple<FString, FName>& EffectInfo : PF2CharacterConstants::GeCoreCharacterBlueprintPaths)
		{
			const FString Subfolder  = EffectInfo.Key;
			const FName   EffectName = EffectInfo.Value;
			const FString EffectPath = PF2CharacterConstants::GetBlueprintPath(EffectName, Subfolder);

			const ConstructorHelpers::FObjectFinder<UClass> EffectFinder(*EffectPath);
			const TSubclassOf<UGameplayEffect>              GameplayEffect = EffectFinder.Object;

			const FName WeightGroup = PF2GameplayAbilityUtilities::GetWeightGroupOfGameplayEffect(GameplayEffect);

			this->CoreGameplayEffects.Add(WeightGroup, GameplayEffect);
		}
	}

public:
	// =================================================================================================================
	// Public Methods - ACharacter Overrides
	// =================================================================================================================
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods - APawn Overrides
	// =================================================================================================================
	virtual void OnRep_Controller() override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	virtual FString GetIdForLogs() const override;

	// =================================================================================================================
	// Public Methods - IAbilitySystemInterface Implementation
	// =================================================================================================================
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// =================================================================================================================
	// Public Methods - IPF2EventEmitterInterface Implementation
	// =================================================================================================================
	virtual UObject* GetGenericEventsObject() const override;

	// =================================================================================================================
	// Public Methods - IPF2CharacterInterface Implementation
	// =================================================================================================================
	virtual UPF2CharacterInterfaceEvents* GetEvents() const override;

	virtual FText GetCharacterName() const override;

	virtual UTexture2D* GetCharacterPortrait() const override;

	virtual int32 GetCharacterLevel() const override;

	virtual TScriptInterface<IPF2CharacterAbilitySystemInterface> GetCharacterAbilitySystemComponent() const override;

	virtual TScriptInterface<IPF2CommandQueueInterface> GetCommandQueueComponent() const override;

	virtual TScriptInterface<IPF2OwnerTrackingInterface> GetOwnerTrackingComponent() const override;

	virtual TScriptInterface<IPF2PlayerControllerInterface> GetPlayerController() const override;

	virtual TArray<TScriptInterface<IPF2AbilityBoostInterface>> GetPendingAbilityBoosts() const override;

	virtual void InitializeOrRefreshAbilities() override;

	virtual AActor* ToActor() override;

	virtual APawn* ToPawn() override;

	virtual bool IsAlive() override;

	virtual void AddAbilityBoostSelection(const TSubclassOf<UPF2AbilityBoostBase>   BoostGameplayAbility,
	                                      const TSet<EPF2CharacterAbilityScoreType> SelectedAbilities) override;

	/**
	 * Attempts to find and activate a pending ability boost Gameplay Ability for each Ability Boost selection on this
	 * character.
	 *
	 * Pending ability boosts are registered on this character via calls to AddAbilityBoostSelection() before a call to
	 * this method.
	 *
	 * The call flow for this is as follows:
	 *	1. Ability boosts are added to this character via one or more calls to AddAbilityBoostSelection().
	 *	2. This method is called.
	 *	3. This method invokes ActivateAbilityBoost() once for each pending boost selection.
	 *	4. ActivateAbilityBoost() is used to activate the boost Gameplay Ability (GA) that corresponds to a pending
	 *	   boost selection.
	 *	5. During activation, the boost GA calls the ApplyAbilityBoost() method on the ASC for this character to
	 *	   activate each valid boost selection.
	 */
	virtual void ApplyAbilityBoostSelections() override;

	virtual void ActivatePassiveGameplayEffects() override;

	virtual void DeactivatePassiveGameplayEffects() override;

	virtual void AddAndActivateGameplayAbility(const TSubclassOf<UGameplayAbility> Ability) override;

	virtual void Native_OnDamageReceived(const float                         Damage,
	                                     IPF2CharacterInterface*             InstigatorCharacter,
	                                     AActor*                             DamageSource,
	                                     const struct FGameplayTagContainer* SourceTags,
	                                     const FHitResult                    HitInfo) override;

	virtual void Native_OnHitPointsChanged(const float                  Delta,
	                                       const float                  NewValue,
	                                       const FGameplayTagContainer* SourceTags) override;

	virtual void Native_OnSpeedChanged(const float                  Delta,
	                                   const float                  NewValue,
	                                   const FGameplayTagContainer* SourceTags) override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_OnEncounterTurnStarted() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_OnEncounterTurnEnded() override;

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
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual bool SetCharacterLevel(int32 NewLevel);

	/**
	 * Adds an additional boost of the specified ability to this character's attribute set.
	 *
	 * The boost is accomplished via a passive Gameplay Effect added to this character's ASC. In addition, the count of
	 * Additional Ability Boosts on this character are updated to ensure that the boost survives passive GEs being
	 * recalculated/reapplied, as would happen during character leveling.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
    virtual void ApplyAbilityBoost(EPF2CharacterAbilityScoreType TargetAbilityScore);

	/**
	 * Removes any ability boosts that were previously provided with a choice from the player or game designer but that
	 * have nevertheless been re-granted to this character (e.g., because passive GEs were toggled off and then on).
	 *
	 * An ability boost is declared "Redundant" if its Boost GA class matches the boost GA class of an applied ability
	 * boost. Since the same boost GA class can be used multiple times on the same character, this method should ONLY be
	 * called when passive GAs are being re-enabled after being toggled off; otherwise, we run the risk of skipping over
	 * a new ability boost GA that just happens to have the same class as one that was already applied.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Characters")
	virtual void RemoveRedundantPendingAbilityBoosts();

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
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
	 * Activates the specified ability boost ability with the provided selections of which abilities to boost.
	 */
	void ActivateAbilityBoost(
		FGameplayAbilitySpec*                     BoostSpec,
		const FPF2CharacterAbilityBoostSelection& AbilityBoostSelection) const;

	/**
	 * Populates the full list of passive Gameplay Effects, sorted by weight.
	 */
	void PopulatePassiveGameplayEffects();

	/**
	 * Adds tags that were set on this character as dynamic tags on the ASC.
	 *
	 * This includes tags like ancestry, additional languages, and skill proficiencies.
	 */
	void ApplyDynamicTags() const;

	/**
	 * Populates the list of passive Gameplay Effects based on the settings in this blueprint.
	 *
	 * This method is idempotent. If the list is already populated, this method has no effect.
	 */
	void GenerateManagedPassiveGameplayEffects();

	/**
	 * Clear the list of managed, passive Gameplay Effects (GEs) so that it can be regenerated.
	 *
	 * This should not be called if passive GEs are already applied to this character. If GEs are already applied, you
	 * must call DeactivateAllPassiveGameplayEffects() first.
	 */
	void ClearManagedPassiveGameplayEffects();

	/**
	 * Grants any gameplay abilities that have been configured on this character by the game designer.
	 *
	 * This method is idempotent. If the character has already been granted the abilities, this method has no effect.
	 */
	void GrantAdditionalAbilities();

	// =================================================================================================================
	// Protected Event Notifications
	// =================================================================================================================
	/**
	 * Callback invoked when a character's level has changed, to allow logic that depends on levels to be refreshed.
	 *
	 * @param OldLevel
	 *	The previous level of this character.
	 * @param NewLevel
	 *	The new level for this character.
	 */
	virtual void Native_OnCharacterLevelChanged(float OldLevel, float NewLevel);

	// =================================================================================================================
	// Blueprint Implementable Events
	// =================================================================================================================
	/**
	 * BP event invoked when a character's level has changed, to allow logic that depends on levels to be refreshed.
	 *
	 * @param OldLevel
	 *	The previous level of this character.
	 * @param NewLevel
	 *	The new level for this character.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Characters",
		meta=(DisplayName="On Character Level Changed")
	)
	void BP_OnCharacterLevelChanged(float OldLevel, float NewLevel);

	/**
	 * BP event invoked when this character receives damage.
	 *
	 * @param Damage
	 *	The amount of the damage.
	 * @param InstigatorCharacter
	 *	The character that is ultimately responsible for the damage. This can be null if the damage is caused by the
	 *	environment.
	 * @param DamageSource
	 *	The actor that directly inflicted the damage, such as a weapon or projectile.
	 * @param SourceTags
	 *	Tags passed along with the damage Gameplay Event that were captured from the source ability and source
	 *	character.
	 * @param HitInfo
	 *	Hit result information, including who was hit and where the damage was inflicted.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Characters",
		meta=(DisplayName="On Damage Received")
	)
	void BP_OnDamageReceived(const float                                     Damage,
	                         const TScriptInterface<IPF2CharacterInterface>& InstigatorCharacter,
	                         AActor*                                         DamageSource,
	                         const FGameplayTagContainer&                    SourceTags,
	                         const FHitResult                                HitInfo);

	/**
	 * BP event invoked when this character's hit points (i.e., health) have changed.
	 *
	 * @param Delta
	 *	The amount that the character's hit points have changed.
	 * @param NewValue
	 *	The new amount of hit points after the change.
	 * @param SourceTags
	 *	Tags passed along with the Gameplay Event that were captured from the source ability and source character as
	 *	metadata about the cause of the change to hit points.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Characters",
		meta=(DisplayName="On Hit Points Changed")
	)
	void BP_OnHitPointsChanged(const float                         Delta,
	                           const float                         NewValue,
	                           const struct FGameplayTagContainer& SourceTags);

	/**
	 * BP event invoked when this character's speed (i.e., how fast it can move during a stride) has changed.
	 *
	 * @param Delta
	 *	The amount that the character's speed has changed.
	 * @param NewValue
	 *	The new amount of speed after the change.
	 * @param SourceTags
	 *	Tags passed along with the Gameplay Event that were captured from the source ability and source character as
	 *	metadata about the cause of the change to speed.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category="OpenPF2|Characters",
		meta=(DisplayName="On Speed Changed")
	)
	void BP_OnSpeedChanged(const float                  Delta,
	                       const float                  NewValue,
	                       const FGameplayTagContainer& SourceTags);
};

/**
 * Type of object for instantiating sub-components for a character.
 *
 * This is in a separate object to allow these types to be parameterized/templated so sub-classes can swap out the type
 * of each sub-component in their constructors. UE4 does not allow a sub-class to create a default sub-component having
 * the same name as one that was defined in the parent class.
 *
 * This approach is used as a workaround for a gap in UE4 constructor behavior. Prior to UE 4.23, it appears that the
 * standard pattern would have been to use ObjectInitializer.SetDefaultSubobjectClass() from within
 * UObject(const FObjectInitializer& ObjectInitializer), but that constructor was deprecated in 4.23. In UE5 it looks
 * like there is now a "Change Subobject Class" method through the "Subobject Data Subsystem".
 *
 * Discussion here:
 * https://stackoverflow.com/questions/69351471/how-can-i-create-a-ue4-uclass-base-class-that-uses-createdefaultsubobject-but
 *
 * @tparam AscType
 *	The class type to use for the Ability System Component (ASC).
 * @tparam CommandQueueType
 *	The class type to use for the command queue component (for encounters).
 * @tparam OwnerTrackerType
 *	The class type to use for the owner tracking component.
 * @tparam AttributeSetType
*	The class type to use for the character attribute set.
 * @tparam AttackAttributeSetType
 *	The class type to use for the transient attack attribute set.
 */
template<class AscType, class CommandQueueType, class OwnerTrackerType, class AttributeSetType, class AttackAttributeSetType>
class TPF2CharacterComponentFactory
{
public:
	/**
	 * Creates an Ability System Component (ASC) for a character.
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
	 * Creates a Command Queue Component for a character.
	 *
	 * The component is automatically created as a default sub-object of the character, with the name
	 * "CommandQueue".
	 *
	 * @param Character
	 *	The character for which the command queue will be created.
	 *
	 * @return
	 *	The new command queue.
	 */
	static CommandQueueType* CreateCommandQueue(APF2CharacterBase* Character)
	{
		return Character->CreateDefaultSubobject<CommandQueueType>(TEXT("CommandQueue"));
	}

	/**
	 * Creates an Owner Tracking Component for a character.
	 *
	 * The component is automatically created as a default sub-object of the character, with the name
	 * "OwnerTracker".
	 *
	 * @param Character
	 *	The character for which the owner tracker will be created.
	 *
	 * @return
	 *	The new owner tracker.
	 */
	static OwnerTrackerType* CreateOwnerTracker(APF2CharacterBase* Character)
	{
		return Character->CreateDefaultSubobject<OwnerTrackerType>(TEXT("OwnerTracker"));
	}

	/**
	 * Creates the primary attribute set for a character.
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

	/**
	 * Creates the transient attack attribute set for a character.
	 *
	 * The attribute set is automatically created as a default sub-object of the character, with the name
	 * "AttackAttributeSet".
	 *
	 * @param Character
	 *	The character for which the attribute set will be created.
	 *
	 * @return
	 *	The new attribute set.
	 */
	static AttackAttributeSetType* CreateAttackAttributeSet(APF2CharacterBase* Character)
	{
		return Character->CreateDefaultSubobject<AttackAttributeSetType>(TEXT("AttackAttributeSet"));
	}
};
