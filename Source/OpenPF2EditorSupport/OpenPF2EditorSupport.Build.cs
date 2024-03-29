// OpenPF2 for UE Game Logic, Copyright 2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

using UnrealBuildTool;

// ReSharper disable once InconsistentNaming
public class OpenPF2EditorSupport : ModuleRules
{
	public OpenPF2EditorSupport(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

		PrivatePCHHeaderFile = "Public/OpenPF2EditorSupport.h";

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"OpenPF2Core",
				"OpenPF2BlueprintNodes",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"BlueprintGraph",
				"Core",
				"CoreUObject",
				"DetailCustomizations",
				"Engine",
				"GraphEditor",
				"Kismet",
				"KismetCompiler",
				"PropertyEditor",
				"Slate",
				"SlateCore",
				"SubobjectEditor",
				"UnrealEd",
			}
		);
	}
}
