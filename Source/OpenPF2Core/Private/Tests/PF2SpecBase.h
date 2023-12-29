// Copyright 2021-2023 Guy Elsmore-Paddock. All Rights Reserved.
// Adapted from content that is Copyright Epic Games, Inc. (Action RPG Sample).
// Licensed only for use with Unreal Engine.

#pragma once

#include <Containers/Array.h>

#include <Engine/World.h>

#include <Misc/AutomationTest.h>

#include "Abilities/PF2CharacterAttributeSet.h"

#include "Tests/PF2TestPawn.h"

#include "Utilities/PF2ArrayUtilities.h"

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
        using FAutomationSpecBase::GetTestSourceFileName; \
		virtual FString GetTestSourceFileName() const override { return FileName; } \
        using FAutomationSpecBase::GetTestSourceFileLine; \
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
		using FAutomationSpecBase::GetTestSourceFileName; \
		virtual FString GetTestSourceFileName() const override { return FileName; } \
		using FAutomationSpecBase::GetTestSourceFileLine; \
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

using FAttributeCapture = TMap<FString, FGameplayAttributeData*>;

/**
 * Base class that provides reusable, boilerplate logic for automation specs in OpenPF2.
 */
class OPENPF2CORE_API FPF2SpecBase : public FAutomationSpecBase
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
	 */
	APF2TestPawn* TestPawn;

	/**
	 * The ASC of the test pawn.
	 */
	UAbilitySystemComponent* TestPawnAsc;

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
	explicit FPF2SpecBase(const FString& InName) :
		FAutomationSpecBase(InName, false),
		World(nullptr),
		TestPawn(nullptr),
		TestPawnAsc(nullptr)
	{
	}

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	virtual void Define() override = 0;

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
	 * Initializes a simple pawn that has an ability system component, for use in ASC tests.
	 */
	void SetupPawn();

	/**
	 * Tears down the simple pawn that has an ability system component, for use in ASC tests.
	 */
	void DestroyPawn();

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
	 * Spawns a test character and returns it.
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
				FString::Printf(TEXT("Expected '%s' to not equal '%s'."), *ActualString, *NotExpectedString),
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
