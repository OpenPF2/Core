// Adapted from "Runtime/Core/Public/Misc/AutomationTest.h" (Unreal Engine 5.3), which is
// Copyright Epic Games, Inc. Licensed only for use with Unreal Engine.

#pragma once

#include <Misc/AutomationTest.h>

class FAutomationSpecBaseEx
	: public FAutomationTestBase
	, public TSharedFromThis<FAutomationSpecBaseEx>
{
	// --- Start OpenPF2 Difference from <Misc/AutomationTest.h>
	friend class FPF2SpecBase;
	// --- End OpenPF2 Difference from <Misc/AutomationTest.h>

private:

	class FSingleExecuteLatentCommand : public IAutomationLatentCommand
	{
	public:
		FSingleExecuteLatentCommand(const FAutomationSpecBaseEx* const InSpec, TFunction<void()> InPredicate, bool bInSkipIfErrored = false)
			: Spec(InSpec)
			, Predicate(MoveTemp(InPredicate))
			, bSkipIfErrored(bInSkipIfErrored)
		{ }

		virtual ~FSingleExecuteLatentCommand()
		{ }

		virtual bool Update() override
		{
			if (bSkipIfErrored && Spec->HasAnyErrors())
			{
				return true;
			}

			Predicate();
			return true;
		}

	private:

		const FAutomationSpecBaseEx* const Spec;
		const TFunction<void()> Predicate;
		const bool bSkipIfErrored;
	};

	class FUntilDoneLatentCommand : public IAutomationLatentCommand
	{
	public:

		FUntilDoneLatentCommand(FAutomationSpecBaseEx* const InSpec, TFunction<void(const FDoneDelegate&)> InPredicate, const FTimespan& InTimeout, bool bInSkipIfErrored = false)
			: Spec(InSpec)
			, Predicate(MoveTemp(InPredicate))
			, Timeout(InTimeout)
			, bSkipIfErrored(bInSkipIfErrored)
			, bIsRunning(false)
			, bDone(false)
		{ }

		virtual ~FUntilDoneLatentCommand()
		{ }

		virtual bool Update() override
		{
			if (!bIsRunning)
			{
				if (bSkipIfErrored && Spec->HasAnyErrors())
				{
					return true;
				}

				bDone = false;
				Predicate(FDoneDelegate::CreateSP(this, &FUntilDoneLatentCommand::Done));
				bIsRunning = true;
				StartedRunning = FDateTime::UtcNow();
			}

			if (bDone)
			{
				Reset();
				return true;
			}
			else if (FDateTime::UtcNow() >= StartedRunning + Timeout)
			{
				Reset();
				Spec->AddError(TEXT("Latent command timed out."), 0);
				return true;
			}

			return false;
		}

	private:

		void Done()
		{
			if (bIsRunning)
			{
				bDone = true;
			}
		}

		void Reset()
		{
			// Reset the done for the next potential run of this command
			bDone = false;
			bIsRunning = false;
		}

	private:

		FAutomationSpecBaseEx* const Spec;
		const TFunction<void(const FDoneDelegate&)> Predicate;
		const FTimespan Timeout;
		const bool bSkipIfErrored;

		bool bIsRunning;
		FDateTime StartedRunning;
		FThreadSafeBool bDone;
	};

	class FAsyncUntilDoneLatentCommand : public IAutomationLatentCommand
	{
	public:

		FAsyncUntilDoneLatentCommand(FAutomationSpecBaseEx* const InSpec, EAsyncExecution InExecution, TFunction<void(const FDoneDelegate&)> InPredicate, const FTimespan& InTimeout, bool bInSkipIfErrored = false)
			: Spec(InSpec)
			, Execution(InExecution)
			, Predicate(MoveTemp(InPredicate))
			, Timeout(InTimeout)
			, bSkipIfErrored(bInSkipIfErrored)
			, bDone(false)
		{ }

		virtual ~FAsyncUntilDoneLatentCommand()
		{ }

		virtual bool Update() override
		{
			if (!Future.IsValid())
			{
				if (bSkipIfErrored && Spec->HasAnyErrors())
				{
					return true;
				}

				bDone = false;
				Future = Async(Execution, [this]() {
					Predicate(FDoneDelegate::CreateRaw(this, &FAsyncUntilDoneLatentCommand::Done));
				});

				StartedRunning = FDateTime::UtcNow();
			}

			if (bDone)
			{
				Reset();
				return true;
			}
			else if (FDateTime::UtcNow() >= StartedRunning + Timeout)
			{
				Reset();
				Spec->AddError(TEXT("Latent command timed out."), 0);
				return true;
			}

			return false;
		}

	private:

		void Done()
		{
			if (Future.IsValid())
			{
				bDone = true;
			}
		}

		void Reset()
		{
			// Reset the done for the next potential run of this command
			bDone = false;
			Future.Reset();
		}

	private:

		FAutomationSpecBaseEx* const Spec;
		const EAsyncExecution Execution;
		const TFunction<void(const FDoneDelegate&)> Predicate;
		const FTimespan Timeout;
		const bool bSkipIfErrored;

		FThreadSafeBool bDone;
		FDateTime StartedRunning;
		TFuture<void> Future;
	};

	class FAsyncLatentCommand : public IAutomationLatentCommand
	{
	public:

		FAsyncLatentCommand(FAutomationSpecBaseEx* const InSpec, EAsyncExecution InExecution, TFunction<void()> InPredicate, const FTimespan& InTimeout, bool bInSkipIfErrored = false)
			: Spec(InSpec)
			, Execution(InExecution)
			, Predicate(MoveTemp(InPredicate))
			, Timeout(InTimeout)
			, bSkipIfErrored(bInSkipIfErrored)
			, bDone(false)
		{ }

		virtual ~FAsyncLatentCommand()
		{ }

		virtual bool Update() override
		{
			if (!Future.IsValid())
			{
				if (bSkipIfErrored && Spec->HasAnyErrors())
				{
					return true;
				}

				bDone = false;
				Future = Async(Execution, [this]() {
					Predicate();
					Done();
				});

				StartedRunning = FDateTime::UtcNow();
			}

			if (bDone)
			{
				Reset();
				return true;
			}
			else if (FDateTime::UtcNow() >= StartedRunning + Timeout)
			{
				Reset();
				Spec->AddError(TEXT("Latent command timed out."), 0);
				return true;
			}

			return false;
		}

	private:

		void Done()
		{
			if (Future.IsValid())
			{
				bDone = true;
			}
		}

		void Reset()
		{
			// Reset the done for the next potential run of this command
			bDone = false;
			Future.Reset();
		}

	private:

		FAutomationSpecBaseEx* const Spec;
		const EAsyncExecution Execution;
		const TFunction<void()> Predicate;
		const FTimespan Timeout;
		const bool bSkipIfErrored;

		FThreadSafeBool bDone;
		FDateTime StartedRunning;
		TFuture<void> Future;
	};

	struct FSpecIt
	{
	public:

		FString Description;
		FString Id;
		FString Filename;
		int32 LineNumber;
		TSharedRef<IAutomationLatentCommand> Command;

		FSpecIt(FString InDescription, FString InId, FString InFilename, int32 InLineNumber, TSharedRef<IAutomationLatentCommand> InCommand)
			: Description(MoveTemp(InDescription))
			, Id(MoveTemp(InId))
			, Filename(InFilename)
			, LineNumber(MoveTemp(InLineNumber))
			, Command(MoveTemp(InCommand))
		{ }
	};

	struct FSpecDefinitionScopeEx
	{
	public:

		FString Description;

		TArray<TSharedRef<IAutomationLatentCommand>> BeforeEach;
		TArray<TSharedRef<FSpecIt>> It;
		TArray<TSharedRef<IAutomationLatentCommand>> AfterEach;

		TArray<TSharedRef<FSpecDefinitionScopeEx>> Children;

		// --- Start OpenPF2 Differences from <Misc/AutomationTest.h>
		TArray<TSharedRef<IAutomationLatentCommand>> BeforeAll;
		// --- End OpenPF2 Differences from <Misc/AutomationTest.h>
	};

	struct FSpec
	{
	public:

		FString Id;
		FString Description;
		FString Filename;
		int32 LineNumber;
		TArray<TSharedRef<IAutomationLatentCommand>> Commands;
	};

public:

	FAutomationSpecBaseEx(const FString& InName, const bool bInComplexTask)
		: FAutomationTestBase(InName, bInComplexTask)
		, DefaultTimeout(FTimespan::FromSeconds(30))
		, bEnableSkipIfError(true)
		, RootDefinitionScope(MakeShareable(new FSpecDefinitionScopeEx()))
	{
		DefinitionScopeStack.Push(RootDefinitionScope.ToSharedRef());
	}

	virtual bool RunTest(const FString& InParameters) override
	{
		EnsureDefinitions();

		if (!InParameters.IsEmpty())
		{
			const TSharedRef<FSpec>* SpecToRun = IdToSpecMap.Find(InParameters);
			if (SpecToRun != nullptr)
			{
				for (int32 Index = 0; Index < (*SpecToRun)->Commands.Num(); ++Index)
				{
					FAutomationTestFramework::GetInstance().EnqueueLatentCommand((*SpecToRun)->Commands[Index]);
				}
			}
		}
		else
		{
			TArray<TSharedRef<FSpec>> Specs;
			IdToSpecMap.GenerateValueArray(Specs);

			for (int32 SpecIndex = 0; SpecIndex < Specs.Num(); SpecIndex++)
			{
				for (int32 CommandIndex = 0; CommandIndex < Specs[SpecIndex]->Commands.Num(); ++CommandIndex)
				{
					FAutomationTestFramework::GetInstance().EnqueueLatentCommand(Specs[SpecIndex]->Commands[CommandIndex]);
				}
			}
		}

		return true;
	}

	virtual bool IsStressTest() const
	{
		return false;
	}

	virtual uint32 GetRequiredDeviceNum() const override
	{
		return 1;
	}

	virtual FString GetTestSourceFileName(const FString& InTestName) const override
	{
		FString TestId = InTestName;
		if (TestId.StartsWith(TestName + TEXT(" ")))
		{
			TestId = InTestName.RightChop(TestName.Len() + 1);
		}

		const TSharedRef<FSpec>* Spec = IdToSpecMap.Find(TestId);
		if (Spec != nullptr)
		{
			return (*Spec)->Filename;
		}

		return FAutomationTestBase::GetTestSourceFileName();
	}

	virtual int32 GetTestSourceFileLine(const FString& InTestName) const override
	{
		FString TestId = InTestName;
		if (TestId.StartsWith(TestName + TEXT(" ")))
		{
			TestId = InTestName.RightChop(TestName.Len() + 1);
		}

		const TSharedRef<FSpec>* Spec = IdToSpecMap.Find(TestId);
		if (Spec != nullptr)
		{
			return (*Spec)->LineNumber;
		}

		return FAutomationTestBase::GetTestSourceFileLine();
	}

	virtual void GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const override
	{
		EnsureDefinitions();

		TArray<TSharedRef<FSpec>> Specs;
		IdToSpecMap.GenerateValueArray(Specs);

		for (int32 Index = 0; Index < Specs.Num(); Index++)
		{
			OutTestCommands.Push(Specs[Index]->Id);
			OutBeautifiedNames.Push(Specs[Index]->Description);
		}
	}

	void xDescribe(const FString& InDescription, TFunction<void()> DoWork)
	{
		//disabled
	}

	void Describe(const FString& InDescription, TFunction<void()> DoWork)
	{
		LLM_SCOPE_BYNAME(TEXT("AutomationTest/Framework"));
		const TSharedRef<FSpecDefinitionScopeEx> ParentScope = DefinitionScopeStack.Last();
		const TSharedRef<FSpecDefinitionScopeEx> NewScope = MakeShareable(new FSpecDefinitionScopeEx());
		NewScope->Description = InDescription;
		ParentScope->Children.Push(NewScope);

		DefinitionScopeStack.Push(NewScope);
		PushDescription(InDescription);
		DoWork();
		PopDescription(InDescription);
		DefinitionScopeStack.Pop();

		if (NewScope->It.Num() == 0 && NewScope->Children.Num() == 0)
		{
			ParentScope->Children.Remove(NewScope);
		}
	}

	void xIt(const FString& InDescription, TFunction<void()> DoWork)
	{
		//disabled
	}

	void xIt(const FString& InDescription, EAsyncExecution Execution, TFunction<void()> DoWork)
	{
		//disabled
	}

	void xIt(const FString& InDescription, EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void()> DoWork)
	{
		//disabled
	}

	void xLatentIt(const FString& InDescription, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		//disabled
	}

	void xLatentIt(const FString& InDescription, const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		//disabled
	}

	void xLatentIt(const FString& InDescription, EAsyncExecution Execution, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		//disabled
	}

	void xLatentIt(const FString& InDescription, EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		//disabled
	}

	void It(const FString& InDescription, TFunction<void()> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		const auto Stack = GetStack();

		PushDescription(InDescription);
		CurrentScope->It.Push(MakeShareable(new FSpecIt(GetDescription(), GetId(), Stack.Get()[0].Filename, Stack.Get()[0].LineNumber, MakeShareable(new FSingleExecuteLatentCommand(this, DoWork, bEnableSkipIfError)))));
		PopDescription(InDescription);
	}

	void It(const FString& InDescription, EAsyncExecution Execution, TFunction<void()> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		const auto Stack = GetStack();

		PushDescription(InDescription);
		CurrentScope->It.Push(MakeShareable(new FSpecIt(GetDescription(), GetId(), Stack.Get()[0].Filename, Stack.Get()[0].LineNumber, MakeShareable(new FAsyncLatentCommand(this, Execution, DoWork, DefaultTimeout, bEnableSkipIfError)))));
		PopDescription(InDescription);
	}

	void It(const FString& InDescription, EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void()> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		const auto Stack = GetStack();

		PushDescription(InDescription);
		CurrentScope->It.Push(MakeShareable(new FSpecIt(GetDescription(), GetId(), Stack.Get()[0].Filename, Stack.Get()[0].LineNumber, MakeShareable(new FAsyncLatentCommand(this, Execution, DoWork, Timeout, bEnableSkipIfError)))));
		PopDescription(InDescription);
	}

	void LatentIt(const FString& InDescription, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		const auto Stack = GetStack();

		PushDescription(InDescription);
		CurrentScope->It.Push(MakeShareable(new FSpecIt(GetDescription(), GetId(), Stack.Get()[0].Filename, Stack.Get()[0].LineNumber, MakeShareable(new FUntilDoneLatentCommand(this, DoWork, DefaultTimeout, bEnableSkipIfError)))));
		PopDescription(InDescription);
	}

	void LatentIt(const FString& InDescription, const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		const auto Stack = GetStack();

		PushDescription(InDescription);
		CurrentScope->It.Push(MakeShareable(new FSpecIt(GetDescription(), GetId(), Stack.Get()[0].Filename, Stack.Get()[0].LineNumber, MakeShareable(new FUntilDoneLatentCommand(this, DoWork, Timeout, bEnableSkipIfError)))));
		PopDescription(InDescription);
	}

	void LatentIt(const FString& InDescription, EAsyncExecution Execution, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		const auto Stack = GetStack();

		PushDescription(InDescription);
		CurrentScope->It.Push(MakeShareable(new FSpecIt(GetDescription(), GetId(), Stack.Get()[0].Filename, Stack.Get()[0].LineNumber, MakeShareable(new FAsyncUntilDoneLatentCommand(this, Execution, DoWork, DefaultTimeout, bEnableSkipIfError)))));
		PopDescription(InDescription);
	}

	void LatentIt(const FString& InDescription, EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		const auto Stack = GetStack();

		PushDescription(InDescription);
		CurrentScope->It.Push(MakeShareable(new FSpecIt(GetDescription(), GetId(), Stack.Get()[0].Filename, Stack.Get()[0].LineNumber, MakeShareable(new FAsyncUntilDoneLatentCommand(this, Execution, DoWork, Timeout, bEnableSkipIfError)))));
		PopDescription(InDescription);
	}

	void xBeforeEach(TFunction<void()> DoWork)
	{
		// disabled
	}

	void xBeforeEach(EAsyncExecution Execution, TFunction<void()> DoWork)
	{
		// disabled
	}

	void xBeforeEach(EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void()> DoWork)
	{
		// disabled
	}

	void xLatentBeforeEach(TFunction<void(const FDoneDelegate&)> DoWork)
	{
		// disabled
	}

	void xLatentBeforeEach(const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		// disabled
	}

	void xLatentBeforeEach(EAsyncExecution Execution, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		// disabled
	}

	void xLatentBeforeEach(EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		// disabled
	}

	void BeforeEach(TFunction<void()> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->BeforeEach.Push(MakeShareable(new FSingleExecuteLatentCommand(this, DoWork, bEnableSkipIfError)));
	}

	void BeforeEach(EAsyncExecution Execution, TFunction<void()> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->BeforeEach.Push(MakeShareable(new FAsyncLatentCommand(this, Execution, DoWork, DefaultTimeout, bEnableSkipIfError)));
	}

	void BeforeEach(EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void()> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->BeforeEach.Push(MakeShareable(new FAsyncLatentCommand(this, Execution, DoWork, Timeout, bEnableSkipIfError)));
	}

	void LatentBeforeEach(TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->BeforeEach.Push(MakeShareable(new FUntilDoneLatentCommand(this, DoWork, DefaultTimeout, bEnableSkipIfError)));
	}

	void LatentBeforeEach(const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->BeforeEach.Push(MakeShareable(new FUntilDoneLatentCommand(this, DoWork, Timeout, bEnableSkipIfError)));
	}

	void LatentBeforeEach(EAsyncExecution Execution, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->BeforeEach.Push(MakeShareable(new FAsyncUntilDoneLatentCommand(this, Execution, DoWork, DefaultTimeout, bEnableSkipIfError)));
	}

	void LatentBeforeEach(EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->BeforeEach.Push(MakeShareable(new FAsyncUntilDoneLatentCommand(this, Execution, DoWork, Timeout, bEnableSkipIfError)));
	}

	void xAfterEach(TFunction<void()> DoWork)
	{
		// disabled
	}

	void xAfterEach(EAsyncExecution Execution, TFunction<void()> DoWork)
	{
		// disabled
	}

	void xAfterEach(EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void()> DoWork)
	{
		// disabled
	}

	void xLatentAfterEach(TFunction<void(const FDoneDelegate&)> DoWork)
	{
		// disabled
	}

	void xLatentAfterEach(const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		// disabled
	}

	void xLatentAfterEach(EAsyncExecution Execution, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		// disabled
	}

	void xLatentAfterEach(EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		// disabled
	}

	void AfterEach(TFunction<void()> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->AfterEach.Push(MakeShareable(new FSingleExecuteLatentCommand(this, DoWork)));
	}

	void AfterEach(EAsyncExecution Execution, TFunction<void()> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->AfterEach.Push(MakeShareable(new FAsyncLatentCommand(this, Execution, DoWork, DefaultTimeout)));
	}

	void AfterEach(EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void()> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->AfterEach.Push(MakeShareable(new FAsyncLatentCommand(this, Execution, DoWork, Timeout)));
	}

	void LatentAfterEach(TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->AfterEach.Push(MakeShareable(new FUntilDoneLatentCommand(this, DoWork, DefaultTimeout)));
	}

	void LatentAfterEach(const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->AfterEach.Push(MakeShareable(new FUntilDoneLatentCommand(this, DoWork, Timeout)));
	}

	void LatentAfterEach(EAsyncExecution Execution, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->AfterEach.Push(MakeShareable(new FAsyncUntilDoneLatentCommand(this, Execution, DoWork, DefaultTimeout)));
	}

	void LatentAfterEach(EAsyncExecution Execution, const FTimespan& Timeout, TFunction<void(const FDoneDelegate&)> DoWork)
	{
		const TSharedRef<FSpecDefinitionScopeEx> CurrentScope = DefinitionScopeStack.Last();
		CurrentScope->AfterEach.Push(MakeShareable(new FAsyncUntilDoneLatentCommand(this, Execution, DoWork, Timeout)));
	}

protected:

	/* The timespan for how long a block should be allowed to execute before giving up and failing the test */
	FTimespan DefaultTimeout;

	/* Whether or not BeforeEach and It blocks should skip execution if the test has already failed */
	bool bEnableSkipIfError;

	void EnsureDefinitions() const
	{
		if (!bHasBeenDefined)
		{
			const_cast<FAutomationSpecBaseEx*>(this)->Define();
			const_cast<FAutomationSpecBaseEx*>(this)->PostDefine();
		}
	}

	virtual void Define() = 0;

	void PostDefine()
	{
		TArray<TSharedRef<FSpecDefinitionScopeEx>> Stack;
		Stack.Push(RootDefinitionScope.ToSharedRef());

		TArray<TSharedRef<IAutomationLatentCommand>> BeforeEach;
		TArray<TSharedRef<IAutomationLatentCommand>> AfterEach;

		while (Stack.Num() > 0)
		{
			const TSharedRef<FSpecDefinitionScopeEx> Scope = Stack.Last();

			BeforeEach.Append(Scope->BeforeEach);
			AfterEach.Append(Scope->AfterEach); // iterate in reverse

			for (int32 ItIndex = 0; ItIndex < Scope->It.Num(); ItIndex++)
			{
				TSharedRef<FSpecIt> It = Scope->It[ItIndex];

				TSharedRef<FSpec> Spec = MakeShareable(new FSpec());
				Spec->Id = It->Id;
				Spec->Description = It->Description;
				Spec->Filename = It->Filename;
				Spec->LineNumber = It->LineNumber;
				Spec->Commands.Append(BeforeEach);
				Spec->Commands.Add(It->Command);

				for (int32 AfterEachIndex = AfterEach.Num() - 1; AfterEachIndex >= 0; AfterEachIndex--)
				{
					Spec->Commands.Add(AfterEach[AfterEachIndex]);
				}

				check(!IdToSpecMap.Contains(Spec->Id));
				IdToSpecMap.Add(Spec->Id, Spec);
			}
			Scope->It.Empty();

			if (Scope->Children.Num() > 0)
			{
				Stack.Append(Scope->Children);
				Scope->Children.Empty();
			}
			else
			{
				while (Stack.Num() > 0 && Stack.Last()->Children.Num() == 0 && Stack.Last()->It.Num() == 0)
				{
					const TSharedRef<FSpecDefinitionScopeEx> PoppedScope = Stack.Pop();

					if (PoppedScope->BeforeEach.Num() > 0)
					{
						BeforeEach.RemoveAt(BeforeEach.Num() - PoppedScope->BeforeEach.Num(), PoppedScope->BeforeEach.Num());
					}

					if (PoppedScope->AfterEach.Num() > 0)
					{
						AfterEach.RemoveAt(AfterEach.Num() - PoppedScope->AfterEach.Num(), PoppedScope->AfterEach.Num());
					}
				}
			}
		}

		RootDefinitionScope.Reset();
		DefinitionScopeStack.Reset();
		bHasBeenDefined = true;
	}

	void Redefine()
	{
		Description.Empty();
		IdToSpecMap.Empty();
		RootDefinitionScope.Reset();
		DefinitionScopeStack.Empty();
		bHasBeenDefined = false;
	}

private:

	void PushDescription(const FString& InDescription)
	{
		LLM_SCOPE_BYNAME(TEXT("AutomationTest/Framework"));
		Description.Add(InDescription);
	}

	void PopDescription(const FString& InDescription)
	{
		Description.RemoveAt(Description.Num() - 1);
	}

	FString GetDescription() const
	{
		FString CompleteDescription;
		for (int32 Index = 0; Index < Description.Num(); ++Index)
		{
			if (Description[Index].IsEmpty())
			{
				continue;
			}

			if (CompleteDescription.IsEmpty())
			{
				CompleteDescription = Description[Index];
			}
			else if (FChar::IsWhitespace(CompleteDescription[CompleteDescription.Len() - 1]) || FChar::IsWhitespace(Description[Index][0]))
			{
				CompleteDescription = CompleteDescription + TEXT(".") + Description[Index];
			}
			else
			{
				CompleteDescription = FString::Printf(TEXT("%s.%s"), *CompleteDescription, *Description[Index]);
			}
		}

		return CompleteDescription;
	}

	FString GetId() const
	{
		if (Description.Last().EndsWith(TEXT("]")))
		{
			FString ItDescription = Description.Last();
			ItDescription.RemoveAt(ItDescription.Len() - 1);

			int32 StartingBraceIndex = INDEX_NONE;
			if (ItDescription.FindLastChar(TEXT('['), StartingBraceIndex) && StartingBraceIndex != ItDescription.Len() - 1)
			{
				FString CommandId = ItDescription.RightChop(StartingBraceIndex + 1);
				return CommandId;
			}
		}

		FString CompleteId;
		for (int32 Index = 0; Index < Description.Num(); ++Index)
		{
			if (Description[Index].IsEmpty())
			{
				continue;
			}

			if (CompleteId.IsEmpty())
			{
				CompleteId = Description[Index];
			}
			else if (FChar::IsWhitespace(CompleteId[CompleteId.Len() - 1]) || FChar::IsWhitespace(Description[Index][0]))
			{
				CompleteId = CompleteId + Description[Index];
			}
			else
			{
				CompleteId = FString::Printf(TEXT("%s %s"), *CompleteId, *Description[Index]);
			}
		}

		return CompleteId;
	}

	static TArray<FProgramCounterSymbolInfo> StackWalk()
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_FAutomationSpecBaseEx_StackWalk);

		LLM_SCOPE_BYNAME(TEXT("AutomationTest/Framework"));
		SAFE_GETSTACK(Stack, 1, 1);
		return Stack;
	}

	static TArray<FProgramCounterSymbolInfo> SkipStackWalk()
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_FAutomationSpecBaseEx_SkipStackWalk);

		LLM_SCOPE_BYNAME(TEXT("AutomationTest/Framework"));
		TArray<FProgramCounterSymbolInfo> Stack;
		FProgramCounterSymbolInfo First;
		TCString<ANSICHAR>::Strcpy(First.Filename, FProgramCounterSymbolInfo::MAX_NAME_LENGTH, "Unknown");
		First.LineNumber = 0;
		Stack.Add(First);

		return Stack;
	}

	static TSharedRef<TArray<FProgramCounterSymbolInfo>> GetStack()
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_FAutomationSpecBaseEx_GetStack);

		static bool CurrentNeedSkipStackWalk(FAutomationTestFramework::NeedSkipStackWalk());
		static TSharedRef<TArray<FProgramCounterSymbolInfo>> Stack = MakeShared<TArray<FProgramCounterSymbolInfo>>(
			CurrentNeedSkipStackWalk ? SkipStackWalk() : StackWalk());

		if (CurrentNeedSkipStackWalk != FAutomationTestFramework::NeedSkipStackWalk())
		{
			// This block is to react on changes of corresponding configuration variable */
			CurrentNeedSkipStackWalk = !CurrentNeedSkipStackWalk;
			Stack = MakeShared<TArray<FProgramCounterSymbolInfo>>(
				CurrentNeedSkipStackWalk ? SkipStackWalk() : StackWalk());
		}

		return Stack;
	}

private:

	TArray<FString> Description;
	TMap<FString, TSharedRef<FSpec>> IdToSpecMap;
	TSharedPtr<FSpecDefinitionScopeEx> RootDefinitionScope;
	TArray<TSharedRef<FSpecDefinitionScopeEx>> DefinitionScopeStack;
	bool bHasBeenDefined;
};
