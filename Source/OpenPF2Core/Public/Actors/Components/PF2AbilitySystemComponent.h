// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <AbilitySystemComponent.h>

#include "PF2EventEmitterInterface.h"

#include "CharacterStats/PF2CharacterAbilitySystemInterface.h"

#include "PF2AbilitySystemComponent.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2AbilityBoostInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UCLASS(ClassGroup="OpenPF2-Characters")
class OPENPF2CORE_API UPF2AbilitySystemComponent :
	public UAbilitySystemComponent,
	public IPF2EventEmitterInterface,
	public IPF2CharacterAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Constants
	// =================================================================================================================
	/**
	 * The name of the tag that designates an ability the default movement ability.
	 *
	 * This ability is used to move a character to a location, such as within range of another actor/character.
	 */
	static const FName DefaultMovementAbilityTagName;

	/**
	 * The name of the tag that designates an ability the default "face target" ability.
	 *
	 * This ability is used to rotate a character to face another actor/character.
	 */
	static const FName DefaultFaceTargetAbilityTagName;

	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The events object used for binding Blueprint callbacks to events from this component.
	 */
	UPROPERTY(Transient)
	mutable UPF2AbilitySystemInterfaceEvents* Events;

	/**
	 * Whether character abilities have been replicated from the server at least once for the owning character.
	 *
	 * @see UPF2AbilitySystemComponent::OnRep_ActivateAbilities()
	 */
	bool bAreAbilitiesAvailable;

	/**
	 * The Gameplay Effects used to boost abilities.
	 *
	 * For each pair in the map, the key is the type ability score that the effect boosts and the value is the GE for
	 * boosting that ability.
	 */
	UPROPERTY()
	TMap<EPF2CharacterAbilityScoreType, TSubclassOf<UGameplayEffect>> AbilityBoostEffects;

	/**
	 * The list of tags on this ASC that are otherwise not granted by a GE.
	 *
	 * These are used to apply replicated tags that are specific to a particular character instance, such as age, size,
	 * skill proficiency, etc.
	 */
	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer DynamicTags;

	/**
	 * The weight groups of Gameplay Effects that have been activated on this ASC.
	 */
	UPROPERTY(BlueprintReadOnly)
	TSet<FName> ActivatedWeightGroups;

	/**
	 * A special, "dummy" GE that is used for applying dynamic tags.
	 *
	 * TODO: Find a different way to accomplish this without a GE. This feels very much like a kludge.
	 */
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DynamicTagsEffect;

	/**
	 * The list of Gameplay Effects (GEs) that are always passively applied to this ASC.
	 *
	 * This is typically a superset of the owning character's managed passive GEs and additional passive GEs. Each value
	 * is a gameplay effect and the key is the weight group of that GE. The weight controls the order that all GEs are
	 * applied. Lower weights are applied earlier than higher weights.
	 */
	TMultiMap<FName, TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	/**
	 * The cached list of all Gameplay Effects registered on this ASC with AddPassiveGameplayEffect() or
	 * AddPassiveGameplayEffectWithWeight.
	 */
	TMultiMap<FName, TSubclassOf<UGameplayEffect>> CachedPassiveGameplayEffectsToApply;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	explicit UPF2AbilitySystemComponent();

	// =================================================================================================================
	// Public Methods - IPF2EventEmitterInterface Implementation
	// =================================================================================================================
	virtual UObject* GetGenericEventsObject() const override;

	// =================================================================================================================
	// Public Methods - IPF2AbilitySystemInterface Implementation
	// =================================================================================================================
	virtual UPF2AbilitySystemInterfaceEvents* GetEvents() const override;

	virtual TScriptInterface<IPF2InteractableAbilityInterface> GetAbilityInstanceFromSpec(
		const FGameplayAbilitySpec& AbilitySpec) const override;

	virtual TArray<TScriptInterface<IPF2InteractableAbilityInterface>> GetAbilities() const override;

	virtual TArray<TScriptInterface<IPF2InteractableAbilityInterface>> GetAbilitiesByTags(
		const FGameplayTagContainer& Tags,
		bool bExactMatch = true) const override;

	virtual bool HasActiveAbilityWithTags(const FGameplayTagContainer& Tags) override;

	virtual FGameplayTagContainer GetActiveGameplayTags() const override;

	virtual bool ArePassiveGameplayEffectsActive() const override
	{
		return this->ActivatedWeightGroups.Num() != 0;
	}

	virtual UAbilitySystemComponent* ToAbilitySystemComponent() override;

	virtual TArray<FGameplayAbilitySpec> FindAbilitySpecsByTags(
		const FGameplayTagContainer& Tags,
		bool                         bOnlyAbilitiesThatSatisfyTagRequirements = true) const override;

	virtual FGameplayAbilitySpec FindAbilitySpecByTags(
		UPARAM(DisplayName="Tags")
		const FGameplayTagContainer& InTags,

		UPARAM(DisplayName="Match Found")
		bool& bOutMatchFound,

		UPARAM(DisplayName="Only Abilities that Satisfy Tag Requirements")
		const bool bInOnlyAbilitiesThatSatisfyTagRequirements = true
	) const override;

	virtual TArray<FGameplayAbilitySpecHandle> FindAbilityHandlesByTags(
		const FGameplayTagContainer& Tags,
		const bool                   bOnlyAbilitiesThatSatisfyTagRequirements = true
	) const override;

	virtual FGameplayAbilitySpecHandle FindAbilityHandleByTags(
		UPARAM(DisplayName="Tags")
		const FGameplayTagContainer& InTags,

		UPARAM(DisplayName="Match Found")
		bool& bOutMatchFound,

		UPARAM(DisplayName="Only Abilities that Satisfy Tag Requirements")
		const bool bInOnlyAbilitiesThatSatisfyTagRequirements = true
	) const override;

	virtual bool TriggerAbilityWithPayload(const FGameplayAbilitySpecHandle AbilityHandle,
	                                       const FGameplayEventData         Payload) override;

	virtual void AddPassiveGameplayEffect(TSubclassOf<UGameplayEffect> Effect) override;

	virtual void AddPassiveGameplayEffectWithWeight(
		const FName WeightGroup,
		const TSubclassOf<UGameplayEffect> Effect
	) override;

	virtual void SetPassiveGameplayEffects(const TMultiMap<FName, TSubclassOf<UGameplayEffect>> Effects) override;

	virtual void RemoveAllPassiveGameplayEffects() override;

	virtual void ActivateAllPassiveGameplayEffects() override;

	virtual void DeactivateAllPassiveGameplayEffects() override;

	virtual TSet<FName> ActivatePassiveGameplayEffectsAfter(const FName StartingWeightGroup) override;

	virtual TSet<FName> DeactivatePassiveGameplayEffectsAfter(const FName StartingWeightGroup) override;

	virtual bool ActivatePassiveGameplayEffects(const FName WeightGroup) override;

	virtual bool DeactivatePassiveGameplayEffects(const FName WeightGroup) override;

	virtual void AddDynamicTag(const FGameplayTag Tag) override;

	virtual void AppendDynamicTags(const FGameplayTagContainer Tags) override;

	virtual void SetDynamicTags(const FGameplayTagContainer Tags) override;

	virtual void RemoveDynamicTag(const FGameplayTag Tag) override;

	virtual void RemoveDynamicTags(const FGameplayTagContainer Tags) override;

	virtual void RemoveAllDynamicTags() override;

	// =================================================================================================================
	// Public Methods - IPF2CharacterAbilitySystemInterface Implementation
	// =================================================================================================================
	virtual TScriptInterface<IPF2CharacterInterface> GetCharacter() const override;

	virtual int32 GetCharacterLevel() const override;

	virtual TMap<EPF2CharacterAbilityScoreType, FPF2AttributeModifierSnapshot> GetAbilityScoreValues() const override;

	virtual TArray<TScriptInterface<IPF2AbilityBoostInterface>> GetPendingAbilityBoosts() const override;

	virtual TSubclassOf<UGameplayEffect> GetBoostEffectForAbility(
		const EPF2CharacterAbilityScoreType AbilityScore) override
	{
		return this->AbilityBoostEffects[AbilityScore];
	}

	virtual void ApplyAbilityBoost(const EPF2CharacterAbilityScoreType TargetAbilityScore) override;

	virtual bool HasDefaultMovementAbility() const override;

	virtual FGameplayAbilitySpecHandle FindDefaultMovementAbilityHandle(bool& bOutMatchFound) const override;

	virtual bool HasDefaultFaceTargetAbility() const override;

	virtual FGameplayAbilitySpecHandle FindDefaultFaceTargetAbilityHandle(bool& bOutMatchFound) const override;

	// =================================================================================================================
	// Public Methods - IPF2ActorComponentInterface Implementation
	// =================================================================================================================
	virtual UActorComponent* ToActorComponent() override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	virtual FString GetIdForLogs() const override;

