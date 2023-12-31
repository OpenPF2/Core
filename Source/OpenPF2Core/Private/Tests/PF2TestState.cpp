// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "PF2TestState.h"

#include <IAutomationControllerModule.h>

void FPF2SpecBlockHandle::Assign()
{
	static int32 HandleCounter = 1;

	this->Handle = HandleCounter++;
}

FPF2TestState::FPF2TestState()
{
	const IAutomationControllerManagerPtr AutomationControllerManager = this->GetAutomationController();

	if (AutomationControllerManager.IsValid())
	{
		this->TestDelegateHandle =
			AutomationControllerManager->OnTestsComplete().AddRaw(this, &FPF2TestState::ClearState);
	}
}

FPF2TestState::~FPF2TestState()
{
	const IAutomationControllerManagerPtr AutomationControllerManager = this->GetAutomationController();

	if (this->TestDelegateHandle.IsValid() && AutomationControllerManager.IsValid())
	{
		GetAutomationController()->OnTestsComplete().Remove(this->TestDelegateHandle);
	}
}

bool FPF2TestState::HasBlockRun(const FPF2SpecBlockHandle& BlockHandle) const
{
	return this->BlocksRun.Contains(BlockHandle);
}

void FPF2TestState::MarkBlockAsRun(const FPF2SpecBlockHandle& BlockHandle)
{
	this->BlocksRun.Add(BlockHandle);
}

IAutomationControllerManagerPtr FPF2TestState::GetAutomationController()
{
	IAutomationControllerManagerPtr Result;

	IAutomationControllerModule* AutomationControllerModule =
		FModuleManager::GetModulePtr<IAutomationControllerModule>(TEXT("AutomationController"));

	if (AutomationControllerModule != nullptr)
	{
		Result = AutomationControllerModule->GetAutomationController();
	}

	return Result;
}

void FPF2TestState::ClearState()
{
	this->BlocksRun.Empty();
}
