// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <AttributeSet.h>

// =====================================================================================================================
// Macros
// =====================================================================================================================
// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(S, P, T, B) \
{ \
	DEFINE_ATTRIBUTE_CAPTUREDEF(S, P, T, B) \
	this->CaptureDefinitions.Add(P##Property->GetName(), &P##Def); \
}

#define DEFINE_PF2_ABILITY_SCORE_CAPTUREDEF(S, P, T, B) \
{ \
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(S, P, T, B) \
	this->AbilityNames.Add(P##Property->GetName()); \
}

#define DEFINE_PF2_ABILITY_MODIFIER_CAPTUREDEF(S, P, T, B) \
{ \
	DEFINE_PF2_ATTRIBUTE_CAPTUREDEF(S, P, T, B) \
	this->AbilityModifierNames.Add(P##Property->GetName()); \
}
