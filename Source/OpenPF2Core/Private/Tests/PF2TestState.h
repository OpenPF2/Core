// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <IAutomationControllerManager.h>

struct FPF2SpecBlockHandle
{
	explicit FPF2SpecBlockHandle(int32 Handle) : Handle(Handle)
	{
	}

	explicit FPF2SpecBlockHandle()
	{
		this->Assign();
	}

	bool IsValid() const
	{
		return (Handle != INDEX_NONE);
	}

	void Assign();

	bool operator==(const FPF2SpecBlockHandle& Other) const
	{
		return (Handle == Other.Handle);
	}

	bool operator!=(const FPF2SpecBlockHandle& Other) const
	{
		return (Handle != Other.Handle);
	}

	friend uint32 GetTypeHash(const FPF2SpecBlockHandle& SpecHandle)
	{
		return ::GetTypeHash(SpecHandle.Handle);
	}

	FString ToString() const
	{
		return (IsValid() ? FString::FromInt(Handle) : TEXT("Invalid"));
	}

private:
	int32 Handle;
};

class FPF2TestState
{
protected:
	FDelegateHandle TestDelegateHandle;
	TSet<FPF2SpecBlockHandle> BlocksRun;

public:
	explicit FPF2TestState();
	~FPF2TestState();

	bool HasBlockRun(const FPF2SpecBlockHandle& BlockHandle) const;
	void MarkBlockAsRun(const FPF2SpecBlockHandle& BlockHandle);

protected:
	static IAutomationControllerManagerPtr GetAutomationController();
	void ClearState();
};
