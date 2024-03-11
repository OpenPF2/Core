// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Commands/PF2AbilityExecutionFilterBase.h"

#include <Engine/World.h>

// ReSharper disable once CppUnusedIncludeDirective
#include <GameFramework/GameStateBase.h>

#include "PF2GameStateInterface.h"

#include "Utilities/PF2InterfaceUtilities.h"

UWorld* UPF2AbilityExecutionFilterBase::GetWorld() const
{
	// This override exists just to avoid shadowing the base class implementation.
	return UObject::GetWorld();
}

FString UPF2AbilityExecutionFilterBase::GetIdForLogs() const
{
	return this->GetName();
}

UWorld* UPF2AbilityExecutionFilterBase::GetWorld(const FPF2AbilityExecutionFilterContext ExecutionContext)
{
	return ExecutionContext.GetWorld();
}

TScriptInterface<IPF2GameStateInterface> UPF2AbilityExecutionFilterBase::GetGameState(
	const FPF2AbilityExecutionFilterContext& ExecutionContext)
{
	TScriptInterface<IPF2GameStateInterface> Result;
	const UWorld*                            World = GetWorld(ExecutionContext);

	if (World == nullptr)
	{
		Result = TScriptInterface<IPF2GameStateInterface>(nullptr);
	}
	else
	{
		IPF2GameStateInterface* GameStateIntf = Cast<IPF2GameStateInterface>(World->GetGameState());

		Result = PF2InterfaceUtilities::ToScriptInterface(GameStateIntf);
	}

	return Result;
}