protected:
	// =================================================================================================================
	// Protected Methods - UAbilitySystemComponent Overrides
	// =================================================================================================================
	virtual void OnRep_ActivateAbilities() override;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================

	/**
	 * Gets whether character abilities have been replicated from the server at least once for the owning character.
	 *
	 * @see UPF2AbilitySystemComponent::OnRep_ActivateAbilities()
	 *
	 * @return
	 *	- true if OnRep_ActivateAbilities() has been called at least once with all abilities replicated.
	 *	- false if OnRep_ActivateAbilities() has either not been called or did not get a full snapshot of abilities
	 *	  from the server yet.
	 */
	FORCEINLINE bool AreAbilitiesAvailable() const
	{
		return this->bAreAbilitiesAvailable;
	}

	/**
	 * Gets or builds the list of all passive gameplay effects to activate, organized by weight group.
	 *
	 * The returned list includes all of the passive GEs that have been added to this GE as well as the dynamic tag GE.
	 *
	 * The list is cached, for performance reasons.
	 *
	 * @return
	 *	The map of passive GEs to activate, keyed by weight group.
	 */
	TMultiMap<FName, TSubclassOf<UGameplayEffect>> GetPassiveGameplayEffectsToApply();

	/**
	 * Builds the list of all passive gameplay effects to activate, organized by weight group.
	 *
	 * The returned list includes all of the passive GEs that have been added to this GE as well as the dynamic tag GE.
	 *
	 * The list is not cached.
	 *
	 * @return
	 *	The map of passive GEs to activate, keyed by weight group.
	 */
	TMultiMap<FName, TSubclassOf<UGameplayEffect>> BuildPassiveGameplayEffectsToApply() const;

	/**
	 * Activates a specific passive Gameplay Effect on this ASC.
	 *
	 * @param WeightGroup
	 *	The weight group for which the effect is being activated.
	 * @param GameplayEffect
	 *	The effect to activate.
	 */
	void ActivatePassiveGameplayEffect(
		const FName                        WeightGroup,
		const TSubclassOf<UGameplayEffect> GameplayEffect);

	/**
	 * Invokes the logic of the specified callable, with special handling if passive GEs are already active on this ASC.
	 *
	 * If passive GEs are active on this ASC before this call, they are deactivated; then the callable is invoked, and
	 * passive GEs are re-activated. If passive GEs are not active before this call, then they are not activated at the
	 * end of this call.
	 *
	 * @param Callable
	 *	A lambda that is invoked to perform the task.
	 */
	template<typename Func>
	void InvokeAndReapplyAllPassiveGEs(const Func Callable);

	/**
	 * Invokes the logic of the specified callable, then re-applies passive GEs in weight groups after it.
	 *
	 * The weight groups affected are determined by the default weight group of the given GE.
	 *
	 * If the default weight group is active, and passive GEs in weight groups after the specified weight group were
	 * active on this ASC before this call, they are deactivated; the callable is invoked; and then passive GEs in the
	 * subsequent weight groups are re-activated.
	 *
	 * If the default weight group is not active, or no passive GEs were active in subsequent weight groups before this
	 * call, no additional weight groups are activated at the end of this call.
	 *
	 * @param Effect
	 *	The GE that the callable is interacting with. The GE is used to dictate which weight group the callable is
	 *	affecting. All subsequent weight groups will be re-applied, if they are active.
	 * @param Callable
	 *	A lambda that is invoked to perform the task.
	 */
	template<typename Func>
	void InvokeAndReapplyPassiveGEsInSubsequentWeightGroups(
		const TSubclassOf<UGameplayEffect> Effect,
		const Func Callable);

	/**
	 * Invokes the logic of the specified callable, then re-applies passive GEs that were active in subsequent groups.
	 *
	 * If the specified weight group is active, and passive GEs in weight groups after the specified weight group were
	 * active on this ASC before this call, they are deactivated; the callable is invoked; and then passive GEs in the
	 * subsequent weight groups are re-activated.
	 *
	 * If the given weight group is not active, or no passive GEs were active in subsequent weight groups before this
	 * call, no additional weight groups are activated at the end of this call.
	 *
	 * @param WeightGroup
	 *	The weight group that the callable affects. If this weight group is currently active on this ASC, all subsequent
	 *	weight groups will be re-applied, if they are active.
	 * @param Callable
	 *	A lambda that is invoked to perform the task.
	 */
	template<typename Func>
	void InvokeAndReapplyPassiveGEsInSubsequentWeightGroups(const FName WeightGroup, const Func Callable);

	// =================================================================================================================
	// Protected Native Event Callbacks
	// =================================================================================================================
	/**
	 * Callback invoked in C++ code when character abilities have become available on the client.
	 *
	 * This notifies all event listeners that abilities have been replicated after a change remotely. Unlike native
	 * engine replication callbacks, this is only invoked after abilities have fully replicated; it will not be invoked
	 * if some abilities are null.
	 */
	virtual void Native_OnAbilitiesAvailable();
};
