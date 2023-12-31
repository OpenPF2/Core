// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayEffect.h>

#include <Containers/Array.h>

#include <Engine/World.h>

#include "Tests/PF2AutomationTest.h"
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

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class APF2TestCharacter;
class IPF2CharacterInterface;
class UPF2CharacterAttributeSet;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
using FAttributeCapture = TMap<FString, FGameplayAttributeData*>;

/**
 * Base class that provides reusable, boilerplate logic for automation specs in OpenPF2.
 */
class OPENPF2CORE_API FPF2SpecBase : public FAutomationSpecBase
{
protected:
	// =================================================================================================================
	// Protected Types
	// =================================================================================================================
	struct FSpecDefinitionScopeEx : FSpecDefinitionScope
	{
		TArray<TSharedRef<IAutomationLatentCommand>> BeforeAll;
		TArray<TSharedRef<IAutomationLatentCommand>> AfterAll;
	};

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
	explicit FPF2SpecBase(const FString& InName) :
		FAutomationSpecBase(InName, false),
		World(nullptr),
		TestPawn(nullptr),
		TestPawnAsc(nullptr),
		TestCharacter(nullptr),
		TestCharacterAsc(nullptr)
	{
	}

protected:
	// =================================================================================================================
	// Protected Methods - FAutomationSpecBase Overrides
	// =================================================================================================================
	virtual bool RunTest(const FString& InParameters) override;
	virtual void GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const override;

	// Needed to avoid our overload hiding the base class implementation.
	FORCEINLINE void Describe(const FString& InDescription, const TFunction<void()>& DoWork)
	{
		FAutomationSpecBase::Describe(InDescription, DoWork);
	}

	// Needed to avoid our overload hiding the base class implementation.
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	FORCEINLINE void xDescribe(const FString& InDescription, const TFunction<void()>& DoWork)
	{
		FAutomationSpecBase::xDescribe(InDescription, DoWork);
	}

	virtual void Define() override = 0;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the current spec definition scope, including all the fields defined by version 2 of the scope structure.
	 *
	 * @return
	 *	The current, version 2 spec definition scope.
	 */
	TSharedRef<FSpecDefinitionScopeEx> GetCurrentV2Scope()
	{
		const TSharedRef<FSpecDefinitionScope> BaseScope = DefinitionScopeStack.Last();

		checkf(
			BaseScope->SpecDefinitionVersion == 2,
			TEXT("This function can only be used with a Describe() block that has opted for version 2 of the spec definition structure.")
		);

		return StaticCastSharedRef<FSpecDefinitionScopeEx>(BaseScope);
	}

