// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Libraries/PF2LoggingLibrary.h"

#include <Logging/LogVerbosity.h>

#include "Utilities/PF2LogIdentifiableInterface.h"

// =====================================================================================================================
// Macro Declarations
// =====================================================================================================================
#define OPENPF2_IMPLEMENT_LOGGING_LIBRARY_METHOD(LogCategory) \
	void UPF2LoggingLibrary::LogToOpen##LogCategory( \
		const FText Text, \
		const TScriptInterface<IPF2LogIdentifiableInterface> Subject, \
		const EPF2LogVerbosityLevel Verbosity) \
	{ \
		if (Subject == nullptr) \
		{ \
			switch (Verbosity) \
			{ \
				case EPF2LogVerbosityLevel::Fatal: \
					UE_LOG(Log##LogCategory, Fatal, TEXT("%s"), *(Text.ToString())); \
					break; \
				\
				case EPF2LogVerbosityLevel::Error: \
					UE_LOG(Log##LogCategory, Error, TEXT("%s"), *(Text.ToString())); \
					break; \
				\
				default: \
				case EPF2LogVerbosityLevel::Warning: \
					UE_LOG(Log##LogCategory, Warning, TEXT("%s"), *(Text.ToString())); \
					break; \
				\
				case EPF2LogVerbosityLevel::Display: \
					UE_LOG(Log##LogCategory, Display, TEXT("%s"), *(Text.ToString())); \
					break; \
				\
				case EPF2LogVerbosityLevel::Log: \
					UE_LOG(Log##LogCategory, Log, TEXT("%s"), *(Text.ToString())); \
					break; \
				\
				case EPF2LogVerbosityLevel::Verbose: \
					UE_LOG(Log##LogCategory, Verbose, TEXT("%s"), *(Text.ToString())); \
					break; \
				\
				case EPF2LogVerbosityLevel::VeryVerbose: \
					UE_LOG(Log##LogCategory, VeryVerbose, TEXT("%s"), *(Text.ToString())); \
					break; \
			} \
		} \
		else \
		{ \
			switch (Verbosity) \
			{ \
				case EPF2LogVerbosityLevel::Fatal: \
					UE_LOG(Log##LogCategory, Fatal, TEXT("[%s] %s"), *(Subject->GetIdForLogs()), *(Text.ToString())); \
					break; \
				\
				case EPF2LogVerbosityLevel::Error: \
					UE_LOG(Log##LogCategory, Error, TEXT("[%s] %s"), *(Subject->GetIdForLogs()), *(Text.ToString())); \
					break; \
				\
				default: \
				case EPF2LogVerbosityLevel::Warning: \
					UE_LOG(Log##LogCategory, Warning, TEXT("[%s] %s"), *(Subject->GetIdForLogs()), *(Text.ToString())); \
					break; \
				\
				case EPF2LogVerbosityLevel::Display: \
					UE_LOG(Log##LogCategory, Display, TEXT("[%s] %s"), *(Subject->GetIdForLogs()), *(Text.ToString())); \
					break; \
				\
				case EPF2LogVerbosityLevel::Log: \
					UE_LOG(Log##LogCategory, Log, TEXT("[%s] %s"), *(Subject->GetIdForLogs()), *(Text.ToString())); \
					break; \
				\
				case EPF2LogVerbosityLevel::Verbose: \
					UE_LOG(Log##LogCategory, Verbose, TEXT("[%s] %s"), *(Subject->GetIdForLogs()), *(Text.ToString())); \
					break; \
				\
				case EPF2LogVerbosityLevel::VeryVerbose: \
					UE_LOG(Log##LogCategory, VeryVerbose, TEXT("[%s] %s"), *(Subject->GetIdForLogs()), *(Text.ToString())); \
					break; \
			} \
		} \
	}

// =====================================================================================================================
// Normal Implementations
// =====================================================================================================================
OPENPF2_IMPLEMENT_LOGGING_LIBRARY_METHOD(Pf2Core)
OPENPF2_IMPLEMENT_LOGGING_LIBRARY_METHOD(Pf2CoreAbilities)
OPENPF2_IMPLEMENT_LOGGING_LIBRARY_METHOD(Pf2CoreEncounters)
OPENPF2_IMPLEMENT_LOGGING_LIBRARY_METHOD(Pf2CoreInitiative)
OPENPF2_IMPLEMENT_LOGGING_LIBRARY_METHOD(Pf2CoreInventory)
OPENPF2_IMPLEMENT_LOGGING_LIBRARY_METHOD(Pf2CoreStats)
OPENPF2_IMPLEMENT_LOGGING_LIBRARY_METHOD(Pf2CoreInput)
