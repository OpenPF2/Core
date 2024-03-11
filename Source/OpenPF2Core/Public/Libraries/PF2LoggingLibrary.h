// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>

#include "PF2LoggingLibrary.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2LogIdentifiableInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * A subset of log verbosity levels defined in ELogVerbosity::Type that are supported for use in blueprints.
 */
UENUM()
enum EPF2LogVerbosityLevel
{
	Fatal,
	Error,
	Warning,
	Display,
	Log,
	Verbose,
	VeryVerbose,
};

/**
 * Function library for writing to OpenPF2 logs from blueprints.
 */
UCLASS()
class UPF2LoggingLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Logs to the category for logic evaluated by the OpenPF2 core code.
	 *
	 * @param Text
	 *	The text to log.
	 * @param Subject
	 *	An optional parameter to provide an ID that will be prefixed to the log message. If omitted, no prefix will be
	 *	added.
	 * @param Verbosity
	 *	The verbosity level at which to log.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Utility|Logging", DisplayName="Log to OpenPF2 Core")
	static void LogToOpenPf2Core(
		const FText                                          Text,
		const TScriptInterface<IPF2LogIdentifiableInterface> Subject,
		const EPF2LogVerbosityLevel                          Verbosity = VeryVerbose);

	/**
	 * Logs to the category for logic evaluated by OpenPF2 code that executes abilities and actions.
	 *
	 * @param Text
	 *	The text to log.
	 * @param Subject
	 *	An optional parameter to provide an ID that will be prefixed to the log message. If omitted, no prefix will be
	 *	added.
	 * @param Verbosity
	 *	The verbosity level at which to log.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Utility|Logging", DisplayName="Log to OpenPF2 Core Abilities")
	static void LogToOpenPf2CoreAbilities(
		const FText                                          Text,
		const TScriptInterface<IPF2LogIdentifiableInterface> Subject,
		const EPF2LogVerbosityLevel                          Verbosity = VeryVerbose);

	/**
	 * Logs to the category for logic evaluated by OpenPF2 during encounters.
	 *
	 * @param Text
	 *	The text to log.
	 * @param Subject
	 *	An optional parameter to provide an ID that will be prefixed to the log message. If omitted, no prefix will be
	 *	added.
	 * @param Verbosity
	 *	The verbosity level at which to log.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Utility|Logging", DisplayName="Log to OpenPF2 Core Encounters")
	static void LogToOpenPf2CoreEncounters(
		const FText                                          Text,
		const TScriptInterface<IPF2LogIdentifiableInterface> Subject,
		const EPF2LogVerbosityLevel                          Verbosity = VeryVerbose);

	/**
	 * Logs to the category for character-initiative management logic evaluated by OpenPF2, usually during encounters.
	 *
	 * @param Text
	 *	The text to log.
	 * @param Subject
	 *	An optional parameter to provide an ID that will be prefixed to the log message. If omitted, no prefix will be
	 *	added.
	 * @param Verbosity
	 *	The verbosity level at which to log.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Utility|Logging", DisplayName="Log to OpenPF2 Core Initiative")
	static void LogToOpenPf2CoreInitiative(
		const FText                                          Text,
		const TScriptInterface<IPF2LogIdentifiableInterface> Subject,
		const EPF2LogVerbosityLevel                          Verbosity = VeryVerbose);

	/**
	 * Logs to the category for inventory management logic evaluated by OpenPF2.
	 *
	 * @param Text
	 *	The text to log.
	 * @param Subject
	 *	An optional parameter to provide an ID that will be prefixed to the log message. If omitted, no prefix will be
	 *	added.
	 * @param Verbosity
	 *	The verbosity level at which to log.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Utility|Logging", DisplayName="Log to OpenPF2 Core Inventory")
	static void LogToOpenPf2CoreInventory(
		const FText                                          Text,
		const TScriptInterface<IPF2LogIdentifiableInterface> Subject,
		const EPF2LogVerbosityLevel                          Verbosity = VeryVerbose);

	/**
	 * Logs to the category for character abilities and stats (very verbose).
	 *
	 * @param Text
	 *	The text to log.
	 * @param Subject
	 *	An optional parameter to provide an ID that will be prefixed to the log message. If omitted, no prefix will be
	 *	added.
	 * @param Verbosity
	 *	The verbosity level at which to log.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Utility|Logging", DisplayName="Log to OpenPF2 Core Stats")
	static void LogToOpenPf2CoreStats(
		const FText                                          Text,
		const TScriptInterface<IPF2LogIdentifiableInterface> Subject,
		const EPF2LogVerbosityLevel                          Verbosity = VeryVerbose);

	/**
	 * Logs to the category for handling input from a player.
	 *
	 * @param Text
	 *	The text to log.
	 * @param Subject
	 *	An optional parameter to provide an ID that will be prefixed to the log message. If omitted, no prefix will be
	 *	added.
	 * @param Verbosity
	 *	The verbosity level at which to log.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Utility|Logging", DisplayName="Log to OpenPF2 Core Input")
	static void LogToOpenPf2CoreInput(
		const FText                                          Text,
		const TScriptInterface<IPF2LogIdentifiableInterface> Subject,
		const EPF2LogVerbosityLevel                          Verbosity = VeryVerbose);
};
