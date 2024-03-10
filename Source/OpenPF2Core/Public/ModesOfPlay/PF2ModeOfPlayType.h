// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//   - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//   - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//   - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
//
// Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
// as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
// file other than the material designated as Open Game Content may be reproduced in any form without written
// permission.

#pragma once

/**
 * An enumeration of the different play modes in OpenPF2.
 *
 * From the Pathfinder 2E Core Rulebook, page 493, "Running Modes of Play":
 * "Pathfinder sessions are divided into three different modes of play: encounters, exploration, and downtime. Each mode
 * represents different kinds of situations, with specific stakes and time scales, and characters can use different
 * sorts of actions and reactions in each."
 */
UENUM(BlueprintType)
enum class EPF2ModeOfPlayType : uint8
{
	/**
	 * No mode of play is currently initialized.
	 */
	None,

	/**
	 * Encounter mode.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 468, "Encounter Mode":
	 * "When every individual action counts, you enter the encounter mode of play. In this mode, time is divided into
	 * rounds, each of which is 6 seconds of time in the game world. Every round, each participant takes a turn in an
	 * established order. During your turn, you can use actions, and depending on the details of the encounter, you
	 * might have the opportunity to use reactions and free actions on your own turn and on others’ turns."
	 */
	Encounter,

	/**
	 * Exploration mode.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 479, "Exploration Mode":
	 * "While encounters use rounds for combat, exploration is more free form. The GM determines the flow of time, as
	 * you could be traveling by horseback across craggy highlands, negotiating with merchants, or delving in a dungeon
	 * in search of danger and treasure. Exploration lacks the immediate danger of encounter mode, but it offers its own
	 * challenges."
	 */
	Exploration,

	/**
	 * Downtime mode.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 481, "Downtime Mode":
	 * "Downtime mode is played day-by-day rather than minute-by-minute or scene-by-scene. Usually this mode of play
	 * occurs when you are in the safety of a settlement, maybe recovering from your adventures or studying an artifact
	 * you found."
	 */
	Downtime,
};
