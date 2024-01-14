// Adapted from "Runtime/Core/Public/Misc/AutomationTest.h" (Unreal Engine 5.3), which is
// Copyright Epic Games, Inc. Licensed only for use with Unreal Engine.

#pragma once

#include <IAutomationControllerManager.h>

#include <Misc/AutomationTest.h>

// =====================================================================================================================
// Macro Declarations
// =====================================================================================================================
/**
 * Declares a new variable for use in the current test scope and nested scopes.
 *
 * This is an alias for Let() that shortens variable and lambda type definitions.
 *
 * @param Name
 *	The name of the variable to declare.
 * @param Type
 *	The type of the variable to declare.
 * @param Captures
 *	A comma-separated list of zero or more captures, optionally beginning with a capture-default. This must be enclosed
 *	in square brackets, even if there are no captures.
 * @param Generator
 *	The block/body of the generator function, enclosed in curly braces. This must return a value of the same type as the
 *	variable.
 */
#define LET(Name, Type, Captures, Generator) \
	const TSpecVariable<Type> Name = Let(TGeneratorFunc<Type>(Captures() Generator))

/**
 * Redefines the value of an existing variable from the current test scope or an outer scope.
 *
 * The new value only has effect within the current scope and nested scopes.
 *
 * This is an alias for RedefineLet() that shortens variable and lambda type definitions.
 *
 * @param Name
 *	The name of the variable to redefine.
 * @param Type
 *	The type of the variable to redefine.
 * @param Captures
 *	A comma-separated list of zero or more captures, optionally beginning with a capture-default. This must be enclosed
 *	in square brackets, even if there are no captures.
 * @param Generator
 *	The block/body of the re-generator function, enclosed in curly braces. This lambda receives a pointer variable
 *	called "Previous" that can be used to obtain the value from the last definition or redefinition of the variable.
 */
#define REDEFINE_LET(Name, Type, Captures, RegeneratorBody) \
	RedefineLet(Name, TGeneratorRedefineFunc<Type>(Captures(const TSpecVariablePtr<Type>& Previous) RegeneratorBody))

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Automation spec base class used by OpenPF2 automation tests.
 *
 * This was adapted and modified from FAutomationSpecBase in the Engine in order to add BeforeAll() capabilities to
 * automation specs. Beyond those enhancements, this class does not contain any OpenPF2-specific test functionality;
 * that functionality exists in the FPF2SpecBase sub-class to ensure clean separation of concerns.
 */
class FPF2AutomationSpecBase : public FAutomationTestBase, public TSharedFromThis<FPF2AutomationSpecBase>
{
public:
	// =================================================================================================================
	// Forward Declarations
	// =================================================================================================================
	struct FSpecBlockHandle;

	template <typename VariableType>
	class TSpecLet;

private:
	class FSpecLetWildcard;

public:
	// =================================================================================================================
	// Public Type Aliases
	// =================================================================================================================
	/**
	 * Alias for a shared pointer to a variable declared with Let().
	 */
	template <typename VariableType>
	using TSpecVariablePtr = TSharedPtr<TSpecLet<VariableType>>;

	/**
	 * Alias for a shared pointer to the un-templated base variable type for Let().
	 */
	using FSpecVariablePtrWildcard = TSharedPtr<FSpecLetWildcard>;

	/**
	 * Alias for a function invoked to generate the value of a Let() variable.
	 */
	template <typename VariableType>
	using TGeneratorFunc = TFunction<VariableType()>;

	/**
	 * Alias for a function invoked to regenerate the value of a Let() variable.
	 */
	template <typename VariableType>
	using TGeneratorRedefineFunc = TFunction<VariableType(const TSpecVariablePtr<VariableType>)>;

	/**
	 * Alias for a map of variables in a spec scope.
	 */
	using FSpecVariableScope = TMap<FSpecBlockHandle, FSpecVariablePtrWildcard>;

	// =================================================================================================================
	// Public Type Definitions
	// =================================================================================================================
	/**
	 * Represents a unique handle/ID for a specific block within an automation test specification.
	 */
	struct FSpecBlockHandle
	{
		// =============================================================================================================
		// Private Fields
		// =============================================================================================================
	private:
		/**
		 * The unique identifier for the handle.
		 */
		int32 Handle;

	public:
		// =============================================================================================================
		// Public Static Methods
		// =============================================================================================================
		/**
		 * Gets the unique hash for this handle.
		 *
		 * @param SpecHandle - The handle to hash.
		 * @return The hash value.
		 */
		friend static uint32 GetTypeHash(const FSpecBlockHandle& SpecHandle)
		{
			return ::GetTypeHash(SpecHandle.Handle);
		}

		// =============================================================================================================
		// Public Constructor
		// =============================================================================================================
		/**
		 * Constructs a new instance and assigns it a unique identifier.
		 */
		explicit FSpecBlockHandle()
		{
			this->Assign();
		}

		// =============================================================================================================
		// Public Instance Methods
		// =============================================================================================================
		/**
         * Equality operator for comparing this handle to another handle.
         *
         * @param Other
         *	The handle to compare against.
         * @return
         *	true if the handles are equal; or, false otherwise.
         */
		bool operator==(const FSpecBlockHandle& Other) const
		{
			return (this->Handle == Other.Handle);
		}

		/**
		 * Inequality operator for comparing this handle to another handle.
		 *
		 * @param Other
		 *	The handle to compare against.
		 * @return
		 *	true if the handles are not equal; or, false otherwise.
		 */
		bool operator!=(const FSpecBlockHandle& Other) const
		{
			return (this->Handle != Other.Handle);
		}

		/**
		 * Converts this handle to a string representation.
		 *
		 * @return
		 *	The string representation of the unique identifier of this handle.
		 */
		FString ToString() const
		{
			return FString::FromInt(this->Handle);
		}

	private:
		/**
		 * Assigns this handle its unique identifier.
		 */
		void Assign();
	};

	/**
	 * A reference to a variable in a test context.
	 *
	 * The real value of the variable is not stored in this object; rather, the value is retrieved from the variable map
	 * of the current scope (the active spec). This allows the value of a variable to be redefined in nested scopes in a
	 * way that impacts outer scopes (e.g., BeforeEach()).
	 *
	 * @tparam VariableType
	 *	The type of the variable.
	 */
	template <typename VariableType>
	class TSpecVariable
	{
		// =============================================================================================================
		// Private Fields
		// =============================================================================================================
		/**
		 * The unique handle of this variable.
		 *
		 * This handle uniquely defines this variable and is retained even if the value of the variable is redefined.
		 */
		FSpecBlockHandle Handle;

		/**
		 * The outer test.
		 */
		const FPF2AutomationSpecBase* Spec;

	public:
		// =============================================================================================================
		// Public Constructors
		// =============================================================================================================
		/**
		 * Constructs a new instance.
		 *
		 * @param Spec
		 *	The outer test.
		 */
		explicit TSpecVariable(const FPF2AutomationSpecBase* const Spec) : Spec(Spec)
		{
		}

		// =============================================================================================================
		// Public Methods
		// =============================================================================================================
		/**
		 * Gets the unique handle for this variable.
		 *
		 * @return
		 *	The block handle for this variable.
		 */
		FORCEINLINE UE_NODISCARD FSpecBlockHandle GetHandle() const
		{
			return this->Handle;
		}

