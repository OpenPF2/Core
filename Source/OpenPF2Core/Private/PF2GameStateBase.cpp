// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "PF2GameStateBase.h"

#include <Engine/World.h>

#include <Net/UnrealNetwork.h>

#include "OpenPF2Core.h"
#include "PF2PlayerControllerInterface.h"

#include "ModesOfPlay/PF2ModeOfPlayRuleSetInterface.h"

#include "Utilities/PF2EnumUtilities.h"
#include "Utilities/PF2LogUtilities.h"

APF2GameStateBase::APF2GameStateBase() : NextPlayerIndex(0), NextPartyIndex(0)
{
}

void APF2GameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APF2GameStateBase, ModeOfPlay);
	DOREPLIFETIME(APF2GameStateBase, ModeOfPlayRuleSet);
}

void APF2GameStateBase::SetModeOfPlay(const EPF2ModeOfPlayType                               NewMode,
                                      const TScriptInterface<IPF2ModeOfPlayRuleSetInterface> NewRuleSet)
{
	if (this->HasAuthority())
	{
		UE_LOG(
			LogPf2Core,
			Verbose,
			TEXT("Transitioning from current mode of play (%s) to new mode (%s)."),
			*PF2EnumUtilities::ToString(this->ModeOfPlay),
			*PF2EnumUtilities::ToString(NewMode)
		);

		this->ModeOfPlay        = NewMode;
		this->ModeOfPlayRuleSet = NewRuleSet;

		// We're running on the server; notify server copies of the game state that we have received a mode of play.
		this->Native_OnModeOfPlayAvailable();
	}
}

void APF2GameStateBase::OnRep_ModeOfPlay()
{
	UE_LOG(
		LogPf2Core,
		VeryVerbose,
		TEXT("[%s] Mode of play has been replicated."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld()))
	);

	// We're running on the client; notify the client that we have received a mode of play.
	this->Native_OnModeOfPlayAvailable();
}

void APF2GameStateBase::Native_OnModeOfPlayAvailable()
{
	const UWorld* const World = this->GetWorld();

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		IPF2PlayerControllerInterface* const PF2PlayerController =
			Cast<IPF2PlayerControllerInterface>(PlayerController);

		if (PF2PlayerController != nullptr)
		{
			PF2PlayerController->Native_OnModeOfPlayChanged(this->ModeOfPlay);
		}
	}
}
