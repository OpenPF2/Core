// OpenPF2 for UE Game Logic, Copyright 2021-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Containers/Map.h>

/**
 * Various utilities for functional programming with maps and similar structures.
 */
namespace PF2MapUtilities
{
	/**
	 * Collapses all of the values of a map to a single value by use of a transformation function.
	 *
	 * The transformation function receives a pair of values -- the "previous value" and the "current value". The
	 * transformation function is invoked once for each tuple of the original map. For the first value of the map,
	 * the "previous value" is the starting value supplied to this function, and the "current value" is the first tuple
	 * in the original map. For each subsequent call, the "previous value" is the result that was returned during the
	 * previous call of the transformation function.
	 *
	 * The original map is not modified.
	 *
	 * @tparam Key
	 *	The type of keys in the map. Must support hash codes.
	 * @tparam Value
	 *	The type of values in the map.
	 * @tparam Out
	 *	The type of result to return. (The type into which all input elements will be consolidated).
	 * @tparam Func
	 *	The type of the lambda function to invoke on each element of the input map, applying a transformation on the
	 *	element and the accumulated result to return a new output value.
	 *
	 * @param InputMap
	 *	The map of values to reduce.
	 * @param StartingValue
	 *	The value passed in to the Callable along side the first value of the map. If the map is empty, this will be the
	 *	value returned.
	 * @param Callable
	 *	The transformation function/lambda invoked to combine each element with the result of flattening/reducing the
	 *	previous element. This function is expected to take in the following two parameters and return a value that
	 *	matches the "Out" type:
	 *	  - PreviousValue: Which must match the "Out" type.
	 *	  - CurrentValue: Which is a pair of the "Key" and "Value" types.
	 *
	 * @return
	 *	The result of reducing the values of the map.
	 */
	template <typename Key, typename Value, typename Out, typename Func>
	Out Reduce(const TMap<Key, Value> InputMap, const Out StartingValue, const Func Callable)
	{
		Out PreviousValue = StartingValue;

		for (const TPair<Key, Value>& CurrentValue : InputMap)
		{
			PreviousValue = Callable(PreviousValue, CurrentValue);
		}

		return PreviousValue;
	}
	/**
	 * Collapses all of the values of a multimap to a single value by use of a transformation function.
	 *
	 * The transformation function receives a pair of values -- the "previous value" and the "current value". The
	 * transformation function is invoked once for each tuple of the original map. For the first value of the map,
	 * the "previous value" is the starting value supplied to this function, and the "current value" is the first tuple
	 * in the original map. For each subsequent call, the "previous value" is the result that was returned during the
	 * previous call of the transformation function.
	 *
	 * The original map is not modified.
	 *
	 * @tparam Key
	 *	The type of keys in the map. Must support hash codes.
	 * @tparam Value
	 *	The type of values in the map.
	 * @tparam Out
	 *	The type of result to return. (The type into which all input elements will be consolidated).
	 * @tparam Func
	 *	The type of the lambda function to invoke on each element of the input map, applying a transformation on the
	 *	element and the accumulated result to return a new output value.
	 *
	 * @param InputMap
	 *	The map of values to reduce.
	 * @param StartingValue
	 *	The value passed in to the Callable along side the first value of the map. If the map is empty, this will be the
	 *	value returned.
	 * @param Callable
	 *	The transformation function/lambda invoked to combine each element with the result of flattening/reducing the
	 *	previous element. This function is expected to take in the following two parameters and return a value that
	 *	matches the "Out" type:
	 *	  - PreviousValue: Which must match the "Out" type.
	 *	  - CurrentValue: Which is a pair of the "Key" and "Value" types.
	 *
	 * @return
	 *	The result of reducing the values of the map.
	 */
	template <typename Key, typename Value, typename Out, typename Func>
	Out Reduce(const TMultiMap<Key, Value> InputMap, const Out StartingValue, const Func Callable)
	{
		Out PreviousValue = StartingValue;

		for (const TPair<Key, Value>& CurrentValue : InputMap)
		{
			PreviousValue = Callable(PreviousValue, CurrentValue);
		}

		return PreviousValue;
	}

	/**
	 * Inverts the keys and values of a map, so that for each pair the key becomes the value and vice-versa.
	 *
	 * @tparam Key
	 *	The type of keys in the map. Must support hash codes.
	 * @tparam Value
	 *	The type of values in the map.
	 *
	 * @param InputMap
	 *	The map to invert.
	 *
	 * @return
	 *	The inverted map.
	 */
	template<typename Key, typename Value>
	TMap<Value, Key> Invert(const TMap<Key, Value> InputMap)
	{
		TMap<Value, Key> OutputMap;

		OutputMap.Reserve(InputMap.Num());

		for (const auto& Pair : InputMap)
		{
			OutputMap.Add(Pair.Value, Pair.Key);
		}

		return OutputMap;
	}
}
