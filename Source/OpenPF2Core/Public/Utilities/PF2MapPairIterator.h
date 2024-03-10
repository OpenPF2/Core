// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "PF2MapPairIterator.generated.h"

/**
 * A Blueprint-friendly wrapper around an FScriptMapHelper pair iterator.
 *
 * This object abstracts away the minutia of working with maps of any type of key or value.
 */
USTRUCT(BlueprintType)
struct FPF2MapPairIterator
{
	GENERATED_BODY()

	/**
	 * Default constructor for FPF2MapPairIterator.
	 */
	explicit FPF2MapPairIterator() : MapHelper(nullptr), Iterator(nullptr)
	{
	}

	/**
	 * Constructs an iterator for navigating through the pairs of the given map.
	 *
	 * @param MapProperty
	 *	Metadata about the structure and type of map.
	 * @param MapData
	 *	A pointer to the raw data in the map.
	 */
	explicit FPF2MapPairIterator(const FMapProperty* MapProperty, const void* MapData) : Iterator(nullptr)
	{
		this->MapHelper = MakeShared<FScriptMapHelper>(MapProperty, MapData);
		this->Iterator  = MakeShared<FScriptMapHelper::FIterator>(*this->MapHelper);
	}

	/**
	 * Gets whether this iterator is currently pointing at a valid pair/location in the map.
	 *
	 * @return
	 *	- true if the iterator is pointing at a valid pair in the element.
	 *	- false if the iterator has moved past the last element in the map or was initialized on an empty map.
	 */
	FORCEINLINE bool HasPair() const
	{
		return ((this->Iterator != nullptr) && *this->Iterator);
	}

	/**
	 * Increments the position of the iterator to the next pair, if available.
	 *
	 * @return
	 *	This object.
	 */
	FORCEINLINE FPF2MapPairIterator& operator++()
	{
		++(*this->Iterator);
		return *this;
	}

	/**
	 * Gets the key from the current pair and copies it into the variable de-referenced by the given pointer.
	 *
	 * @param OutKeyPtr
	 *	A pointer to the address where the value should be copied. This should always correspond to the BASE + OFFSET,
	 *	where:
	 *	- BASE   = (for member properties) the address of the UObject which contains this data
	 *	           (for locals/parameters) the address of the space allocated for the function's locals
	 *	- OFFSET = the Offset of the FProperty
	 */
	FORCEINLINE void GetCurrentKey(void* OutKeyPtr) const
	{
		check(this->HasPair());

		{
			const uint8*     MapRelativeKeyPtr = this->MapHelper->GetKeyPtr(**this->Iterator);
			const FProperty* KeyProperty       = this->MapHelper->GetKeyProperty();

			KeyProperty->CopyCompleteValueFromScriptVM(OutKeyPtr, MapRelativeKeyPtr);
		}
	}

	/**
	 * Gets the value from the current pair and copies it into the variable de-referenced by the given pointer.
	 *
	 * @param OutValuePtr
	 *	A pointer to the address where the value should be copied. This should always correspond to the BASE + OFFSET,
	 *	where:
	 *	- BASE   = (for member properties) the address of the UObject which contains this data
	 *	           (for locals/parameters) the address of the space allocated for the function's locals
	 *	- OFFSET = the Offset of the FProperty
	 */
	FORCEINLINE void GetCurrentValue(void* OutValuePtr) const
	{
		check(this->HasPair());

		{
			const uint8*     MapRelativeValuePtr = this->MapHelper->GetValuePtr(**this->Iterator);
			const FProperty* ValueProperty       = this->MapHelper->GetValueProperty();

			ValueProperty->CopyCompleteValueFromScriptVM(OutValuePtr, MapRelativeValuePtr);
		}
	}

protected:
	/**
	 * A wrapper around the raw data of the map.
	 */
	TSharedPtr<FScriptMapHelper> MapHelper;

	/**
	 * A special iterator provided by the map helper for navigating through map pairs.
	 */
	TSharedPtr<FScriptMapHelper::FIterator> Iterator;
};
