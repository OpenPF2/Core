// OpenPF2 Game Framework for Unreal Engine, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <PropertyEditorModule.h>

#include <Modules/ModuleManager.h>

/**
 * Top-level module for the OpenPF2 Blueprint Nodes plug-in.
 *
 * This module exposes the logic that custom OpenPF2 Blueprint nodes execute at run-time in uncooked builds. In a cooked
 * build, nodes have already been compiled and this module is then not necessary.
 */
class FOpenPF2BlueprintNodes final : public IModuleInterface
{
public:
	// =================================================================================================================
	// Public Methods - IModuleInterface Implementation
	// =================================================================================================================
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
