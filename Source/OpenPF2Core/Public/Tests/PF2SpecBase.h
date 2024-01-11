// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayEffect.h>

#include <Containers/Array.h>

#include <Engine/World.h>

#include "Tests/PF2AutomationSpecBase.h"
#include "Tests/PF2TestPawn.h"

#include "Utilities/PF2ArrayUtilities.h"

// =====================================================================================================================
// Macro Declarations
// =====================================================================================================================
#define DEFINE_PF_SPEC_PRIVATE(TClass, PrettyName, TFlags, FileName, LineNumber) \
	class TClass : public FPF2SpecBase \
	{ \
	public: \
		TClass(const FString& InName) : FPF2SpecBase(InName) \
		{ \
			static_assert((TFlags)&EAutomationTestFlags::ApplicationContextMask, "AutomationTest has no application flag.  It shouldn't run.  See AutomationTest.h."); \
			static_assert((((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::SmokeFilter) || \
						  (((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::EngineFilter) || \
						  (((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::ProductFilter) || \
						  (((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::PerfFilter) || \
						  (((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::StressFilter) || \
						  (((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::NegativeFilter), \
						  "All AutomationTests must have exactly 1 filter type specified.  See AutomationTest.h."); \
		} \
		virtual uint32 GetTestFlags() const override { return TFlags; } \
        using FPF2AutomationSpecBase::GetTestSourceFileName; \
		virtual FString GetTestSourceFileName() const override { return FileName; } \
        using FPF2AutomationSpecBase::GetTestSourceFileLine; \
		virtual int32 GetTestSourceFileLine() const override { return LineNumber; } \
		virtual FString GetTestSourceFileName(const FString&) const override { return GetTestSourceFileName(); } \
		virtual int32 GetTestSourceFileLine(const FString&) const override { return GetTestSourceFileLine(); } \
	\
	protected: \
		virtual FString GetBeautifiedTestName() const override { return PrettyName; } \
		virtual void Define() override; \
	};

#define BEGIN_DEFINE_PF_SPEC_PRIVATE(TClass, PrettyName, TFlags, FileName, LineNumber) \
	class TClass : public FPF2SpecBase \
	{ \
	public: \
		TClass(const FString& InName) : FPF2SpecBase(InName) \
		{ \
			static_assert((TFlags)&EAutomationTestFlags::ApplicationContextMask, "AutomationTest has no application flag.  It shouldn't run.  See AutomationTest.h."); \
			static_assert((((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::SmokeFilter) || \
						  (((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::EngineFilter) || \
						  (((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::ProductFilter) || \
						  (((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::PerfFilter) || \
						  (((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::StressFilter) || \
						  (((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::NegativeFilter), \
						  "All AutomationTests must have exactly 1 filter type specified.  See AutomationTest.h."); \
		} \
		virtual uint32 GetTestFlags() const override { return TFlags; } \
		using FPF2AutomationSpecBase::GetTestSourceFileName; \
		virtual FString GetTestSourceFileName() const override { return FileName; } \
		using FPF2AutomationSpecBase::GetTestSourceFileLine; \
		virtual int32 GetTestSourceFileLine() const override { return LineNumber; } \
	\
	protected: \
		virtual FString GetBeautifiedTestName() const override { return PrettyName; } \
		virtual void Define() override;

#if WITH_AUTOMATION_WORKER
	#define DEFINE_PF_SPEC(TClass, PrettyName, TFlags) \
		DEFINE_PF_SPEC_PRIVATE(TClass, PrettyName, TFlags, __FILE__, __LINE__) \
		namespace\
		{\
			TClass TClass##AutomationSpecInstance(TEXT(#TClass));\
		}

	#define BEGIN_DEFINE_PF_SPEC(TClass, PrettyName, TFlags) \
		BEGIN_DEFINE_PF_SPEC_PRIVATE(TClass, PrettyName, TFlags, __FILE__, __LINE__)

	#define END_DEFINE_PF_SPEC(TClass) \
		};\
		namespace\
		{\
			TClass TClass##AutomationSpecInstance(TEXT(#TClass));\
		}
#else
	#define DEFINE_PF_SPEC(TClass, PrettyName, TFlags) \
		DEFINE_PF_SPEC_PRIVATE(TClass, PrettyName, TFlags, __FILE__, __LINE__)

	#define BEGIN_DEFINE_PF_SPEC(TClass, PrettyName, TFlags) \
		BEGIN_DEFINE_PF_SPEC_PRIVATE(TClass, PrettyName, TFlags, __FILE__, __LINE__)

	#define END_DEFINE_PF_SPEC(TClass) \
		};

#endif

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class APF2TestCharacter;
class FPF2TestSessionState;
class IPF2CharacterInterface;
class UPF2CharacterAttributeSet;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
using FAttributeCapture = TMap<FString, FGameplayAttributeData*>;

/**
 * Base class that provides reusable, boilerplate logic for automation specs in OpenPF2.
 */
class OPENPF2CORE_API FPF2SpecBase : public FPF2AutomationSpecBase
{
protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * A throwaway world for use in tests.
	 */
	UWorld* World;

	/**
	 * A simple pawn that has an ability system component, for use in ASC tests.
	 *
	 * This is initialized by SetupTestPawn() and torn down by DestroyTestPawn().
	 *
	 * @see PF2SpecBase::SetupTestPawn()
	 * @see PF2SpecBase::DestroyTestPawn()
	 */
	APF2TestPawn* TestPawn;

	/**
	 * The ASC of the test pawn.
	 *
	 * This is initialized by SetupTestPawn() and torn down by DestroyTestPawn().
	 *
	 * @see PF2SpecBase::SetupTestPawn()
	 * @see PF2SpecBase::DestroyTestPawn()
	 */
	UAbilitySystemComponent* TestPawnAsc;

	/**
	 * A simple pawn that has an ability system component, for use in ASC tests.
	 *
	 * This is initialized by SetupTestCharacter() and torn down by DestroyTestCharacter().
	 *
	 * @see PF2SpecBase::SetupTestCharacter()
	 * @see PF2SpecBase::DestroyTestCharacter()
	 */
	TScriptInterface<IPF2CharacterInterface> TestCharacter;

	/**
	 * The ASC of the test character.
	 *
	 * This is initialized by SetupTestCharacter() and torn down by DestroyTestCharacter().
	 *
	 * @see PF2SpecBase::SetupTestCharacter()
	 * @see PF2SpecBase::DestroyTestCharacter()
	 */
	UAbilitySystemComponent* TestCharacterAsc;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Constructor for FPF2SpecBase.
	 *
	 * @param InName
	 *	The name of the test, for reporting in the session frontend.
	 */
	explicit FPF2SpecBase(const FString& InName);

protected:
	// =================================================================================================================
	// Protected Methods - FFP2AutomationSpecBase Overrides
	// =================================================================================================================
	virtual void Define() override = 0;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Instantiates a blueprint for use in a C++ test.
	 *
	 * @tparam BlueprintType
	 *	The concrete type of blueprint to load.
	 *
	 * @param FolderPath
	 *	The path to the folder that contains the blueprint being loaded.
	 * @param BlueprintName
	 *	The name of the blueprint to load, as it appears in the Unreal editor.
	 *
	 * @return The loaded blueprint.
	 */
	template <typename BlueprintType>
	static TSubclassOf<BlueprintType> LoadBlueprint(const FString FolderPath, const FString BlueprintName)
	{
		const FString ObjectPath =
			FString::Printf(TEXT("BlueprintGeneratedClass'%s/%s.%s_C'"), *FolderPath, *BlueprintName, *BlueprintName);

		const TSoftClassPtr<BlueprintType> ObjectClass =
			TSoftClassPtr<BlueprintType>(FSoftObjectPath(ObjectPath));

		return ObjectClass.LoadSynchronous();
	}

	/**
	 * Captures all attribute values of the provided character attribute set.
	 *
	 * @param AttributeSet
	 *	The attribute set from which to capture values.
	 *
	 * @return
	 *	A map from attribute machine names to attribute values.
	 */
	static FAttributeCapture CaptureAttributes(const UPF2CharacterAttributeSet* AttributeSet);

	/**
	 * Captures all ability attribute values of the provided character attribute set.
	 *
	 * @param AttributeSet
	 *	The attribute set from which to capture values.
	 *
	 * @return
	 *	A map from ability attribute machine names to attribute values.
	 */
	static FAttributeCapture CaptureAbilityAttributes(const UPF2CharacterAttributeSet* AttributeSet);

	/**
	 * Captures all ability modifier attribute values of the provided character attribute set.
	 *
	 * @param AttributeSet
	 *	The attribute set from which to capture values.
	 *
	 * @return
	 *	A map from ability modifier attribute machine names to attribute values.
	 */
	static FAttributeCapture CaptureAbilityModifierAttributes(const UPF2CharacterAttributeSet* AttributeSet);

	/**
	 * Captures all saving throw modifier attribute values of the provided character attribute set.
	 *
	 * @param AttributeSet
	 *	The attribute set from which to capture values.
	 *
	 * @return
	 *	A map from saving throw modifier attribute machine names to attribute values.
	 */
	static FAttributeCapture CaptureSavingThrowModifierAttributes(const UPF2CharacterAttributeSet* AttributeSet);

	/**
	 * Captures all skill modifier attribute values of the provided character attribute set.
	 *
	 * @param AttributeSet
	 *	The attribute set from which to capture values.
	 *
	 * @return
	 *	A map from skill modifier attribute machine names to attribute values.
	 */
	static FAttributeCapture CaptureSkillModifierAttributes(const UPF2CharacterAttributeSet* AttributeSet);

	/**
	 * Captures spell attribute values of the provided character attribute set.
	 *
	 * @param AttributeSet
	 *	The attribute set from which to capture values.
	 *
	 * @return
	 *	A map from spell attribute machine names to attribute values.
	 */
	static FAttributeCapture CaptureSpellAttributes(const UPF2CharacterAttributeSet* AttributeSet);

	/**
	 * Grants the specified character a "fake" Gameplay Ability and returns its handle.
	 *
	 * @param Character
	 *	The character to which the ability will be granted.
	 * @param DynamicAbilityTags
	 *	Optional tags to apply to the ability that will contribute toward the source tags of any gameplay effects that
	 *	the ability applies.
	 *
	 * @return
	 *	The server-side handle of the granted ability.
	 */
	static FGameplayAbilitySpecHandle GrantCharacterFakeAbility(
		TScriptInterface<IPF2CharacterInterface> Character,
		const FGameplayTagContainer&             DynamicAbilityTags = FGameplayTagContainer());

	/**
	 * Grants the specified character the specified Gameplay Ability and returns its handle.
	 *
	 * @param Character
	 *	The character to which the ability will be granted.
	 * @param AbilityType
	 *	The type of ability to grant.
	 * @param DynamicAbilityTags
	 *	Optional tags to apply to the ability that will contribute toward the source tags of any gameplay effects that
	 *	the ability applies.
	 *
	 * @return
	 *	The server-side handle of the granted ability.
	 */
	static FGameplayAbilitySpecHandle GrantCharacterAbility(
		TScriptInterface<IPF2CharacterInterface> Character,
		TSubclassOf<UGameplayAbility>            AbilityType,
		const FGameplayTagContainer&             DynamicAbilityTags = FGameplayTagContainer());

	/**
	 * Builds a Gameplay Effect (GE) specification for the given type of GE.
	 *
	 * @param EffectClass
	 *	The type of GE for which a spec is desired.
	 * @param SetByCallerMagnitudesMap
	 *	An optional map of set-by-caller-magnitudes/parameters to parameters, to pass into the GE.
	 *
	 * @return
	 *	The server-side handle of the GE specification.
	 */
	FGameplayEffectSpecHandle BuildEffectSpec(
		const TSubclassOf<UGameplayEffect> EffectClass,
		TMap<FName, float>                 SetByCallerMagnitudesMap = {}) const;

	/**
	 * Builds a Gameplay Effect (GE) specification for the given GE type, instigated by the given ability and character.
	 *
	 * @param EffectClass
	 *	The type of GE for which a spec is desired.
	 * @param Instigator
	 *	The character to spoof as being responsible for the effect (e.g., the source of damage).
	 * @param InvokingAbility
	 *	The ability to spoof as having initiated the effect (e.g., the attack ability).
	 * @param SetByCallerMagnitudesMap
	 *	An optional map of set-by-caller-magnitudes/parameters to parameters, to pass into the GE.
	 *
	 * @return
	 *	The server-side handle of the GE specification.
	 */
	static FGameplayEffectSpecHandle BuildEffectSpec(
		const TSubclassOf<UGameplayEffect>             EffectClass,
		const TScriptInterface<IPF2CharacterInterface> Instigator,
		const UGameplayAbility*                        InvokingAbility,
		const TMap<FName, float>&                      SetByCallerMagnitudesMap = {});

	/**
	 * Builds a Gameplay Effect (GE) specification for the given GE type, instigated by the given ability and character.
	 *
	 * @param EffectClass
	 *	The type of GE for which a spec is desired.
	 * @param Instigator
	 *	The character to spoof as being responsible for the effect (e.g., the source of damage).
	 * @param InvokingAbility
	 *	The ability to spoof as having initiated the effect (e.g., the attack ability).
	 * @param EffectCauser
	 *	The actor inflicting damage on the target (e.g., a projectile or melee weapon).
	 * @param SetByCallerMagnitudesMap
	 *	An optional map of set-by-caller-magnitudes/parameters to parameters, to pass into the GE.
	 *
	 * @return
	 *	The server-side handle of the GE specification.
	 */
	static FGameplayEffectSpecHandle BuildEffectSpec(
		const TSubclassOf<UGameplayEffect>             EffectClass,
		const TScriptInterface<IPF2CharacterInterface> Instigator,
		const UGameplayAbility*                        InvokingAbility,
		AActor*                                        EffectCauser,
		const TMap<FName, float>&                      SetByCallerMagnitudesMap = {});

	/**
	 * Creates the throwaway world for use in tests.
	 */
	void SetupWorld();

	/**
	 * Begins simulation of the world, initializing components and actors and allowing them to settle.
	 */
	void BeginPlay() const;

	/**
	 * Tears down the throwaway world and forces garbage collection of it.
	 */
	void DestroyWorld() const;

	/**
	 * Initializes this test with a simple pawn in this->TestPawn that has an ability system component.
	 */
	void SetupTestPawn();

	/**
	 * Tears down the simple pawn.
	 */
	void DestroyTestPawn();

	/**
	 * Initializes this test with OpenPF2-compatible character in this->TestCharacter.
	 */
	void SetupTestCharacter();

	/**
	 * Tears down the OpenPF2-compatible character.
	 */
	void DestroyTestCharacter();

	/**
	 * Spawns an actor component on the test pawn and returns it.
	 *
	 * @tparam ActorComponentType
	 *	The type of actor component to instantiate.
	 *
	 * @return
	 *	The new actor component instance.
	 */
	template<class ActorComponentType>
	ActorComponentType* SpawnActorComponent()
	{
		ActorComponentType* Component = NewObject<ActorComponentType>(this->TestPawn);

		this->TestPawn->AddInstanceComponent(Component);
		Component->RegisterComponent();

		return Component;
	}

	/**
	 * Spawns a lightweight test pawn with an ability system component and then returns it.
	 *
	 * @return
	 *	The new character instance.
	 */
	APF2TestPawn* SpawnPawn() const;

	/**
	 * Spawns an OpenPF2-compatible test character and returns it.
	 *
	 * @return
	 *	The new character instance.
	 */
	IPF2CharacterInterface* SpawnCharacter() const;

	/**
	 * Initializes an attribute with a starting value and then applies a Gameplay Effect (GE) that modifies it.
	 *
	 * @param Attribute
	 *	A reference to the attribute.
	 * @param StartingValue
	 *	The initial value to which the attribute will be set.
	 * @param EffectBP
	 *	The GE blueprint to apply in order to affect the attribute.
	 * @param CharacterLevel
	 *	The level of the character, used to control the ability level.
	 *
	 * @return
	 *	A handle to the active GE.
	 */
	FActiveGameplayEffectHandle InitializeAttributeAndApplyEffect(
		FGameplayAttributeData&             Attribute,
		float                               StartingValue,
		const TSubclassOf<UGameplayEffect>& EffectBP,
		const float                         CharacterLevel = 1.0f) const;

	/**
	 * Applies a "loose", unreplicated Gameplay Tag to the test pawn.
	 *
	 * The tag is "loose" in the sense that it is not being applied by an active Gameplay Effect on the ASC of the test
	 * pawn.
	 *
	 * @param TagName
	 *	The name of the tag to apply.
	 */
	void ApplyUnreplicatedTag(const FString& TagName) const;

	/**
	 * Removes a "loose", unreplicated Gameplay Tag from the test pawn.
	 *
	 * @param TagName
	 *	The name of the tag to remove.
	 */
	void RemoveUnreplicatedTag(const FString& TagName) const;

	/**
	 * Tests that each element of an array matches its corresponding element from expected values.
	 *
	 * This provides better output on failure than TestEqual().
	 *
	 * @tparam ElementType
	 *	The type of elements in the array.
	 *
	 * @param What
	 *	The name of the array being tested.
	 * @param Actual
	 *	The actual array.
	 * @param Expected
	 *	The expected elements of the array.
	 */
	template <typename ElementType>
	void TestArrayEquals(const FString& What, const TArray<ElementType>& Actual, const TArray<ElementType>& Expected)
	{
		this->TestEqual(
			FString::Format(TEXT("{0}.Num()"), {What}),
			Actual.Num(),
			Expected.Num()
		);

		for (auto It = Expected.CreateConstIterator(); It; ++It)
		{
			int               Index            = It.GetIndex();
			const ElementType &ExpectedElement = *It,
			                  &ActualElement   = Actual[Index];

			this->TestEqual(
				FString::Format(TEXT("{0}[{1}]"), {What, FString::FormatAsNumber(Index)}),
				ActualElement,
				ExpectedElement
			);
		}
	}

	/**
	 * Tests that at least one element of an array does not match expected values.
	 *
	 * This provides better output on failure than TestEqual().
	 *
	 * @tparam ElementType
	 *	The type of elements in the array.
	 *
	 * @param What
	 *	The name of the array being tested.
	 * @param Actual
	 *	The actual array.
	 * @param NotExpected
	 *	A second array to which the actual array is not expected to be equal.
	 */
	template <typename ElementType>
	void TestArrayNotEquals(const FString&             What,
	                        const TArray<ElementType>& Actual,
	                        const TArray<ElementType>& NotExpected)
	{
		bool bAtLeastOneMismatch = false;

		if (Actual.Num() != NotExpected.Num())
		{
			bAtLeastOneMismatch = true;
		}
		else
		{
			for (auto It = NotExpected.CreateConstIterator(); It; ++It)
			{
				int               Index               = It.GetIndex();
				const ElementType &NotExpectedElement = *It,
				                  &ActualElement      = Actual[Index];

				if (ActualElement != NotExpectedElement)
				{
					bAtLeastOneMismatch = true;
					break;
				}
			}
		}

		if (!bAtLeastOneMismatch)
		{
			const FString &ActualString      = ArrayToString(Actual),
			              &NotExpectedString = ArrayToString(NotExpected);

			this->AddError(
				FString::Printf(
					TEXT("Expected %s('%s') to not equal '%s'."),
					*What,
					*ActualString,
					*NotExpectedString
				),
				1
			);
		}
	}

	/**
	 * Converts an array of 32-bit integers to a comma-delimited string.
	 *
	 * @tparam ElementType
	 *	The type of elements in the array.
	 *
	 * @param Elements
	 *	The array to convert into a string.
	 *
	 * @return
	 *	A comma-delimited string containing the elements of the array.
	 */
	static FString ArrayToString(const TArray<int32>& Elements)
	{
		const TArray<FString>& StringElements =
			PF2ArrayUtilities::Map<FString, int32>(Elements, [](const int32 Element) {
				return FString::FromInt(Element);
			});

		return ArrayToString(StringElements);
	}

	/**
	 * Converts an array of strings to a comma-delimited string.
	 *
	 * @tparam ElementType
	 *	The type of elements in the array.
	 *
	 * @param Elements
	 *	The array to convert into a string.
	 *
	 * @return
	 *	A comma-delimited string containing the elements of the array.
	 */
	static FString ArrayToString(const TArray<FString>& Elements)
	{
		return FString::Printf(TEXT("{%s}"), *FString::Join(Elements, TEXT(",")));
	}
};
