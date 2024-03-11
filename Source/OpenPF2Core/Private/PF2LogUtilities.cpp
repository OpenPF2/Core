// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Utilities/PF2LogUtilities.h"

namespace PF2LogUtilities
{
	FString GetHostNetId(const UWorld* World)
	{
		FString NetPrefix = TEXT("UNK");

		if (World != nullptr)
		{
			// Inspired by UKismetSystemLibrary::PrintString().
			switch (World->GetNetMode())
			{
				case NM_Client:
					// GPlayInEditorID 0 is always the server, so 1 will be first client.
					NetPrefix = FString::Printf(TEXT("CLNT %d"), static_cast<int>(GPlayInEditorID));
					break;

				case NM_DedicatedServer:
					NetPrefix = FString::Printf(TEXT("DED SRV"));
					break;

				case NM_ListenServer:
					NetPrefix = FString::Printf(TEXT("LSTN SRV"));
					break;

				default:
				case NM_Standalone:
					NetPrefix = FString::Printf(TEXT("SA SRV"));
					break;
			}
		}

		return NetPrefix;
	}
}
