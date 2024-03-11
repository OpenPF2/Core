// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayEffectAttributeCaptureDefinition.h>

/**
 * Base struct for singleton containers of attribute capture definitions.
 *
 * These struct centralize static capture definitions for attributes instead of there being multiple, smaller "Statics"
 * definitions like those preferred by Epic's sample projects.
 */
struct OPENPF2CORE_API FPF2AttributeStaticsBase
{
protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * A map of all capture definitions, keyed by property name.
	 */
	TMap<FString, const FGameplayEffectAttributeCaptureDefinition*> CaptureDefinitions;

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets all attribute capture definitions.
	 *
	 * @return
	 *	An array of all the capture definitions for character attributes.
	 */
	FORCEINLINE TArray<const FGameplayEffectAttributeCaptureDefinition*> GetCaptureDefinitions() const
	{
		TArray<const FGameplayEffectAttributeCaptureDefinition*> Result;

		this->CaptureDefinitions.GenerateValueArray(Result);

		return Result;
	}

	/**
	 * Gets a capture definition for the specified attribute.
	 *
	 * @param Name
	 *	The name of the attribute for which a capture definition is desired.
	 *
	 * @return
	 *	Either the desired capture definition; or nullptr if the given attribute name doesn't correspond to a value in
	 *	the attribute set.
	 */
	FORCEINLINE const FGameplayEffectAttributeCaptureDefinition* GetCaptureByAttributeName(const FString& Name) const
	{
		if (this->CaptureDefinitions.Contains(Name))
		{
			return this->CaptureDefinitions[Name];
		}
		else
		{
			UE_LOG(
				LogPf2CoreStats,
				Error,
				TEXT("No attribute capture corresponds to attribute name '%s'."),
				*Name
			);

			return nullptr;
		}
	}
};
