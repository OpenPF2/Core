// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "CharacterStats/PF2CharacterAttributeStaticsBase.h"

/**
 * Singleton container for OpenPF2 character attribute capture definitions for target actors.
 */
struct OPENPF2CORE_API FPF2TargetCharacterAttributeStatics final : FPF2CharacterAttributeStaticsBase
{
protected:
	// =================================================================================================================
	// Protected Constants
	// =================================================================================================================
	/**
	 * Map from each transient damage attribute to the attribute tracking resistance to that damage type.
	 */
	inline static const TMap<FName, FName> DamageAttributeToResistanceAttributeMap = {
		{ "TmpDmgTypePhysicalBludgeoning", "RstPhysicalBludgeoning" },
		{ "TmpDmgTypePhysicalPiercing",    "RstPhysicalPiercing"    },
		{ "TmpDmgTypePhysicalSlashing",    "RstPhysicalSlashing"    },

		{ "TmpDmgTypeEnergyAcid",          "RstEnergyAcid"          },
		{ "TmpDmgTypeEnergyCold",          "RstEnergyCold"          },
		{ "TmpDmgTypeEnergyElectricity",   "RstEnergyElectricity"   },
		{ "TmpDmgTypeEnergyFire",          "RstEnergyFire"          },
		{ "TmpDmgTypeEnergySonic",         "RstEnergySonic"         },
		{ "TmpDmgTypeEnergyPositive",      "RstEnergyPositive"      },
		{ "TmpDmgTypeEnergyNegative",      "RstEnergyNegative"      },
		{ "TmpDmgTypeEnergyForce",         "RstEnergyForce"         },

		{ "TmpDmgTypeAlignmentChaotic",    "RstAlignmentChaotic"    },
		{ "TmpDmgTypeAlignmentEvil",       "RstAlignmentEvil"       },
		{ "TmpDmgTypeAlignmentGood",       "RstAlignmentGood"       },
		{ "TmpDmgTypeAlignmentLawful",     "RstAlignmentLawful"     },

		{ "TmpDmgTypeMental",              "RstMental"              },
		{ "TmpDmgTypePoison",              "RstPoison"              },
		{ "TmpDmgTypeBleed",               "RstBleed"               },
		{ "TmpDmgTypePrecision",           "RstPrecision"           },
	};

public:
	// =================================================================================================================
	// Public Static Methods
	// =================================================================================================================
	/**
	 * Gets an instance of this container.
	 *
	 * @return
	 *	A reference to the capture definition container.
	 */
	FORCEINLINE static const FPF2TargetCharacterAttributeStatics& GetInstance()
	{
		static FPF2TargetCharacterAttributeStatics AttributeStatics;

		return AttributeStatics;
	}

	// =================================================================================================================
	// Attribute Capture Definitions
	// =================================================================================================================
	DECLARE_ATTRIBUTE_CAPTUREDEF(TmpDamageIncoming);
	DECLARE_ATTRIBUTE_CAPTUREDEF(TmpLastIncomingAttackDegreeOfSuccess);

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets the transient damage attribute capture definition for the damage type that has the given tag name.
	 *
	 * @param DamageAttributeName
	 *	The name of the transient damage attribute for which a resistance capture definition is desired.
	 *
	 * @return
	 *	Either the desired capture definition; or nullptr if the character is using an ASC that does not provide a
	 *	resistance attribute that corresponds to the specified transient damage attribute.
	 */
	FORCEINLINE const FGameplayEffectAttributeCaptureDefinition* GetResistanceCaptureForDamageAttribute(
		const FString& DamageAttributeName) const
	{
		return GetResistanceCaptureForDamageAttribute(FName(DamageAttributeName));
	}

	/**
	 * Gets the transient damage attribute capture definition for the damage type that has the given tag name.
	 *
	 * @param DamageAttributeName
	 *	The name of the transient damage attribute for which a resistance capture definition is desired.
	 *
	 * @return
	 *	Either the desired capture definition; or nullptr if the character is using an ASC that does not provide a
	 *	resistance attribute that corresponds to the specified transient damage attribute.
	 */
	const FGameplayEffectAttributeCaptureDefinition* GetResistanceCaptureForDamageAttribute(
		const FName& DamageAttributeName) const;

private:
	// =================================================================================================================
	// Private Constructors
	// =================================================================================================================
	/**
	 * Constructor for FPF2TargetCharacterAttributeStatics.
	 */
	explicit FPF2TargetCharacterAttributeStatics();
};
