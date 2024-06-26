﻿// OpenPF2 Game Framework for Unreal Engine, Copyright 2022-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Components/ActorComponent.h>

#include <GameFramework/Actor.h>

#include "PF2ArrayUtilities.h"

#include "UObject/ScriptInterface.h"

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Utility logic for working with UObjects that implement UInterfaces.
 */
namespace PF2InterfaceUtilities
{
	/**
	 * Converts a UObject that implements an interface into a TScriptInterface object.
	 *
	 * The provided object MUST be a UObject or the conversion will fail. This is because Blueprint can only interact
	 * with UObjects so TScriptInterface has to wrap a UObject.
	 *
	 * This can be used to reduce impedance between parts of the OpenPF2 API that use TScriptInterface (because they're
	 * exposed to Blueprint) and parts that use interfaces (because they are not exposed to Blueprint). When possible,
	 * use TScriptInterface to avoid having to use this method.
	 *
	 * @tparam InterfaceType
	 *	The type of interface (e.g., IMyInterface).
	 *
	 * @param InterfaceObject
	 *	The interface object to convert to being wrapped in a script interface object. This must be a UObject.
	 *
	 * @return
	 *	The wrapped, Blueprint-friendly script interface object.
	 */
	template<class InterfaceType>
	FORCEINLINE OPENPF2GAMEFRAMEWORK_API TScriptInterface<InterfaceType> ToScriptInterface(InterfaceType* InterfaceObject)
	{
		UObject* Object = Cast<UObject>(InterfaceObject);

		checkf(
			Object != nullptr,
			TEXT("Only a UObject that implements the interface can be provided to this method. TScriptInterface does not support unmanaged object types.")
		);

		return TScriptInterface<InterfaceType>(Object);
	}

	/**
	 * Converts an array of UObjects that implement an interface into an array of TScriptInterface objects.
	 *
	 * The provided objects MUST be UObjects or the conversion will fail. This is because Blueprint can only interact
	 * with UObjects so each TScriptInterface has to wrap a UObject.
	 *
	 * This can be used to reduce impedance between parts of the OpenPF2 API that use TScriptInterface (because they're
	 * exposed to Blueprint) and parts that use interfaces (because they are not exposed to Blueprint). When possible,
	 * use TScriptInterface to avoid having to use this method.
	 *
	 * @tparam InterfaceType
	 *	The type of interface (e.g., IMyInterface).
	 *
	 * @param InterfaceObjects
	 *	The interface objects to convert to being wrapped in script interface objects. These must be UObjects.
	 *
	 * @return
	 *	The wrapped, Blueprint-friendly script interface objects.
	 */
	template <class InterfaceType>
	FORCEINLINE OPENPF2GAMEFRAMEWORK_API TArray<TScriptInterface<InterfaceType>> ToScriptInterfaces(
		const TArray<InterfaceType*> InterfaceObjects)
	{
		return PF2ArrayUtilities::Map<TScriptInterface<InterfaceType>>(
			InterfaceObjects,
			[](InterfaceType* InterfaceObject)
			{
				return ToScriptInterface<InterfaceType>(InterfaceObject);
			}
		);
	}

	/**
	 * Unwraps the interface object inside a TScriptInterface object into the appropriate "raw" interface type.
	 *
	 * This can be used to reduce impedance between parts of the OpenPF2 API that use TScriptInterface (because they're
	 * exposed to Blueprint) and parts that use interfaces (because they are not exposed to Blueprint). When possible,
	 * use TScriptInterface all the way through to avoid having to use this method.
	 *
	 * @tparam InterfaceType
	 *	The type of interface (e.g., IMyInterface).
	 *
	 * @param ScriptInterface
	 *	The script interface object that wraps the interface object.
	 *
	 * @return
	 *	The unwrapped interface object.
	 */
	template<class InterfaceType>
	FORCEINLINE OPENPF2GAMEFRAMEWORK_API InterfaceType* FromScriptInterface(TScriptInterface<InterfaceType> ScriptInterface)
	{
		return static_cast<InterfaceType*>(ScriptInterface.GetInterface());
	}

	/**
	 * Unwraps the interface objects inside TScriptInterface objects into an array of "raw" interface types.
	 *
	 * This can be used to reduce impedance between parts of the OpenPF2 API that use TScriptInterface (because they're
	 * exposed to Blueprint) and parts that use interfaces (because they are not exposed to Blueprint). When possible,
	 * use TScriptInterface all the way through to avoid having to use this method.
	 *
	 * @tparam InterfaceType
	 *	The type of interface (e.g., IMyInterface).
	 *
	 * @param ScriptInterfaces
	 *	The script interface objects that wrap interface objects.
	 *
	 * @return
	 *	The unwrapped interface objects.
	 */
	template <class InterfaceType>
	FORCEINLINE OPENPF2GAMEFRAMEWORK_API TArray<InterfaceType*> FromScriptInterfaces(
		const TArray<TScriptInterface<InterfaceType>> ScriptInterfaces)
	{
		return PF2ArrayUtilities::Map<InterfaceType*>(
			ScriptInterfaces,
			[](TScriptInterface<InterfaceType> ScriptInterface)
			{
				return FromScriptInterface<InterfaceType>(ScriptInterface);
			}
		);
	}

	/**
	 * Locates the actor component that implements the specified interface.
	 *
	 * @tparam InterfaceType
	 *	The type of interface (e.g., IMyInterface).
	 * @tparam UObjectType
	 *	The UObject that corresponds to the interface type (e.g., UMyInterface).
	 *
	 * @param Actor
	 *	The actor that contains the desired component.
	 */
	template <class InterfaceType, class UObjectType>
	FORCEINLINE OPENPF2GAMEFRAMEWORK_API InterfaceType* FindComponentByInterface(AActor* Actor)
	{
		InterfaceType*                 Result     = nullptr;
		const TArray<UActorComponent*> Components = Actor->GetComponentsByInterface(UObjectType::StaticClass());

		checkf(
			Components.Num() < 2,
			TEXT("More than one component implements the same interface: %s"),
			*(UObjectType::StaticClass()->GetName())
		);

		for (UActorComponent* Component : Components)
		{
			Result = Cast<InterfaceType>(Component);
			break;
		}

		return Result;
	}
}