		/**
		 * Retrieves the value of this variable from the current scope.
		 *
		 * Variables are lazily evaluated but only evaluated once per test, so subsequent fetches of the same variable
		 * during the same test return the same value.
		 *
		 * @return
		 *	The current value of this variable.
		 */
		FORCEINLINE UE_NODISCARD VariableType& Get() const
		{
			TSpecVariablePtr<VariableType> VariablePtr = this->Spec->GetVariable<VariableType>(this->Handle);

			return VariablePtr->Get();
		}

		/**
		 * Defines a dereference operator for retrieving the value of this variable from the current scope.
		 *
		 * Variables are lazily evaluated but only evaluated once per test, so subsequent fetches of the same variable
		 * during the same test return the same value.
		 *
		 * @return
		 *	The current value of this variable.
		 */
		FORCEINLINE UE_NODISCARD VariableType& operator *() const
		{
			return this->Get();
		}

		/**
		 * Defines an arrow operator for interacting with the value of this variable from the current scope.
		 *
		 * Variables are lazily evaluated but only evaluated once per test, so subsequent fetches of the same variable
		 * during the same test return the same value.
		 *
		 * @return
		 *	The current value of this variable.
		 */
		FORCEINLINE UE_NODISCARD VariableType& operator ->() const
		{
			return this->Get();
		}
	};

private:
	// =================================================================================================================
	// Private Type Definitions
	// =================================================================================================================
	/**
	 * Un-templated base class for variables declared with Let() in spec contexts.
	 *
	 * This class mainly exists to give us a concrete type to store in the map of variables in tests, since maps require
	 * a concrete value type. At execution time, this type is downcast to the specific variable type upon retrieval.
	 */
	class FSpecLetWildcard
	{
	protected:
		// =============================================================================================================
		// Protected Fields
		// =============================================================================================================
		/**
		 * Whether the value of the variable has been generated and memoized during the current test.
		 */
		bool bWasGenerated;

		// =============================================================================================================
		// Protected Constructors
		// =============================================================================================================
		/**
		 * Constructs a new instance.
		 */
		explicit FSpecLetWildcard() : bWasGenerated(false)
		{
		}

		/**
		 * Destructs this instance.
		 */
		virtual ~FSpecLetWildcard() = default;

	public:
		// =============================================================================================================
		// Public Methods
		// =============================================================================================================
		/**
		 * Resets the value of this variable so that will regenerate its value on the next request.
		 */
		virtual void Reset()
		{
			this->bWasGenerated = false;
		}

		// =============================================================================================================
		// Protected Methods
		// =============================================================================================================
		FORCEINLINE UE_NODISCARD bool WasGenerated() const
		{
			return this->bWasGenerated;
		}
	};

	/**
	 * A placeholder for a Let() block that performs no operation and returns the default value for a type.
	 *
	 * This is instantiated as the "prior definition" for the first definition of a variable within a scope.
	 *
	 * @tparam VariableType
	 *	The type of variable that this is standing in for.
	 */
	template <typename VariableType>
	class TNoOpSpecLet final : public TSpecLet<VariableType>
	{
	public:
		// =============================================================================================================
		// Public Constructors and Destructors
		// =============================================================================================================
		/**
		 * Constructs a new instance.
		 */
		explicit TNoOpSpecLet() :
			TSpecLet<VariableType>(
				TGeneratorRedefineFunc<VariableType>([](const TSpecVariablePtr<VariableType>&)
					{
						return VariableType();
					}
				),
				TSpecVariablePtr<VariableType>()
			)
		{
		}

		/**
		 * Destructs this instance.
		 */
		virtual ~TNoOpSpecLet() override = default;
	};

	/**
	 * A lazy-loaded, memoized variable that can be defined in a spec. scope and overridden in nested scopes.
	 *
	 * This is the actual object stored in the variables map of a test. Tests generate and retrieve the value from this
	 * object via a TSpecVariable object, which acts like a handle.
	 *
	 * @tparam VariableType
	 *	The type of the variable.
	 */
	template <typename VariableType>
	class TSpecLet : public FSpecLetWildcard
	{
		// =============================================================================================================
		// Private Fields
		// =============================================================================================================
		/**
		 * The original definition of this variable within the same scope.
		 *
		 * This enables a re-definition of an existing variable to be based on the prior value of the same variable.
		 */
		TSharedPtr<TSpecLet> PriorDefinition;

		/**
		 * The lambda that will be invoked to generate the value of the variable.
		 *
		 * This is invoked the first time the variable is retrieved during the test, and the value is memoized so the
		 * same value will be returned if the value is retrieved again during the same test. The lambda receives a const
		 * pointer to the previous definition of variable, to enable the variable to base its value on its prior
		 * definition. If the variable has been redefined multiple times (e.g., through multiple nested scopes), the
		 * variables are chained, so only the prior definition is accessible within each lambda.
		 */
		TGeneratorRedefineFunc<VariableType> GeneratorFunc;

		/**
		 * The value of this variable (if any) that was memoized during the current test.
		 */
		VariableType MemoizedValue;

		// =============================================================================================================
		// Public Constructors and Destructors
		// =============================================================================================================
	public:
		/**
		 * Constructs a new instance for a Let() block that has no prior definition in outer scopes.
		 *
		 * @param GeneratorFunc
		 *	The lambda to invoke to generate the value of the variable. The lambda receives a const pointer to the
		 *	previous definition of variable, to enable the variable to base its value on its prior definition. If the
		 *	variable has been redefined multiple times (e.g., through multiple nested scopes), the variables are
		 *	chained, so only the prior definition is accessible within each lambda.
		 */
		explicit TSpecLet(TGeneratorRedefineFunc<VariableType> GeneratorFunc) :
			PriorDefinition(TSharedPtr<TSpecLet>(new TNoOpSpecLet<VariableType>())),
			GeneratorFunc(MoveTemp(GeneratorFunc))
		{
		}

		/**
		 * Constructs a new instance for a Let() block that is redefining a definition from outer scopes.
		 *
		 * @param PriorDefinition
		 *	The original definition of this variable from an outer scope or earlier within the same scope.
		 * @param GeneratorFunc
		 *	The lambda to invoke to generate the value of the variable. The lambda receives a const pointer to the
		 *	previous definition of variable, to enable the variable to base its value on its prior definition. If the
		 *	variable has been redefined multiple times (e.g., through multiple nested scopes), the variables are
		 *	chained, so only the prior definition is accessible within each lambda.
		 */
		explicit TSpecLet(TGeneratorRedefineFunc<VariableType> GeneratorFunc, TSharedPtr<TSpecLet> PriorDefinition) :
			PriorDefinition(MoveTemp(PriorDefinition)),
			GeneratorFunc(MoveTemp(GeneratorFunc))
		{
		}

		/**
		 * Destructs this instance.
		 */
		virtual ~TSpecLet() override
		{
		};

		// =============================================================================================================
		// Public Methods
		// =============================================================================================================
		/**
		 * Gets the value of this variable.
		 *
		 * If the variable has not been generated during this test, it is generated and then memoized.
		 *
		 * @return
		 *	The value of this variable.
		 */
		UE_NODISCARD VariableType& Get()
		{
			if (!this->WasGenerated())
			{
				this->MemoizedValue = this->GeneratorFunc(this->PriorDefinition);
				this->bWasGenerated = true;
			}

			return this->MemoizedValue;
		}