	/**
	 * Defines a new scope for expectations, such that they are more readable and DRYer.
	 *
	 * Describe() is used as a way to scope complicated expectations. Using Describe() makes your code DRYer based on
	 * the interaction it has with other supporting functions such as BeforeEach(), BeforeAll(), AfterEach(), and
	 * AfterAll().
	 *
	 * You can cascade Describe() by putting a Describe() in another Describe().
	 *
	 * Keep in mind that Describe() is not a test and is not executed during an actual test run. They are only executed
	 * once when first defining the expectations (or tests) within the Spec.
	 *
	 * @param InDescription
	 *	A string that describes the scope of the expectations within it.
	 * @param SpecVersion
	 *	The version of the spec definition structure to use for the new scope. "2" is recommended, especially if you
	 *	will be using BeforeAll() blocks.
	 * @param DoWork
	 *	A lambda that defines the expectations within this scope.
	 */
	void Describe(const FString& InDescription, const uint8 SpecVersion, const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of Describe().
	 *
	 * @see FPF2SpecBase::Describe(const FString&, const uint8, const TFunction<void()>&)
	 *
	 * @param InDescription
	 *	A string that describes the scope of the expectations within it.
	 * @param SpecVersion
	 *	The version of the spec definition structure to use for the new scope. "2" is recommended, especially if you
	 *	will be using BeforeAll() blocks.
	 * @param DoWork
	 *	A lambda that defines the expectations within this scope.
	 */
	void xDescribe(const FString& InDescription, const uint8 SpecVersion, const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run before the first It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined. Each BeforeAll() block will only execute
	 * before the first It() block of the scope in which it has been defined.
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void BeforeAll(const TFunction<void()>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->BeforeAll.Push(
			MakeShareable(new FSingleExecuteLatentCommand(this, DoWork, this->bEnableSkipIfError))
		);
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of BeforeAll().
	 *
	 * @see FPF2SpecBase::BeforeAll(const TFunction<void()>&)
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void xBeforeAll(const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run asynchronous before the first It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined. Each BeforeAll() block will only execute
	 * before the first It() block of the scope in which it has been defined.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void BeforeAll(const EAsyncExecution Execution, const TFunction<void()>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->BeforeAll.Push(
			MakeShareable(new FAsyncLatentCommand(this, Execution, DoWork, DefaultTimeout, this->bEnableSkipIfError))
		);
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of BeforeAll().
	 *
	 * @see FPF2SpecBase::BeforeAll(const EAsyncExecution, const TFunction<void()>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void xBeforeAll(const EAsyncExecution Execution, const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run asynchronously before the first It() block of the current scope, with a timeout.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined. Each BeforeAll() block will only execute
	 * before the first It() block of the scope in which it has been defined.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void BeforeAll(const EAsyncExecution Execution, const FTimespan& Timeout, const TFunction<void()>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->BeforeAll.Push(
			MakeShareable(new FAsyncLatentCommand(this, Execution, DoWork, Timeout, this->bEnableSkipIfError))
		);
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of BeforeAll().
	 *
	 * @see FPF2SpecBase::BeforeAll(const EAsyncExecution, const FTimespan&, const TFunction<void()>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void xBeforeAll(const EAsyncExecution Execution, const FTimespan& Timeout, const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code over multiple frames that must run before the first It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined. Each BeforeAll() block will only execute
	 * before the first It() block of the scope in which it has been defined.
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void LatentBeforeAll(const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->BeforeAll.Push(
			MakeShareable(new FUntilDoneLatentCommand(this, DoWork, DefaultTimeout, this->bEnableSkipIfError))
		);
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeAll().
	 *
	 * @see FPF2SpecBase::LatentBeforeAll(const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void xLatentBeforeAll(const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code over multiple frames that must run before the first It() block of the current scope, with a timeout.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined. Each BeforeAll() block will only execute
	 * before the first It() block of the scope in which it has been defined.
	 *
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void LatentBeforeAll(const FTimespan& Timeout, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->BeforeAll.Push(
			MakeShareable(new FUntilDoneLatentCommand(this, DoWork, Timeout, this->bEnableSkipIfError))
		);
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeAll().
	 *
	 * @see FPF2SpecBase::LatentBeforeAll(const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void xLatentBeforeAll(const FTimespan& Timeout, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines async. code over multiple frames that must run before the first It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined. Each BeforeAll() block will only execute
	 * before the first It() block of the scope in which it has been defined.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void LatentBeforeAll(const EAsyncExecution Execution, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->BeforeAll.Push(
			MakeShareable(
				new FAsyncUntilDoneLatentCommand(this, Execution, DoWork, DefaultTimeout, this->bEnableSkipIfError)
			)
		);
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeAll().
	 *
	 * @see FPF2SpecBase::LatentBeforeAll(const EAsyncExecution, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void xLatentBeforeAll(const EAsyncExecution Execution, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines async. code over multiple frames that must run before the first It() block of the current scope, with a
	 * timeout.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined. Each BeforeAll() block will only execute
	 * before the first It() block of the scope in which it has been defined.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void LatentBeforeAll(const EAsyncExecution                        Execution,
	                     const FTimespan&                             Timeout,
	                     const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->BeforeAll.Push(
			MakeShareable(new FAsyncUntilDoneLatentCommand(this, Execution, DoWork, Timeout, this->bEnableSkipIfError))
		);
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeAll().
	 *
	 * @see FPF2SpecBase::LatentBeforeAll(const EAsyncExecution, const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void xLatentBeforeAll(const EAsyncExecution                        Execution,
	                      const FTimespan&                             Timeout,
	                      const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run after the last It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both AfterAll() and AfterEach() blocks, the
	 * BeforeAll() blocks will be evaluated after any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * AfterAll() only affects the Describe() scope in which it is defined. Each AfterAll() block will only execute
	 * after the last It() block of the scope in which it has been defined.
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void AfterAll(const TFunction<void()>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->AfterAll.Push(MakeShareable(new FSingleExecuteLatentCommand(this, DoWork)));
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of AfterAll().
	 *
	 * @see PF2SpecBase::AfterAll(const TFunction<void()>&)
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void xAfterAll(const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run after the last It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both AfterAll() and AfterEach() blocks, the
	 * BeforeAll() blocks will be evaluated after any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * AfterAll() only affects the Describe() scope in which it is defined. Each AfterAll() block will only execute
	 * after the last It() block of the scope in which it has been defined.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void AfterAll(const EAsyncExecution Execution, const TFunction<void()>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->AfterAll.Push(MakeShareable(new FAsyncLatentCommand(this, Execution, DoWork, DefaultTimeout)));
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of AfterAll().
	 *
	 * @see FPF2SpecBase::AfterAll(const EAsyncExecution, const TFunction<void()>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void xAfterAll(const EAsyncExecution Execution, const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run after the last It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both AfterAll() and AfterEach() blocks, the
	 * BeforeAll() blocks will be evaluated after any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * AfterAll() only affects the Describe() scope in which it is defined. Each AfterAll() block will only execute
	 * after the last It() block of the scope in which it has been defined.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void AfterAll(const EAsyncExecution Execution, const FTimespan& Timeout, const TFunction<void()>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->AfterAll.Push(MakeShareable(new FAsyncLatentCommand(this, Execution, DoWork, Timeout)));
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of AfterAll().
	 *
	 * @see FPF2SpecBase::AfterAll(const EAsyncExecution, const FTimespan&, const TFunction<void()>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void xAfterAll(const EAsyncExecution Execution, const FTimespan& Timeout, const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run after the last It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both AfterAll() and AfterEach() blocks, the
	 * BeforeAll() blocks will be evaluated after any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * AfterAll() only affects the Describe() scope in which it is defined. Each AfterAll() block will only execute
	 * after the last It() block of the scope in which it has been defined.
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void LatentAfterAll(const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->AfterAll.Push(MakeShareable(new FUntilDoneLatentCommand(this, DoWork, DefaultTimeout)));
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentAfterAll().
	 *
	 * @see FPF2SpecBase::LatentAfterAll(const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void xLatentAfterAll(const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run after the last It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both AfterAll() and AfterEach() blocks, the
	 * BeforeAll() blocks will be evaluated after any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * AfterAll() only affects the Describe() scope in which it is defined. Each AfterAll() block will only execute
	 * after the last It() block of the scope in which it has been defined.
	 *
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void LatentAfterAll(const FTimespan& Timeout, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->AfterAll.Push(MakeShareable(new FUntilDoneLatentCommand(this, DoWork, Timeout)));
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentAfterAll().
	 *
	 * @see FPF2SpecBase::LatentAfterAll(const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void xLatentAfterAll(const FTimespan& Timeout, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run after the last It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both AfterAll() and AfterEach() blocks, the
	 * BeforeAll() blocks will be evaluated after any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * AfterAll() only affects the Describe() scope in which it is defined. Each AfterAll() block will only execute
	 * after the last It() block of the scope in which it has been defined.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void LatentAfterAll(const EAsyncExecution Execution, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->AfterAll.Push(
			MakeShareable(new FAsyncUntilDoneLatentCommand(this, Execution, DoWork, DefaultTimeout))
		);
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentAfterAll().
	 *
	 * @see FPF2SpecBase::LatentAfterAll(const EAsyncExecution, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void xLatentAfterAll(const EAsyncExecution Execution, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run after the last It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both AfterAll() and AfterEach() blocks, the
	 * BeforeAll() blocks will be evaluated after any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * AfterAll() only affects the Describe() scope in which it is defined. Each AfterAll() block will only execute
	 * after the last It() block of the scope in which it has been defined.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void LatentAfterAll(const EAsyncExecution                        Execution,
	                    const FTimespan&                             Timeout,
	                    const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = this->GetCurrentV2Scope();

		CurrentScope->AfterAll.Push(MakeShareable(new FAsyncUntilDoneLatentCommand(this, Execution, DoWork, Timeout)));
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentAfterAll().
	 *
	 * @see FPF2SpecBase::LatentAfterAll(const EAsyncExecution, const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after the last It() block within the enclosing scope.
	 */
	void xLatentAfterAll(const EAsyncExecution                        Execution,
	                     const FTimespan&                             Timeout,
	                     const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

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
	 *
	 * @return
	 *	The server-side handle of the granted ability.
	 */
	static FGameplayAbilitySpecHandle GrantCharacterFakeAbility(TScriptInterface<IPF2CharacterInterface> Character);

	/**
	 * Grants the specified character the specified Gameplay Ability and returns its handle.
	 *
	 * @param Character
	 *	The character to which the ability will be granted.
	 * @param AbilityClass
	 *	The type of ability to grant.
	 *
	 * @return
	 *	The server-side handle of the granted ability.
	 */
	static FGameplayAbilitySpecHandle GrantCharacterAbility(TScriptInterface<IPF2CharacterInterface> Character,
	                                                        TSubclassOf<UGameplayAbility>            AbilityClass);

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
	 * @param InvokingAbilityHandle
	 *	The handle of the ability to spoof as having initiated the effect (e.g., the attack ability).
	 * @param SetByCallerMagnitudesMap
	 *	An optional map of set-by-caller-magnitudes/parameters to parameters, to pass into the GE.
	 *
	 * @return
	 *	The server-side handle of the GE specification.
	 */
	static FGameplayEffectSpecHandle BuildEffectSpec(
		const TSubclassOf<UGameplayEffect>             EffectClass,
		const TScriptInterface<IPF2CharacterInterface> Instigator,
		const FGameplayAbilitySpecHandle               InvokingAbilityHandle,
		const TMap<FName, float>&                      SetByCallerMagnitudesMap = {});


	/**
	 * Builds a Gameplay Effect (GE) specification for the given GE type, instigated by the given ability and character.
	 *
	 * @param EffectClass
	 *	The type of GE for which a spec is desired.
	 * @param Instigator
	 *	The character to spoof as being responsible for the effect (e.g., the source of damage).
	 * @param InvokingAbilityHandle
	 *	The handle of the ability to spoof as having initiated the effect (e.g., the attack ability).
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
		const FGameplayAbilitySpecHandle               InvokingAbilityHandle,
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
				FString::Printf(TEXT("Expected '%s' to not equal '%s'."), *ActualString, *NotExpectedString),
				1
			);
		}
	}

	/**
	 * Ensures that all test definitions have been loaded and cached.
	 *
	 * This is a re-implementation of FAutomationSpecBase::EnsureDefinitions() to invoke PostDefineEx() for handling v2
	 * spec definitions that include BeforeAll() and AfterAll().
	 */
	void EnsureDefinitionsEx() const;

	/**
	 * Converts all test definitions into executable tests.
	 *
	 * This is a re-implementation of FAutomationSpecBase::PostDefine() to handle v2 spec definitions that include
	 * BeforeAll() and AfterAll().
	 */
	void PostDefineEx();

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
