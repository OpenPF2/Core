// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

/**
 * Constants related to OpenPF2 character logic.
 */
namespace PF2CharacterConstants
{
	/**
	 * The name of each weight "group" for passive GEs on a character.
	 *
	 * Weight groups are evaluated in the order that their names are sorted. Placing different GEs in different groups
	 * allows game designers to control the order that GEs are applied to an OpenPF2 character.
	 *
	 * For consistency, each name in this list must exactly match a tag in the "GameplayEffect.WeightGroup" group. This
	 * ensures that game designers have the same options for placing a GE in a weight group as exists in backend code.
	 */
	namespace GeWeightGroups
	{
		/**
		 * The weight group used for GEs that initialize base stats.
		 */
		static const FName InitializeBaseStats = FName(TEXT("GameplayEffect.WeightGroup.00_InitializeBaseStats"));

		/**
		 * The weight group used for GEs provided by the game designer that have to run right after base stats.
		 */
		static const FName PostInitializeBaseStats =
			FName(TEXT("GameplayEffect.WeightGroup.05_PostInitializeBaseStats"));

		/**
		 * The weight group used for GEs generated from other values on this character (managed by ASC logic).
		 */
		static const FName ManagedEffects = FName(TEXT("GameplayEffect.WeightGroup.10_ManagedEffects"));

		/**
		 * The default weight group for custom, passive GEs from a game designer; applied before ability boosts.
		 */
		static const FName PreAbilityBoosts = FName(TEXT("GameplayEffect.WeightGroup.15_PreAbilityBoosts"));

		/**
		 * The weight group used for ability boosts selected by the player or a game designer.
		 */
		static const FName AbilityBoosts = FName(TEXT("GameplayEffect.WeightGroup.20_AbilityBoosts"));

		/**
		 * The weight group used for custom GEs provided by the game designer that must run before the last group of
		 * stats GEs.
		 */
		static const FName PreFinalizeStats = FName(TEXT("GameplayEffect.WeightGroup.25_PreFinalizeStats"));

		/**
		 * The weight group used for GEs that need to run last because they heavily depend on the results of earlier
		 * GEs.
		 */
		static const FName FinalizeStats = FName(TEXT("GameplayEffect.WeightGroup.30_FinalizeStats"));
	}

	/**
	 * The path to the top-level folder for GE blueprints that are core to the plug-in.
	 */
	static const FString CoreBlueprintBasePath = TEXT("/OpenPF2Core/OpenPF2/Core/");

	/**
	 * The top-most path under the core blueprint base path.
	 */
	static const FString BlueprintSubfolderRoot = TEXT("");

	/**
	 * The path under the base path that contains character-stat-related blueprints.
	 */
	static const FString BlueprintSubfolderCharacterStats = TEXT("CharacterStats/");

	/**
	 * The path under the base path that contains ability-modifier-related blueprints.
	 */
	static const FString BlueprintSubfolderAbilityModifiers = TEXT("CharacterStats/AbilityModifiers/");

	/**
	 * The path under the base path that contains ability-boost-related blueprints.
	 */
	static const FString BlueprintSubfolderAbilityBoosts = TEXT("CharacterStats/AbilityBoosts/");

	/**
	 * The path under the base path that contains ancestry-related blueprints.
	 */
	static const FString BlueprintSubfolderAncestries = TEXT("CharacterStats/Ancestries/");

	/**
	 * The path under the blueprint base path that contains skill-related blueprints.
	 */
	static const FString BlueprintSubfolderSkillCalculations = TEXT("CharacterStats/Skills/");

	/**
	 * Format string for the name of the Gameplay Effect blueprint that boosts abilities.
	 */
	static const FString GeBlueprintBoostNameFormat = TEXT("GE_Boost{0}");

	/**
	 * The name of the Gameplay Effect that is used to apply tags that have been set on the character.
	 */
	static const FString GeDynamicTagsName = TEXT("GE_ApplyDynamicTags");

	/**
	* The name of the Gameplay Effect that is used to apply tags that have been set on the character.
	*/
	static const FString GeDynamicTagsClassName = FString::Format(TEXT("{0}_C"), { GeDynamicTagsName });

	/**
	 * Paths to Gameplay Effect Blueprints for core stat calculations in characters.
	 *
	 * The weight group tag on each passive GE controls the order in which it is applied. Base stat GEs are applied
	 * first, followed by ancestry and class GEs, ability boost GEs, additional passive GEs, and then all other core
	 * GEs. GEs that have the same weight group are applied in the order they have been added/listed here.
	 *
	 * Each key in this map is the sub-folder that contains the GE, while the value is the name of the GE.
	 *
	 * TODO: Consider whether we want to move this list into a Blueprint UPROPERTY, so that it's not hard-coded.
	 */
	static const TMultiMap<FString, FName> GeCoreCharacterBlueprintPaths = {
		// // Initialize base stats.
		{BlueprintSubfolderCharacterStats,    TEXT("GE_ApplyBaseCharacterStats")     },
		{BlueprintSubfolderCharacterStats,    TEXT("GE_GrantCharacterBaseAbilities") },
		{BlueprintSubfolderCharacterStats,    TEXT("GE_CalcKeyAbilityBoost")         },

		// Finalize stats.
		{BlueprintSubfolderAbilityModifiers,  TEXT("GE_CalcAbilityModifiers")        },
		{BlueprintSubfolderCharacterStats,    TEXT("GE_CalcClassDifficultyClass")    },
		{BlueprintSubfolderCharacterStats,    TEXT("GE_CalcArmorClass")              },
		{BlueprintSubfolderCharacterStats,    TEXT("GE_CalcPerceptionModifier")      },
		{BlueprintSubfolderCharacterStats,    TEXT("GE_CalcSavingThrowModifiers")    },
		{BlueprintSubfolderCharacterStats,    TEXT("GE_CalcSpellAttackRoll")         },
		{BlueprintSubfolderCharacterStats,    TEXT("GE_CalcSpellDifficultyClass")    },
		{BlueprintSubfolderSkillCalculations, TEXT("GE_CalcSkillModifiers")          },
		{BlueprintSubfolderAncestries,        TEXT("GE_CalcAncestryFeatLimit")       },
	};

	/**
	 * Returns the path to the Blueprint having the given name.
	 *
	 * @param Name
	 *	The name of the blueprint for which a path is desired.
	 * @param Subfolder
	 *	Optionally, the name of the folder in the base path where the blueprint is expected to be found. Defaults to
	 *	BlueprintSubfolderRoot, indicating the blueprint will be found at the root of the core blueprint base path.
	 *
	 * @return
	 *	The path to the blueprint.
	 */
	FORCEINLINE static FString GetBlueprintPath(const FName Name, const FString& Subfolder = BlueprintSubfolderRoot)
	{
		return FString::Format(TEXT("{0}{1}{2}.{2}_C"), { CoreBlueprintBasePath, Subfolder, Name.ToString() });
	}
}