		/**
		 * Defines a dereference operator for retrieving the value of this variable.
		 *
		 * Variables are lazily evaluated but only evaluated once per test, so subsequent fetches of the same variable
		 * during the same test return the same value.
		 *
		 * @return
		 *	The current value of this variable.
		 */
		FORCEINLINE UE_NODISCARD VariableType& operator *()
		{
			return this->Get();
		}

		/**
		 * Defines an arrow operator for interacting with the value of this variable.
		 *
		 * Variables are lazily evaluated but only evaluated once per test, so subsequent fetches of the same variable
		 * during the same test return the same value.
		 *
		 * @return
		 *	The current value of this variable.
		 */
		FORCEINLINE UE_NODISCARD VariableType& operator ->()
		{
			return this->Get();
		}

		// =============================================================================================================
		// Public Methods - FSpecLetWildcard Overrides
		// =============================================================================================================
		virtual void Reset() override
		{
			FSpecLetWildcard::Reset();

			this->MemoizedValue = VariableType();
		}
	};

	/**
	 * An automation test command that just runs code and waits until it returns.
	 *
	 * The code executes within a single engine frame.
	 */
	class FSimpleBlockingCommand final : public IAutomationLatentCommand
	{
		// =============================================================================================================
		// Private Fields
		// =============================================================================================================
		/**
		 * The automation test specification that supplied the code for this command.
		 */
		const FPF2AutomationSpecBase* const Spec;

		/**
		 * The code to execute for this command.
		 */
		const TFunction<void()> Work;

		/**
		 * Whether this command should skip execution if the parent specification has reported a failure in prior tests.
		 */
		const bool bSkipIfErrored;

	public:
		// =============================================================================================================
		// Public Constructor / Destructor
		// =============================================================================================================
		/**
		 * Constructs a new instance.
		 *
		 * @param Spec
		 *	The automation test specification that is instantiating this command.
		 * @param Work
		 *	The code the command should execute.
		 * @param bInSkipIfErrored
		 *	Whether the command should skip execution if the parent specification reports failures in prior tests.
		 */
		FSimpleBlockingCommand(const FPF2AutomationSpecBase* const Spec,
		                       TFunction<void()>                   Work,
		                       const bool                          bInSkipIfErrored = false) :
			Spec(Spec),
			Work(MoveTemp(Work)),
			bSkipIfErrored(bInSkipIfErrored)
		{
		}

		/**
		 * Destructor.
		 */
		virtual ~FSimpleBlockingCommand() override
		{
		}

		// =============================================================================================================
		// Public Methods - IAutomationLatentCommand Overrides
		// =============================================================================================================
		virtual bool Update() override;
	};

	/**
	 * An automation test command for performing actions asynchronously with custom execution.
	 */
	class FAsyncCommand final : public IAutomationLatentCommand
	{
		// =============================================================================================================
		// Private Fields
		// =============================================================================================================
		/**
		 * The automation test specification that supplied the code for this command.
		 */
		FPF2AutomationSpecBase* const Spec;

		/**
		 * How the code in this command should be executed (task graph, thread pool, dedicated thread, etc.).
		 */
		const EAsyncExecution Execution;

		/**
		 * The code to execute for this command.
		 */
		const TFunction<void()> Work;

		/**
		 * The maximum amount of time to wait for the code in this command to execute before failing the test.
		 */
		const FTimespan Timeout;

		/**
		 * Whether this command should skip execution if the parent specification has reported a failure in prior tests.
		 */
		const bool bSkipIfErrored;

		/**
		 * Indicates whether this command has finished running.
		 */
		FThreadSafeBool bDone;

		/**
		 * The date and time that the command started running (for timing).
		 */
		FDateTime StartTime;

		/**
		 * The eventual result of executing the work.
		 */
		TFuture<void> Future;

	public:
		// =============================================================================================================
		// Public Constructor / Destructor
		// =============================================================================================================
		/**
		 * Constructs a new instance.
		 *
		 * @param Spec
		 *	The automation test specification that is instantiating this command.
		 * @param Execution
		 *	How the code in this command should be executed (task graph, thread pool, dedicated thread, etc.).
		 * @param Work
		 *	The code the command should execute.
		 * @param Timeout
		 *	The maximum amount of time to wait for the code in the command to execute before failing the test.
		 * @param bSkipIfErrored
		 *	Whether the command should skip execution if the parent specification reports failures in prior tests.
		 */
		FAsyncCommand(FPF2AutomationSpecBase* const Spec,
		              const EAsyncExecution         Execution,
		              TFunction<void()>             Work,
		              const FTimespan&              Timeout,
		              const bool                    bSkipIfErrored = false) :
			Spec(Spec),
			Execution(Execution),
			Work(MoveTemp(Work)),
			Timeout(Timeout),
			bSkipIfErrored(bSkipIfErrored),
			bDone(false)
		{
		}

		/**
		 * Destructor.
		 */
		virtual ~FAsyncCommand() override
		{
		}

		// =============================================================================================================
		// Public Methods - IAutomationLatentCommand Overrides
		// =============================================================================================================
		virtual bool Update() override;

	private:
		/**
		 * Callback invoked when the work of the command has completed running.
		 */
		void Done();

		/**
		 * Clears and resets the state of this command so that it can be run again.
		 */
		void Reset();
	};

	/**
	 * An automation test command for performing actions that take multiple frames.
	 *
	 * When using this variation, the test will not continue execution to the next code block in the test sequence until
	 * the actively running latent code block invokes the Done() delegate.
	 */
	class FMultiFrameLatentCommand final : public IAutomationLatentCommand
	{
		// =============================================================================================================
		// Private Fields
		// =============================================================================================================
		/**
		 * The automation test specification that supplied the code for this command.
		 */
		FPF2AutomationSpecBase* const Spec;

		/**
		 * The code to execute for this command.
		 */
		const TFunction<void(const FDoneDelegate&)> Work;

		/**
		 * The maximum amount of time to wait for the code in this command to execute before failing the test.
		 */
		const FTimespan Timeout;

		/**
		 * Whether this command should skip execution if the parent specification has reported a failure in prior tests.
		 */
		const bool bSkipIfErrored;

		/**
		 * Whether the command has started running.
		 */
		bool bHasStartedRunning;

		/**
		 * The date and time that the command started running (for timing).
		 */
		FDateTime StartTime;

		/**
		 * Indicates whether this command has finished running.
		 */
		FThreadSafeBool bDone;

	public:
		// =============================================================================================================
		// Public Constructor / Destructor
		// =============================================================================================================
		/**
		 * Constructs a new instance.
		 *
		 * @param Spec
		 *	The automation test specification that is instantiating this command.
		 * @param Work
		 *	The code the command should execute.
		 * @param Timeout
		 *	The maximum amount of time to wait for the code in the command to execute before failing the test.
		 * @param bSkipIfErrored
		 *	Whether the command should skip execution if the parent specification reports failures in prior tests.
		 */
		FMultiFrameLatentCommand(FPF2AutomationSpecBase* const         Spec,
		                         TFunction<void(const FDoneDelegate&)> Work,
		                         const FTimespan&                      Timeout,
		                         const bool                            bSkipIfErrored = false) :
			Spec(Spec),
			Work(MoveTemp(Work)),
			Timeout(Timeout),
			bSkipIfErrored(bSkipIfErrored),
			bHasStartedRunning(false),
			bDone(false)
		{
		}

