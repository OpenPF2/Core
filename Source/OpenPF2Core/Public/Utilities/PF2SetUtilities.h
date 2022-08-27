﻿// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

/**
 * Various utilities for functional programming with sets and similar structures.
 */
namespace PF2SetUtilities
{
	/**
	 * Applies a transformation function to the values in a set, then returns the result of the transformation.
	 *
	 * The original set is not modified.
	 *
	 * @param Elements
	 *	The set of values to map.
	 * @param Callable
	 *	The transformation function/lambda invoked on each element in order to get the mapped value.
	 *
	 * @return
	 *	An set of the values that resulted from applying the transformation to every value of the source set.
	 */
	template <typename Out, typename In, typename Func>
	TSet<Out> Map(const TSet<In> Elements, const Func Callable)
	{
		TSet<Out> Result;

		for (const In& Element : Elements)
		{
			const Out MappingOutput = Callable(Element);

			Result.Add(MappingOutput);
		}

		return Result;
	}

	/**
	 * Collapses all of the values of a set to a single value by use of a transformation function.
	 *
	 * The transformation function receives a pair of values -- the "previous value" and the "current value". The
	 * transformation function is invoked once for each value in the original set. For the first value of the set,
	 * the "previous value" is the starting value supplied to this function, and the "current value" is the first value
	 * in the original set. For each subsequent call, the "previous value" is the result that was returned during the
	 * previous call of the transformation function.
	 *
	 * The original set is not modified.
	 *
	 * @param Elements
	 *	The set of values to reduce.
	 * @param StartingValue
	 *	The value passed-in to the Callable along side the first value of the set. If the set is empty, this will be the
	 *	value returned.
	 * @param Callable
	 *	The transformation function/lambda invoked to combine each element with the result of flattening/reducing the
	 *	previous element. This function is expected to take in the following two parameters and return a value that
	 *	matches the "Out" type:
	 *	  - PreviousValue: Which must match the "Out" type.
	 *	  - CurrentValue: Which must match the "In" type.
	 *
	 * @return
	 *	The result of reducing the values of the set.
	 */
	template <typename Out, typename In, typename Func>
	Out Reduce(const TSet<In> Elements, const Out StartingValue, const Func Callable)
	{
		Out PreviousValue = StartingValue;

		for (const In& CurrentValue : Elements)
		{
			PreviousValue = Callable(PreviousValue, CurrentValue);
		}

		return PreviousValue;
	}

	/**
	 * Filters out null values from a set.
	 *
	 * The original set is not modified.
	 *
	 * @param Elements
	 *	The set of values to filter.
	 *
	 * @return
	 *	A new set containing all the values of the original set that were not null.
	*/
	template <typename In>
	TSet<In> Filter(const TSet<In> Elements)
	{
		return Filter<In>(
			Elements,
			[](const In Element)
			{
				return Element != nullptr;
			}
		);
	}

	/**
	 * Filters the values of a set using a predicate function.
	 *
	 * The predicate receives each value of the set and is expected to return true if a value should be included, or
	 * false if it should be omitted.
	 *
	 * The original set is not modified.
	 *
	 * @param Elements
	 *	The set of values to filter.
	 * @param Callable
	 *	The predicate function/lambda invoked for each element of the set. This function is expected to take in a
	 *	parameter of type "In" and return a boolean.
	 *
	 * @return
	 *	A new set containing all the values of the original set for which the callable returned "true".
	 */
	template <typename In, typename Func>
	TSet<In> Filter(const TSet<In> Elements, const Func Callable)
	{
		return Elements.FilterByPredicate(Callable);
	}
}