// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <AbilitySystemComponent.h>

#include "PF2CharacterAbilitySystemInterface.h"

#include "PF2AbilitySystemComponent.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2AbilityBoostInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UCLASS(ClassGroup="OpenPF2-Characters")
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2AbilitySystemComponent :
	public UAbilitySystemComponent, public IPF2CharacterAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Fields - Multicast Delegates
	// =================================================================================================================
	/**
	 * Event fired to react to character abilities becoming available on the client.
	 *
	 * This event is not fired on the server. This can be used to listen to abilities that have been replicated after
	 * a change remotely. Unlike native engine replication callbacks, this is only invoked after abilities have fully
	 * replicated; it will not be invoked if some abilities are null.
	 */
	UPROPERTY(BlueprintAssignable, Category="OpenPF2|Components|Characters|Ability System")
	FPF2ClientAbilitiesChangeDelegate OnAbilitiesAvailable;

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
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
	UPF2AbilitySystemComponent();

	// =================================================================================================================
	// Public Methods - IPF2AbilitySystemInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual TScriptInterface<IPF2GameplayAbilityInterface> GetAbilityInstanceFromSpec(
		const FGameplayAbilitySpec& AbilitySpec) const override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<TScriptInterface<IPF2GameplayAbilityInterface>> GetAbilities() const override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<TScriptInterface<IPF2GameplayAbilityInterface>> GetAbilitiesByTags(
		const FGameplayTagContainer& Tags,
		bool bExactMatch = true) const override;

	virtual bool ArePassiveGameplayEffectsActive() const override
	{
		return this->ActivatedWeightGroups.Num() != 0;
	}

	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* ToAbilitySystemComponent() override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<FGameplayAbilitySpec> FindAbilitySpecsByTags(
		const FGameplayTagContainer& Tags,
		bool                         bOnlyAbilitiesThatSatisfyTagRequirements = true) const override;

	UFUNCTION(BlueprintCallable)
	virtual FGameplayAbilitySpec FindAbilitySpecByTags(
		UPARAM(DisplayName="Tags")
		const FGameplayTagContainer& InTags,

		UPARAM(DisplayName="Match Found")
		bool& OutMatchFound,

		UPARAM(DisplayName="Only Abilities that Satisfy Tag Requirements")
		const bool bInOnlyAbilitiesThatSatisfyTagRequirements = true
	) const override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<FGameplayAbilitySpecHandle> FindAbilityHandlesByTags(
		const FGameplayTagContainer& Tags,
		const bool                   bOnlyAbilitiesThatSatisfyTagRequirements = true
	) const override;

	UFUNCTION(BlueprintCallable)
	virtual FGameplayAbilitySpecHandle FindAbilityHandleByTags(
		UPARAM(DisplayName="Tags")
		const FGameplayTagContainer& InTags,

		UPARAM(DisplayName="Match Found")
		bool& OutMatchFound,

		UPARAM(DisplayName="Only Abilities that Satisfy Tag Requirements")
		const bool bInOnlyAbilitiesThatSatisfyTagRequirements = true
	) const override;

	UFUNCTION(BlueprintCallable)
	virtual bool TriggerAbilityWithPayload(const FGameplayAbilitySpecHandle AbilityHandle,
	                                       const FGameplayEventData         Payload) override;

	UFUNCTION(BlueprintCallable)
	virtual void AddPassiveGameplayEffect(TSubclassOf<UGameplayEffect> Effect) override;

	UFUNCTION(BlueprintCallable)
	virtual void AddPassiveGameplayEffectWithWeight(
		const FName WeightGroup,
		const TSubclassOf<UGameplayEffect> Effect
	) override;

	virtual void SetPassiveGameplayEffects(const TMultiMap<FName, TSubclassOf<UGameplayEffect>> Effects) override;

	UFUNCTION(BlueprintCallable)
	virtual void RemoveAllPassiveGameplayEffects() override;

	UFUNCTION(BlueprintCallable)
	virtual void ActivateAllPassiveGameplayEffects() override;

	UFUNCTION(BlueprintCallable)
	virtual void DeactivateAllPassiveGameplayEffects() override;

	UFUNCTION(BlueprintCallable)
	virtual TSet<FName> ActivatePassiveGameplayEffectsAfter(const FName StartingWeightGroup) override;

	UFUNCTION(BlueprintCallable)
	virtual TSet<FName> DeactivatePassiveGameplayEffectsAfter(const FName StartingWeightGroup) override;

	UFUNCTION(BlueprintCallable)
	virtual bool ActivatePassiveGameplayEffects(const FName WeightGroup) override;

	UFUNCTION(BlueprintCallable)
	virtual bool DeactivatePassiveGameplayEffects(const FName WeightGroup) override;

	UFUNCTION(BlueprintCallable)
	virtual void AddDynamicTag(const FGameplayTag Tag) override;

	UFUNCTION(BlueprintCallable)
	virtual void AppendDynamicTags(const FGameplayTagContainer Tags) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetDynamicTags(const FGameplayTagContainer Tags) override;

	UFUNCTION(BlueprintCallable)
	virtual void RemoveDynamicTag(const FGameplayTag Tag) override;

	UFUNCTION(BlueprintCallable)
	virtual void RemoveDynamicTags(const FGameplayTagContainer Tags) override;

	UFUNCTION(BlueprintCallable)
	virtual void RemoveAllDynamicTags() override;

	UFUNCTION(BlueprintCallable)
	virtual FGameplayTagContainer GetActiveGameplayTags() const override;

	virtual FPF2ClientAbilitiesChangeDelegate* GetClientAbilityChangeDelegate() override;

	// =================================================================================================================
	// Public Methods - IPF2CharacterAbilitySystemInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual TScriptInterface<IPF2CharacterInterface> GetCharacter() const override;

	UFUNCTION(BlueprintCallable)
	virtual int32 GetCharacterLevel() const override;

	UFUNCTION(BlueprintCallable)
	virtual TMap<EPF2CharacterAbilityScoreType, FPF2AttributeModifierSnapshot> GetAbilityScoreValues() const override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<TScriptInterface<IPF2AbilityBoostInterface>> GetPendingAbilityBoosts() const override;

	UFUNCTION(BlueprintCallable)
	virtual FORCEINLINE TSubclassOf<UGameplayEffect> GetBoostEffectForAbility(
		const EPF2CharacterAbilityScoreType AbilityScore) override
	{
		return this->AbilityBoostEffects[AbilityScore];
	}

	UFUNCTION(BlueprintCallable)
	virtual void ApplyAbilityBoost(const EPF2CharacterAbilityScoreType TargetAbilityScore) override;

	// =================================================================================================================
	// Public Methods - IPF2ActorComponentInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual UActorComponent* ToActorComponent() override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
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