		/**
		 * Destructor.
		 */
		virtual ~FMultiFrameLatentCommand() override
		{
		}

		// =============================================================================================================
		// Public Methods - IAutomationLatentCommand Overrides
		// =============================================================================================================
		virtual bool Update() override;

	private:
		/**
		 * Callback invoked when the work of the command has completed running.
		 */
		void Done();

		/**
		 * Clears and resets the state of this command so that it can be run again.
		 */
		void Reset();
	};

	/**
	 * An automation test command for performing actions asynchronously with custom execution over multiple frames.
	 *
	 * When using this variation, the test will not continue execution to the next code block in the test sequence until
	 * the actively running latent code block invokes the Done() delegate.
	 */
	class FAsyncMultiFrameLatentCommand final : public IAutomationLatentCommand
	{
		// =============================================================================================================
		// Private Fields
		// =============================================================================================================
		/**
		 * The automation test specification that supplied the code for this command.
		 */
		FPF2AutomationSpecBase* const Spec;

		/**
		 * How the code in this command should be executed (task graph, thread pool, dedicated thread, etc.).
		 */
		const EAsyncExecution Execution;

		/**
		 * The code to execute for this command.
		 */
		const TFunction<void(const FDoneDelegate&)> Work;

		/**
		 * The maximum amount of time to wait for the code in this command to execute before failing the test.
		 */
		const FTimespan Timeout;

		/**
		 * Whether this command should skip execution if the parent specification has reported a failure in prior tests.
		 */
		const bool bSkipIfErrored;

		/**
		 * Indicates whether this command has finished running.
		 */
		FThreadSafeBool bDone;

		/**
		 * The date and time that the command started running (for timing).
		 */
		FDateTime StartTime;

		/**
		 * The eventual result of executing the work.
		 */
		TFuture<void> Future;

	public:
		// =============================================================================================================
		// Public Constructor / Destructor
		// =============================================================================================================
		/**
		 * Constructs a new instance.
		 *
		 * @param Spec
		 *	The automation test specification that is instantiating this command.
		 * @param Execution
		 *	How the code in this command should be executed (task graph, thread pool, dedicated thread, etc.).
		 * @param Work
		 *	The code the command should execute.
		 * @param Timeout
		 *	The maximum amount of time to wait for the code in the command to execute before failing the test.
		 * @param bSkipIfErrored
		 *	Whether the command should skip execution if the parent specification reports failures in prior tests.
		 */
		FAsyncMultiFrameLatentCommand(FPF2AutomationSpecBase* const         Spec,
		                              const EAsyncExecution                 Execution,
		                              TFunction<void(const FDoneDelegate&)> Work,
		                              const FTimespan&                      Timeout,
		                              const bool                            bSkipIfErrored = false) :
			Spec(Spec),
			Execution(Execution),
			Work(MoveTemp(Work)),
			Timeout(Timeout),
			bSkipIfErrored(bSkipIfErrored),
			bDone(false)
		{
		}

		/**
		 * Destructor.
		 */
		virtual ~FAsyncMultiFrameLatentCommand() override
		{
		}

		// =============================================================================================================
		// Public Methods - IAutomationLatentCommand Overrides
		// =============================================================================================================
		virtual bool Update() override;

	private:
		/**
		 * Callback invoked when the work of the command has completed running.
		 */
		void Done();

		/**
		 * Clears and resets the state of this command so that it can be run again.
		 */
		void Reset();
	};

	/**
	 * Represents an It() block within a specification.
	 *
	 * Each It() block defines a specific test case, including a human-readable description and automation command to
	 * invoke when that test has been executed.
	 *
	 * The data in this struct is only maintained while tests are being defined, but is then converted into an FSpec for
	 * execution during PostDefine().
	 */
	struct FSpecItDefinition final
	{
		// =============================================================================================================
		// Public Fields
		// =============================================================================================================
		/**
		 * The unique identifier of this block.
		 */
		FString Id;

		/**
	     * The human-readable description of the test case this block performs.
	     */
		FString Description;

		/**
		 * The filename of the specification in which this block was defined.
		 */
		FString Filename;

		/**
		 * The line number where this block was defined.
		 */
		int32 LineNumber;

		/**
		 * The automation command to invoke to execute the test case of this block.
		 */
		TSharedRef<IAutomationLatentCommand> Command;

		// =============================================================================================================
		// Public Constructor
		// =============================================================================================================
		/**
		 * Constructs a new instance.
		 *
		 * @param Id
		 *	The unique identifier of the block.
		 * @param Description
		 *	The human-readable description of the test case the block performs.
		 * @param Filename
		 *	The filename of the specification defining the block.
		 * @param LineNumber
		 *	The line number where the block is defined.
		 * @param Command
		 *	The automation command to invoke to execute the test case of the block.
		 */
		FSpecItDefinition(FString                              Id,
		                  FString                              Description,
		                  FString                              Filename,
		                  int32                                LineNumber,
		                  TSharedRef<IAutomationLatentCommand> Command) :
			Id(MoveTemp(Id)),
			Description(MoveTemp(Description)),
			Filename(MoveTemp(Filename)),
			LineNumber(MoveTemp(LineNumber)),
			Command(MoveTemp(Command))
		{
		}
	};

	/**
	 * Represents the scope of execution for one or more test cases being defined.
	 *
	 * This is the container for the setup (defined by BeforeAll() and BeforeEach()), execution (defined by It()), and
	 * teardown (defined by AfterEach()) commands associated with test cases. The scope can also have zero or more child
	 * scopes (defined by Describe()).
	 *
	 * The data in this struct is only maintained while tests are being defined, but is then converted into an FSpec for
	 * execution during PostDefine().
	 */
	struct FSpecDefinitionScope final
	{
		// =============================================================================================================
		// Public Fields
		// =============================================================================================================
		/**
		 * The human-readable description for this scope, as provided by Describe().
		 */
		FString Description;

		/**
		 * The variables defined in this scope.
		 */
		FSpecVariableScope Variables;

		/**
		 * Latent commands to execute once before all It() blocks within the specification (including nested scopes).
		 */
		TArray<TSharedRef<IAutomationLatentCommand>> BeforeAll;

		/**
		 * Latent commands to execute before each It() block within the specification (including nested scopes).
		 */
		TArray<TSharedRef<IAutomationLatentCommand>> BeforeEach;

		/**
		 * Collection of It() blocks defining the individual expectations of this immediate scope.
		 */
		TArray<TSharedRef<FSpecItDefinition>> It;

		/**
		 * Latent commands to execute after each It() block within the specification (including nested scopes).
		 */
		TArray<TSharedRef<IAutomationLatentCommand>> AfterEach;

		/**
		 * Child definitions representing nested scopes within this one.
		 */
		TArray<TSharedRef<FSpecDefinitionScope>> Children;
	};

	/**
	 * A flattened, ready-to-execute test case.
	 *
	 * Each instance of this struct is populated with all the commands of the information needed to setup (BeforeAll(),
	 * BeforeEach()), execute (It()), and tear down a test case (AfterEach()).
	 */
	struct FSpec final
	{
		// =============================================================================================================
		// Public Fields
		// =============================================================================================================
		/**
		 * The unique identifier for this test case.
		 */
		FString Id;

		/**
		 * The human-readable description for this scope, as provided by Describe().
		 */
		FString Description;

		/**
		 * The filename of the specification in which this test was defined.
		 */
		FString Filename;

		/**
		 * The line number where this test was defined.
		 */
		int32 LineNumber;

		/**
		 * The variables defined in this scope.
		 */
		FSpecVariableScope Variables;

		/**
		 * The automation commands to execute to perform this test.
		 */
		TArray<TSharedRef<IAutomationLatentCommand>> Commands;
	};

	/**
	 * A simple object for tracking the state of a test session.
	 *
	 * Data within this object is automatically reset between test suite runs.
	 */
	class FPF2TestSessionState final
	{
		// =============================================================================================================
		// Private Fields
		// =============================================================================================================
		/**
		 * The handle of the delegate that has been registered to be notified about test completion.
		 */
		FDelegateHandle TestDelegateHandle;

		/**
		 * The handles of the blocks for which execution is being tracked (e.g., "BeforeAll()" blocks).
		 */
		TSet<FSpecBlockHandle> BlocksRun;

	public:
		// =============================================================================================================
		// Public Constructor / Destructor
		// =============================================================================================================
		/**
		 * Constructs a new instance.
		 */
		explicit FPF2TestSessionState();

		/**
		 * Destructor.
		 */
		~FPF2TestSessionState();

		// =============================================================================================================
		// Public Instance Methods
		// =============================================================================================================
		/**
		 * Checks if a specific test block has been executed.
		 *
		 * @param BlockHandle
		 *	The handle of the block to check.
		 *
		 * @return
		 *	true if the block has been executed; or, false otherwise.
		 */
		bool HasBlockRun(const FSpecBlockHandle& BlockHandle) const;

		/**
		 * Marks a test block as having executed.
		 *
		 * @param BlockHandle
		 *	The handle of the block to mark.
		 */
		void MarkBlockAsRun(const FSpecBlockHandle& BlockHandle);

	private:
		// =============================================================================================================
		// Private Methods
		// =============================================================================================================
		/**
		 * Tries to get the automation controller manager.
		 *
		 * @return
		 *	Either the automation controller manager; or a null pointer if the manager is not available (e.g., during
		 *	shutdown of the engine).
		 */
		static IAutomationControllerManagerPtr GetAutomationController();

		/**
		 * Clears all test state.
		 *
		 * This is invoked automatically whenever tests in the current session finish running.
		 */
		void ClearState();
	};

	// =================================================================================================================
	// Private Static Methods
	// =================================================================================================================
	/**
	 * Gets the current value of a variable from a scope.
	 *
	 * @tparam VariableType
	 *	The type of the variable.
	 *
	 * @param Variables
	 *	The scope of variables from which to obtain the variable.
	 * @param Handle
	 *	The unique handle of the desired variable.
	 *
	 * @return
	 *	The value of the variable.
	 */
	template <typename VariableType>
	FORCEINLINE UE_NODISCARD static TSpecVariablePtr<VariableType> GetVariableFromScope(
		FSpecVariableScope      Variables,
		const FSpecBlockHandle& Handle)
	{
		check(Variables.Contains(Handle));
		return StaticCastSharedPtr<TSpecLet<VariableType>>(Variables[Handle]);
	}

	// =================================================================================================================
	// Private Fields
	// =================================================================================================================
	/**
	 * The stack (hierarchy) of descriptions for the test currently being defined.
	 */
	TArray<FString> DescriptionStack;

	/**
	 * A map that associates the ID of each specification to the ready-to-execute test case for that spec.
	 *
	 * This is populated from the hierarchy of test definitions by PostDefine().
	 *
	 * @see PostDefine()
	 */
	TMap<FString, TSharedRef<FSpec>> IdToSpecMap;

	/**
	 * The top-most, root scope of the test hierarchy/tree structure.
	 */
	TSharedPtr<FSpecDefinitionScope> RootDefinitionScope;

	/**
	 * The stack (hierarchy) of test scopes defined so far for the current test scenario/spec.
	 */
	TArray<TSharedRef<FSpecDefinitionScope>> DefinitionScopeStack;

	/**
	 * A flag to indicate whether this test has finished defining and caching its expectations.
	 */
	bool bHasBeenDefined;

	/**
	 * The default timespan for how long a block should be allowed to execute before giving up and failing the test.
	 */
	FTimespan DefaultTimeout;

	/**
	 * Whether test blocks should skip executing if any of the tests of this specification fail.
	 *
	 * This affects code in BeforeAll(), BeforeEach(), and It() blocks.
	 */
	bool bEnableSkipIfError;

	/**
	 * The state of the current test suite session during execution.
	 *
	 * This is used to keep track of state that has to be reset between test suite runs.
	 */
	TSharedPtr<FPF2TestSessionState> SuiteSessionState;

	/**
	 * The variables defined for the current test.
	 *
	 * This is cleared and repopulated at the start of each test case.
	 */
	FSpecVariableScope VariablesInScope;

public:
	// =================================================================================================================
	// Public Constructor
	// =================================================================================================================
	/**
	 * Constructs a new instance.
	 *
	 * @param Name
	 *	The name of the automation test.
	 * @param bComplexTask
	 *	Whether this test is complex and should be included in a stress test.
	 */
	FPF2AutomationSpecBase(const FString& Name, const bool bComplexTask) :
		FAutomationTestBase(Name, bComplexTask),
		RootDefinitionScope(MakeShareable(new FSpecDefinitionScope())),
		bHasBeenDefined(false),
		DefaultTimeout(FTimespan::FromSeconds(30)),
		bEnableSkipIfError(true)
	{
		this->DefinitionScopeStack.Push(this->RootDefinitionScope.ToSharedRef());
	}

	// =================================================================================================================
	// Public Methods - FAutomationTestBase Overrides
	// =================================================================================================================
	virtual uint32 GetRequiredDeviceNum() const override
	{
		return 1;
	}

	virtual FString GetTestSourceFileName(const FString& InTestName) const override;
	virtual int32 GetTestSourceFileLine(const FString& InTestName) const override;
	virtual void GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const override;
	virtual bool RunTest(const FString& InParameters) override;

protected:
	// =================================================================================================================
	// Protected Methods - Spec Test DSL
	// =================================================================================================================
	/**
	 * Defines a new scope for expectations, such that they are more readable and DRYer.
	 *
	 * Describe() is used as a way to scope complicated expectations. Using Describe() makes your code DRYer based on
	 * the interaction it has with other supporting functions such as BeforeEach(), BeforeAll() and AfterEach().
	 *
	 * You can cascade Describe() by putting a Describe() in another Describe().
	 *
	 * Keep in mind that Describe() is not a test and is not executed during an actual test run. They are only executed
	 * once when first defining the expectations (or tests) within the Spec.
	 *
	 * @param InDescription
	 *	A string that describes the scope of the expectations within it.
	 * @param DoWork
	 *	A lambda that defines the expectations within this scope.
	 */
	void Describe(const FString& InDescription, const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of Describe().
	 *
	 * @see Describe(const FString&, const TFunction<void()>&)
	 *
	 * @param InDescription
	 *	A string that describes the scope of the expectations within it.
	 * @param DoWork
	 *	A lambda that defines the expectations within this scope.
	 */
	FORCEINLINE void xDescribe(const FString& InDescription, const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Declares a variable for use in a test.
	 *
	 * @tparam VariableType
	 *	The type of the variable.
	 *
	 * @param GeneratorFunc
	 *	The lambda to invoke to generate the value of the variable.
	 *
	 * @return
	 *	A reference/handle to the variable, for access to the variable during the test.
	 */
	template <typename VariableType>
	TSpecVariable<VariableType> Let(const TGeneratorFunc<VariableType>& GeneratorFunc)
	{
		const TSharedRef<FSpecDefinitionScope> CurrentScope = this->GetCurrentScope();
		TSpecVariable<VariableType>            Variable     = TSpecVariable<VariableType>(this);

		// Adapt the signature so that we only have one type of generator function we have to call.
		FSpecVariablePtrWildcard Definition = FSpecVariablePtrWildcard(
			new TSpecLet<VariableType>([GeneratorFunc](const TSpecVariablePtr<VariableType>)
			{
				return GeneratorFunc();
			})
		);

		CurrentScope->Variables.Add(Variable.GetHandle(), Definition);

		return Variable;
	}

	/**
	 * Redefines the value of a previously defined variable in a test.
	 *
	 * @tparam VariableType
	 *	The type of the variable.
	 *
	 * @param Variable
	 *	The variable to redefine.
	 * @param GeneratorFunc
	 *	The lambda to invoke to generate the value of the variable. The lambda receives a const pointer to the previous
	 *	definition of variable, to enable the variable to base its value on its prior definition. If the variable has
	 *	been redefined multiple times (e.g., through multiple nested scopes), the variables are chained, so only the
	 *	prior definition is accessible within each lambda.
	 */
	template <typename VariableType>
	void RedefineLet(TSpecVariable<VariableType> Variable, const TGeneratorRedefineFunc<VariableType>& GeneratorFunc)
	{
		const FSpecBlockHandle&                VariableHandle  = Variable.GetHandle();
		const TSharedRef<FSpecDefinitionScope> CurrentScope    = this->GetCurrentScope();
		TSpecVariablePtr<VariableType>         PriorDefinition = TSpecVariablePtr<VariableType>();
		FSpecVariablePtrWildcard               NewDefinition;

		// Iterate in reverse to evaluate variables from the inner-most scope outwards, looking for an existing
		// definition of this variable.
		for (int32 ScopeIndex = this->DefinitionScopeStack.Num() - 1; ScopeIndex >= 0; --ScopeIndex)
		{
			const TSharedRef<FSpecDefinitionScope> Scope = this->DefinitionScopeStack[ScopeIndex];

			if (Scope->Variables.Contains(VariableHandle))
			{
				PriorDefinition = GetVariableFromScope<VariableType>(Scope->Variables, VariableHandle);
				break;
			}
		}

		check(PriorDefinition.IsValid());

		NewDefinition = FSpecVariablePtrWildcard(new TSpecLet<VariableType>(GeneratorFunc, PriorDefinition));

		CurrentScope->Variables.Add(VariableHandle, NewDefinition);
	}

	/**
	 * Defines an expectation for a specific scenario within the current test scope.
	 *
	 * You can call It() from the root Define() method or within any Describe() lambda. It() should be used to
	 * assert the expectation, but it can also be utilized for the final bits of setup for the scenario being tested.
	 *
	 * Generally, it is a best practice to start an It() call description string with a third person singular present
	 * tense verb (e.g., "makes", "creates", "sets", etc.) or start with the word "should", implying "it should".
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	void It(const FString& InDescription, const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of It().
	 *
	 * @see It(const FString&, const TFunction<void()>&)
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	FORCEINLINE void xIt(const FString& InDescription, const TFunction<void()>& DoWork)
	{
	    // Disabled.
	}

	/**
	 * Defines an asynchronous expectation for a specific scenario within the current test scope.
	 *
	 * You can call It() from the root Define() method or within any Describe() lambda. It() should be used to
	 * assert the expectation, but it can also be utilized for the final bits of setup for the scenario being tested.
	 *
	 * Generally, it is a best practice to start an It() call description string with a third person singular present
	 * tense verb (e.g., "makes", "creates", "sets", etc.) or start with the word "should", implying "it should".
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	void It(const FString& InDescription, const EAsyncExecution Execution, const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of It().
	 *
	 * @see It(const FString&, const EAsyncExecution, const TFunction<void()>&)
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	FORCEINLINE void xIt(const FString& InDescription, const EAsyncExecution Execution, const TFunction<void()>& DoWork)
	{
	    // Disabled.
	}

	/**
	 * Defines an asynchronous expectation with a timeout for a specific scenario within the current test scope.
	 *
	 * You can call It() from the root Define() method or within any Describe() lambda. It() should be used to
	 * assert the expectation, but it can also be utilized for the final bits of setup for the scenario being tested.
	 *
	 * Generally, it is a best practice to start an It() call description string with a third person singular present
	 * tense verb (e.g., "makes", "creates", "sets", etc.) or start with the word "should", implying "it should".
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	void It(const FString&           InDescription,
	        const EAsyncExecution    Execution,
	        const FTimespan&         Timeout,
	        const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of It().
	 *
	 * @see It(const FString&, const EAsyncExecution, const FTimespan&, const TFunction<void()>&)
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	FORCEINLINE void xIt(const FString&           InDescription,
	                     const EAsyncExecution    Execution,
	                     const FTimespan&         Timeout,
	                     const TFunction<void()>& DoWork)
	{
	    // Disabled.
	}

	/**
	 * Defines an async. expectation that runs over multiple frames for a specific scenario within the current test
	 * scope.
	 *
	 * You can call It() from the root Define() method or within any Describe() lambda. It() should be used to
	 * assert the expectation, but it can also be utilized for the final bits of setup for the scenario being tested.
	 *
	 * Generally, it is a best practice to start an It() call description string with a third person singular present
	 * tense verb (e.g., "makes", "creates", "sets", etc.) or start with the word "should", implying "it should".
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	void LatentIt(const FString& InDescription, const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentIt().
	 *
	 * @see LatentIt(const FString&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	FORCEINLINE void xLatentIt(const FString& InDescription, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
	    // Disabled.
	}

	/**
	 * Defines an async. expectation that runs over multiple frames with a timeout for a specific scenario within the
	 * current test scope.
	 *
	 * You can call It() from the root Define() method or within any Describe() lambda. It() should be used to
	 * assert the expectation, but it can also be utilized for the final bits of setup for the scenario being tested.
	 *
	 * Generally, it is a best practice to start an It() call description string with a third person singular present
	 * tense verb (e.g., "makes", "creates", "sets", etc.) or start with the word "should", implying "it should".
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	void LatentIt(const FString&                               InDescription,
	              const FTimespan&                             Timeout,
	              const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentIt().
	 *
	 * @see LatentIt(const FString&, const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	FORCEINLINE void xLatentIt(const FString&                               InDescription,
	                           const FTimespan&                             Timeout,
	                           const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
	    // Disabled.
	}

	/**
	 * Defines asynchronously expected behavior that runs over multiple frames for a specific scenario within the
	 * current test scope.
	 *
	 * You can call It() from the root Define() method or within any Describe() lambda. It() should be used to
	 * assert the expectation, but it can also be utilized for the final bits of setup for the scenario being tested.
	 *
	 * Generally, it is a best practice to start an It() call description string with a third person singular present
	 * tense verb (e.g., "makes", "creates", "sets", etc.) or start with the word "should", implying "it should".
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	void LatentIt(const FString&                               InDescription,
	              const EAsyncExecution                        Execution,
	              const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentIt().
	 *
	 * @see LatentIt(const FString&, const EAsyncExecution, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	FORCEINLINE void xLatentIt(const FString&                               InDescription,
	                           const EAsyncExecution                        Execution,
	                           const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
	    // Disabled.
	}

	/**
	 * Defines an async. expectation that runs over multiple frames with a timeout for a specific scenario within the
	 * current test scope.
	 *
	 * You can call It() from the root Define() method or within any Describe() lambda. It() should be used to
	 * assert the expectation, but it can also be utilized for the final bits of setup for the scenario being tested.
	 *
	 * Generally, it is a best practice to start an It() call description string with a third person singular present
	 * tense verb (e.g., "makes", "creates", "sets", etc.) or start with the word "should", implying "it should".
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	void LatentIt(const FString&                               InDescription,
	              const EAsyncExecution                        Execution,
	              const FTimespan&                             Timeout,
	              const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentIt().
	 *
	 * @see LatentIt(const FString&, const EAsyncExecution, const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param InDescription
	 *	A descriptive string specifying the expectation or behavior being tested.
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that contains the code to execute for the specified scenario.
	 */
	FORCEINLINE void xLatentIt(const FString&                               InDescription,
	                           const EAsyncExecution                        Execution,
	                           const FTimespan&                             Timeout,
	                           const TFunction<void(const FDoneDelegate&)>& DoWork)
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
	 * BeforeAll() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before the first It() block within its scope. If there are BeforeAll() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void BeforeAll(const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of BeforeAll().
	 *
	 * @see BeforeAll(const TFunction<void()>&)
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	FORCEINLINE void xBeforeAll(const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run asynchronously before the first It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before the first It() block within its scope. If there are BeforeAll() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void BeforeAll(const EAsyncExecution Execution, const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of BeforeAll().
	 *
	 * @see BeforeAll(const EAsyncExecution, const TFunction<void()>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	FORCEINLINE void xBeforeAll(const EAsyncExecution Execution, const TFunction<void()>& DoWork)
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
	 * BeforeAll() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before the first It() block within its scope. If there are BeforeAll() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void BeforeAll(const EAsyncExecution Execution, const FTimespan& Timeout, const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of BeforeAll().
	 *
	 * @see BeforeAll(const EAsyncExecution, const FTimespan&, const TFunction<void()>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	FORCEINLINE void xBeforeAll(const EAsyncExecution    Execution,
	                            const FTimespan&         Timeout,
	                            const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run over multiple frames before the first It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before the first It() block within its scope. If there are BeforeAll() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void LatentBeforeAll(const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeAll().
	 *
	 * @see LatentBeforeAll(const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentBeforeAll(const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run over multiple frames with a timeout before the first It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before the first It() block within its scope. If there are BeforeAll() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void LatentBeforeAll(const FTimespan& Timeout, const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeAll().
	 *
	 * @see LatentBeforeAll(const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentBeforeAll(const FTimespan& Timeout, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines async. code that must run over multiple frames before the first It() block of the current scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before the first It() block within its scope. If there are BeforeAll() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	void LatentBeforeAll(const EAsyncExecution Execution, const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeAll().
	 *
	 * @see LatentBeforeAll(const EAsyncExecution, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentBeforeAll(const EAsyncExecution                        Execution,
	                                  const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines async. code that must run over multiple frames with a timeout before the first It() block of the current
	 * scope.
	 *
	 * Each code block is executed from top to bottom. If a scope contains both BeforeAll() and BeforeEach() blocks, the
	 * BeforeAll() blocks will be evaluated before any BeforeEach() blocks are executed, including those inherited from
	 * outer scopes.
	 *
	 * BeforeAll() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before the first It() block within its scope. If there are BeforeAll() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
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
	                     const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeAll().
	 *
	 * @see LatentBeforeAll(const EAsyncExecution, const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before the first It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentBeforeAll(const EAsyncExecution                        Execution,
	                                  const FTimespan&                             Timeout,
	                                  const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run before each individual test case of the current scope.
	 *
	 * BeforeEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before each It() block within that scope. If there are BeforeEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	void BeforeEach(const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of BeforeEach().
	 *
	 * @see BeforeEach(const TFunction<void()>&)
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	FORCEINLINE void xBeforeEach(const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run asynchronously before each individual test case of the current scope.
	 *
	 * BeforeEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before each It() block within that scope. If there are BeforeEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	void BeforeEach(const EAsyncExecution Execution, const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of BeforeEach().
	 *
	 * @see BeforeEach(const EAsyncExecution, const TFunction<void()>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	FORCEINLINE void xBeforeEach(const EAsyncExecution Execution, const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run asynchronously with a timeout before each individual test case of the current scope.
	 *
	 * BeforeEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before each It() block within that scope. If there are BeforeEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	void BeforeEach(const EAsyncExecution Execution, const FTimespan& Timeout, const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of BeforeEach().
	 *
	 * @see BeforeEach(const EAsyncExecution, const FTimespan&, const TFunction<void()>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	FORCEINLINE void xBeforeEach(const EAsyncExecution    Execution,
	                             const FTimespan&         Timeout,
	                             const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines asynchronous code that must run over multiple frames before each test case of the current scope.
	 *
	 * BeforeEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before each It() block within that scope. If there are BeforeEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	void LatentBeforeEach(const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeEach().
	 *
	 * @see LatentBeforeEach(const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentBeforeEach(const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines asynchronous code that must run over multiple frames with a timeout before each test case of the current
	 * scope.
	 *
	 * BeforeEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before each It() block within that scope. If there are BeforeEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	void LatentBeforeEach(const FTimespan& Timeout, const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeEach().
	 *
	 * @see LatentBeforeEach(const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentBeforeEach(const FTimespan& Timeout, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines asynchronous code over multiple frames that must run before each individual test case within the current
	 * scope.
	 *
	 * BeforeEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before each It() block within that scope. If there are BeforeEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	void LatentBeforeEach(const EAsyncExecution Execution, const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeEach().
	 *
	 * @see LatentBeforeEach(const EAsyncExecution, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentBeforeEach(const EAsyncExecution                        Execution,
	                                   const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines asynchronous code that must run over multiple frames with a timeout before each test case of the current
	 * scope.
	 *
	 * BeforeEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom before each It() block within that scope. If there are BeforeEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	void LatentBeforeEach(const EAsyncExecution                        Execution,
	                      const FTimespan&                             Timeout,
	                      const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentBeforeEach().
	 *
	 * @see LatentBeforeEach(const EAsyncExecution, const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute before each It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentBeforeEach(const EAsyncExecution                        Execution,
	                                   const FTimespan&                             Timeout,
	                                   const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run after each individual test case of the current scope.
	 *
	 * AfterEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom after each It() block within that scope. If there are AfterEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	void AfterEach(const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of AfterEach().
	 *
	 * @see AfterEach(const TFunction<void()>&)
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	FORCEINLINE void xAfterEach(const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run asynchronously after each individual test case of the current scope.
	 *
	 * AfterEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom after each It() block within that scope. If there are AfterEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	void AfterEach(const EAsyncExecution Execution, const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of AfterEach().
	 *
	 * @see AfterEach(const EAsyncExecution, const TFunction<void()>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	FORCEINLINE void xAfterEach(const EAsyncExecution Execution, const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines code that must run asynchronously with a timeout after each individual test case of the current scope.
	 *
	 * AfterEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom after each It() block within that scope. If there are AfterEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	void AfterEach(const EAsyncExecution Execution, const FTimespan& Timeout, const TFunction<void()>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of AfterEach().
	 *
	 * @see AfterEach(const EAsyncExecution, const FTimespan&, const TFunction<void()>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	FORCEINLINE void xAfterEach(const EAsyncExecution    Execution,
	                            const FTimespan&         Timeout,
	                            const TFunction<void()>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines async. code that must run over multiple frames after each individual test case of the current scope.
	 *
	 * AfterEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom after each It() block within that scope. If there are AfterEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	void LatentAfterEach(const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentAfterEach().
	 *
	 * @see LatentAfterEach(const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentAfterEach(const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines async. code that must run over multiple frames with a timeout after each individual test case of the
	 * current scope.
	 *
	 * AfterEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom after each It() block within that scope. If there are AfterEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	void LatentAfterEach(const FTimespan& Timeout, const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentAfterEach().
	 *
	 * @see LatentAfterEach(const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentAfterEach(const FTimespan& Timeout, const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines async. code over multiple frames that must run after each individual test case within the current scope.
	 *
	 * AfterEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom after each It() block within that scope. If there are AfterEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	void LatentAfterEach(const EAsyncExecution Execution, const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentAfterEach().
	 *
	 * @see LatentAfterEach(const EAsyncExecution, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentAfterEach(const EAsyncExecution                        Execution,
	                                  const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Defines asynchronous code that must run over multiple frames with a timeout after each individual test case of
	 * the current scope.
	 *
	 * AfterEach() affects only the Describe() scope in which it is defined and its children. Each code block is
	 * executed from top to bottom after each It() block within that scope. If there are AfterEach() blocks in the
	 * enclosing scopes, they run from top to bottom from outermost scope to innermost scope.
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	void LatentAfterEach(const EAsyncExecution                        Execution,
	                     const FTimespan&                             Timeout,
	                     const TFunction<void(const FDoneDelegate&)>& DoWork);

	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	/**
	 * Disabled/skipped version of LatentAfterEach().
	 *
	 * @see LatentAfterEach(const EAsyncExecution, const FTimespan&, const TFunction<void(const FDoneDelegate&)>&)
	 *
	 * @param Execution
	 *	How the code in this block should be executed (task graph, thread pool, dedicated thread, etc.).
	 * @param Timeout
	 *	The maximum amount of time to wait for the code in this block to execute before failing the test.
	 * @param DoWork
	 *	A lambda that defines the code to execute after each It() block within the enclosing scope.
	 */
	FORCEINLINE void xLatentAfterEach(const EAsyncExecution                        Execution,
	                                  const FTimespan&                             Timeout,
	                                  const TFunction<void(const FDoneDelegate&)>& DoWork)
	{
		// Disabled.
	}

	/**
	 * Ensures that all test definitions have been loaded and cached.
	 */
	void EnsureDefinitions() const;

	/**
	 * Method that sub-classes must implement to define the structure and expectations of the test.
	 */
	virtual void Define() = 0;

	/**
	 * Converts all test definitions into executable tests.
	 */
	void PostDefine();

	/**
	 * Resets the cached definitions of this test, allowing the test structure to be redefined.
	 *
	 * This method is useful when working with parameterized tests that depend on some external input. For example, an
	 * external file that drives inputs. This method, when called, will re-invoke the Define() process. This results in
	 * all code blocks for this test to be re-gathered and collated.
	 */
	void Redefine();

private:
	// =============================================================================================================
	// Private Methods
	// =============================================================================================================
	/**
	 * Gets a unique identifier for the test scenario being defined.
	 *
	 * @return
	 *	The unique identifier.
	 */
	FString GetId() const;

	/**
	 * Gets the description for the test scenario being defined.
	 *
	 * This is assembled by concatenating the current description stack.
	 *
	 * @return
	 *	The description string.
	 */
	FString GetDescription() const;

	/**
	 * Gets the current spec definition scope.
	 *
	 * @return
	 *	The current spec definition scope.
	 */
	FORCEINLINE_DEBUGGABLE TSharedRef<FSpecDefinitionScope> GetCurrentScope() const
	{
		return this->DefinitionScopeStack.Last();
	}

	/**
	 * Gets the top-most stack frame, which represents the execution of the current test.
	 *
	 * @return
	 *	The top-most stack frame.
	 */
	FORCEINLINE static const FProgramCounterSymbolInfo& GetTopOfCallStack()
	{
		return GetCallStack().Get()[0];
	}

	/**
	 * Gets the current stack trace.
	 *
	 * @return
	 *	An array in which each element is a captured stack frame.
	 */
	static TSharedRef<TArray<FProgramCounterSymbolInfo>> GetCallStack();

	/**
	 * Walks and captures the current call stack of this test.
	 *
	 * @return
	 *	An array in which each element is a captured stack frame.
	 */
	static TArray<FProgramCounterSymbolInfo> CallStackWalk();

	/**
	 * Returns an empty call stack.
	 *
	 * @return
	 *	A fake, empty call stack.
	 */
	static TArray<FProgramCounterSymbolInfo> SkipCallStackWalk();

	/**
	 * Pushes a description onto the test hierarchy description stack.
	 *
	 * This description will be included in calls to GetDescription() until it is popped with PopDescription().
	 *
	 * @param InDescription
	 *	The description to push.
	 */
	void PushDescription(const FString& InDescription);

	/**
	 * Pops the previous description from the test hierarchy description stack.
	 */
	void PopDescription();

	/**
	 * Creates a wrapper function for ensuring a block runs only once during a test session.
	 *
	 * @param BlockHandle
	 *	Handle of the spec block.
	 * @param DoWork
	 *	The code to execute within the block.
	 *
	 * @return
	 *	The wrapper function.
	 */
	TFunction<void()> CreateRunWorkOnceWrapper(const FSpecBlockHandle&  BlockHandle,
	                                           const TFunction<void()>& DoWork) const;

	/**
	 * Creates a wrapper function for ensuring a block runs only once during a test session.
	 *
	 * @param BlockHandle
	 *	Handle of the spec block.
	 * @param DoWork
	 *	The code to execute within the block.
	 *
	 * @return
	 *	The wrapper function.
	 */
	TFunction<void(const FDoneDelegate&)> CreateRunWorkOnceWrapper(
		const FSpecBlockHandle&                      BlockHandle,
		const TFunction<void(const FDoneDelegate&)>& DoWork) const;

	/**
	 * Runs the specified spec.
	 *
	 * @param SpecToRun
	 *	The spec to run.
	 */
	void RunSpec(const TSharedRef<FSpec>& SpecToRun);

	/**
	 * Gets the value of the specified variable from the scope of the current test.
	 *
	 * This is used only while the test is running, not during test definition.
	 *
	 * @tparam VariableType
	 *	The type of the variable.
	 *
	 * @param Handle
	 *	The unique handle of the desired variable.
	 *
	 * @return
	 *	The value of the variable.
	 */
	template <typename VariableType>
	FORCEINLINE UE_NODISCARD TSpecVariablePtr<VariableType> GetVariable(const FSpecBlockHandle& Handle) const
	{
		return GetVariableFromScope<VariableType>(this->VariablesInScope, Handle);
	}
};
