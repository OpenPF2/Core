// OpenPF2 for UE Game Logic, Copyright 2021-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

/**
 * Various utilities for functional programming with arrays and similar structures.
 */
namespace PF2ArrayUtilities
{
	/**
	 * Concatenates two or more arrays together into a new array.
	 *
	 * @tparam T
	 *	The type of elements in the array.
	 * @tparam TArgs
	 *	Variadic type for the remaining arrays.
	 *
	 * @param Array
	 *	The array containing the starting list of items to which the contents of remaining arrays will be concatenated.
	 * @param OtherArray
	 *	Each additional array that will be appended to the end of the result.
	 *
	 * @return
	 *	A new array containing all of the elements of the arrays in the order that they were passed to this method.
	 */
	template <typename T, typename... TArgs>
	TArray<T> Concatenate(const TArray<T>& Array, const TArgs&... OtherArray) {
		TArray<T> CombinedArray = Array;

		((CombinedArray += OtherArray), ...);

		return CombinedArray;
	}

	/**
	 * Add each unique element from one array to the end of the other.
	 *
	 * Elements that already exist in the target array are skipped.
	 *
	 * @tparam T
	 *	The type of elements in the array.
	 *
	 * @param NewElements
	 *	The array of elements to add to the target.
	 * @param Target
	 *	The array to which elements will be added.
	 */
	template <typename T>
	void AddAllUnique(TArray<T> NewElements, TArray<T>& Target)
	{
		TSet<T> TargetIndex = TSet<T>(Target);

		for (const auto& NewElement : NewElements)
		{
			// We could use Target.AddUnique() but for larger sets it's O(N^2) whereas building a set from the target
			// and then using the set as an index of what's in the array should cost only O(2N).
			if (!TargetIndex.Contains(NewElement))
			{
				Target.Add(Target);
			}
		}
	}

	/**
	 * Applies a transformation function to the values in an array, then returns the result of the transformation.
	 *
	 * The original array is not modified.
	 *
	 * @tparam In
	 *	The type of elements in the input array.
	 * @tparam Out
	 *	The type of elements in the output array. (The type into which input elements will be transformed).
	 * @tparam Func
	 *	The type of the lambda function to invoke on each element of the input array to return a new element to add to
	 *	the output array.
	 *
	 * @param Elements
	 *	The array of values to map.
	 * @param Callable
	 *	The transformation function/lambda invoked on each element in order to get the mapped value.
	 *
	 * @return
	 *	The array of values that resulted from applying the transformation to every value of the source array.
	 */
	template <typename Out, typename In, typename Func>
	TArray<Out> Map(const TArray<In> Elements, const Func Callable)
	{
		TArray<Out> Result;

		Result.Reserve(Elements.Num());

		for (const In& Element : Elements)
		{
			const Out MappingOutput = Callable(Element);

			Result.Add(MappingOutput);
		}

		return Result;
	}

	/**
	 * Collapses all of the values of an array to a single value, by use of a transformation function.
	 *
	 * The transformation function receives a pair of values -- the "previous value" and the "current value". The
	 * transformation function is invoked once for each value in the original array. For the first value of the array,
	 * the "previous value" is the starting value supplied to this function, and the "current value" is the first value
	 * of the input array. For each subsequent call, the "previous value" is the result that was returned by the
	 * previous call to the transformation function and the value is the subsequent value of the input array.
	 *
	 * The input array is not modified.
	 *
	 * @tparam In
	 *	The type of elements in the input array.
	 * @tparam Out
	 *	The type of result to return. (The type into which all input elements will be consolidated).
	 * @tparam Func
	 *	The type of the lambda function to invoke on each element of the input array, applying a transformation on the
	 *	element and the accumulated result to return a new output value.
	 *
	 * @param Elements
	 *	The array of values to reduce.
	 * @param StartingValue
	 *	The value passed in to the Callable along side the first value of the array. If the array is empty, this will be
	 *	the value returned.
	 * @param Callable
	 *	The transformation function/lambda invoked to combine each element with the result of flattening/reducing the
	 *	previous element. This function is expected to take in the following two parameters and return a value that
	 *	matches the "Out" type:
	 *	  - PreviousValue: Which must match the "Out" type.
	 *	  - CurrentValue: Which must match the "In" type.
	 *
	 * @return
	 *	The result of reducing the values of the array.
	 */
	template <typename Out, typename In, typename Func>
	Out Reduce(const TArray<In> Elements, const Out StartingValue, const Func Callable)
	{
		Out PreviousValue = StartingValue;

		for (const In& CurrentValue : Elements)
		{
			const Out& ConstPreviousValue = PreviousValue;

			PreviousValue = Callable(ConstPreviousValue, CurrentValue);
		}

		return PreviousValue;
	}

	/**
	 * Collapses all of the values of an array to a new array by use of a transformation function.
	 *
	 * This is similar to Reduce but is optimized around array results, in two key ways:
	 *   1. This method pre-allocates the result array, sizing it to have a slack size equal to the source array.
	 *   2. To avoid unnecessary copying, the result array is passed as a reference to each iteration and this array is
	 *      expected to be modified in place.
	 *
	 * The transformation function receives a pair of values -- the "result array" and the "current value". The
	 * transformation function is invoked once for each value in the original array. For the first value of the array,
	 * an empty array is supplied as the "result array", and the first value of the input array becomes the
	 * "current value". For each subsequent call, the "result array" is the array of results after the previous call to
	 * the transformation function and the value is the subsequent value of the input array.
	 *
	 * The input array is not modified.
	 *
	 * @tparam In
	 *	The type of elements in the input array.
	 * @tparam Out
	 *	The type of elements in the output array.
	 * @tparam Func
	 *	The type of the lambda function to invoke on the result array and each element of the input array, applying a
	 *	transformation on the element and then updating the result array as appropriate.
	 *
	 * @param Elements
	 *	The array of values to reduce.
	 * @param Callable
	 *	The transformation function/lambda invoked to combine each element with the result of flattening/reducing the
	 *	previous element. This function is expected to take in the following two parameters:
	 *	  - PreviousValues: Which is an array of items matching the "Out" type. This should be modified in place.
	 *	  - CurrentValue: Which must match the "In" type.
	 *
	 * @return
	 *	The result of reducing the values of the array to a new array.
	 */
	template <typename Out, typename In, typename Func>
	TArray<Out> ReduceToArray(const TArray<In> Elements, const Func Callable)
	{
		TArray<Out> ResultArray = TArray<Out>();

		ResultArray.Reserve(Elements.Num());

		for (const In& CurrentValue : Elements)
		{
			Callable(ResultArray, CurrentValue);
		}

		return ResultArray;
	}

	/**
	 * Filters out null values from an array.
	 *
	 * The original array is not modified.
	 *
	 * @tparam T
	 *	The type of elements in the array.
	 *
	 * @param Elements
	 *	The array of values to filter.
	 *
	 * @return
	 *	A new array containing all the values of the original array that were not null.
	 */
	template <typename T>
	TArray<T> Filter(const TArray<T> Elements)
	{
		return Filter<T>(
			Elements,
			[](const T Element)
			{
				return Element != nullptr;
			}
		);
	}

	/**
	 * Filters the values of an array using a predicate function.
	 *
	 * The predicate receives each value of the array and is expected to return true if a value should be included, or
	 * false if it should be omitted.
	 *
	 * The original array is not modified.
	 *
	 * @tparam T
	 *	The type of elements in the array.
	 *
	 * @param Elements
	 *	The array of values to filter.
	 * @param Callable
	 *	The predicate function/lambda invoked for each element of the array. This function is expected to take in a
	 *	parameter of type "In" and return a boolean.
	 *
	 * @return
	 *	A new array containing all the values of the original array for which the callable returned "true".
	 */
	template <typename T, typename Func>
	TArray<T> Filter(const TArray<T> Elements, const Func Callable)
	{
		return Elements.FilterByPredicate(Callable);
	}

	/**
	 * Typecast one array to an array of another type.
	 *
	 * This should only be used in cases when the types are compatible. For example, to downcast a generic type like
	 * TArray<Actor*> to TArray<IPF2CharacterInterface*> when it is know that an array of actors must all implement a
	 * particular interface.
	 *
	 * In development builds, the sizes of the types are checked for compatibility with assertions.
	 *
	 * Credit for this approach:
	 * https://forums.unrealengine.com/t/how-to-cast-tarray-to-another-type/293689/13
	 *
	 * @param InArray
	 *	The array to typecast.
	 *
	 * @return
	 *	The typecast array.
	 */
	template <typename In, typename Out>
	TArray<Out> Cast(TArray<In> InArray)
	{
		TArray<Out> Result;

		// Confirm compatible data types.
		check(sizeof(In) == sizeof(Out));

		Result = MoveTemp(*reinterpret_cast<TArray<Out>*>(&InArray));

		// Confirm all elements were moved.
		check(InArray.Num() == 0);

		return Result;
	}

	/**
	 * Identify what elements have been added or removed between two copies of an array.
	 *
	 * @tparam T
	 *	The type of elements in the arrays. Should not be a pointer type.
	 *
	 * @param OldArray
	 *	The old copy of the array.
	 * @param NewArray
	 *	The new copy of the array.
	 * @param EqualityCallback
	 *	A lambda/delegate taking in two elements and returning whether they are equivalent items. For efficiency, this
	 *	should compare only a single field in each struct that uniquely identifies it (e.g., a unique ID, a handle, or
	 *	a name), rather than comparing all fields of the two structs.
	 * @param OutRemovedElements
	 *	The array to which elements that were present in OldArray but are no longer present in NewArray will be added.
	 * @param OutAddedElements
	 *	The array to which elements that were not present in OldArray but are now present in NewArray will be added.
	 */
	template <typename T, typename Func>
	void CaptureStructDeltas(const TArray<T> OldArray,
	                         const TArray<T> NewArray,
	                         const Func      EqualityCallback,
	                         TArray<T>&      OutRemovedElements,
	                         TArray<T>&      OutAddedElements)
	{
		// Identify which elements were removed.
		for (T const OldElement : OldArray)
		{
			bool ElementRemoved = true;

			for (T const NewElement : NewArray)
			{
				if (EqualityCallback(OldElement, NewElement))
				{
					ElementRemoved = false;
					break;
				}
			}

			if (ElementRemoved)
			{
				OutRemovedElements.Add(OldElement);
			}
		}

		// Identify which elements were added.
		for (T const NewElement : NewArray)
		{
			bool ElementAdded = true;

			for (T const OldElement : OldArray)
			{
				if (EqualityCallback(OldElement, NewElement))
				{
					ElementAdded = false;
					break;
				}
			}

			if (ElementAdded)
			{
				OutAddedElements.Add(NewElement);
			}
		}
	}

	/**
	 * Identify what pointers have been added or removed between two copies of an array.
	 *
	 * @tparam T
	 *	The type of elements in the arrays. Should be a pointer type (e.g., AActor*).
	 *
	 * @param OldArray
	 *	The old copy of the array.
	 * @param NewArray
	 *	The new copy of the array.
	 * @param OutRemovedElements
	 *	The array to which elements that were present in OldArray but are no longer present in NewArray will be added.
	 * @param OutAddedElements
	 *	The array to which elements that were not present in OldArray but are now present in NewArray will be added.
	 */
	template <typename T>
	void CapturePtrDeltas(const TArray<T> OldArray,
	                      const TArray<T> NewArray,
	                      TArray<T>&      OutRemovedElements,
	                      TArray<T>&      OutAddedElements)
	{
		// Identify which elements were removed.
		for (T const Element : OldArray)
		{
			if ((Element != nullptr) && !NewArray.Contains(Element))
			{
				OutRemovedElements.Add(Element);
			}
		}

		// Identify which elements were added.
		for (T const Element : NewArray)
		{
			if ((Element != nullptr) && !OldArray.Contains(Element))
			{
				OutAddedElements.Add(Element);
			}
		}
	}

	/**
	 * Identify what pointers have been added or removed between two copies of an array, casting results in the process.
	 *
	 * Elements will be typecast from SrcT to ResultT. Only elements for which the typecast is successful will be added
	 * to OutRemovedElements and OutAddedElements; all other elements will be disregarded.
	 *
	 * @tparam SrcT
	 *	The type of elements in the old and new array, without the pointer specifier (this code needs the raw type, so
	 *	it already assumes the arrays use pointers).
	 * @tparam ResultT
	 *	The type of elements in the removed and added arrays, without the pointer specifier (this code needs the raw
	 *	type, so it already assumes the arrays use pointers).
	 *
	 * @param OldArray
	 *	The old copy of the array.
	 * @param NewArray
	 *	The new copy of the array.
	 * @param OutRemovedElements
	 *	The array to which elements that were present in OldArray but are no longer present in NewArray will be added.
	 * @param OutAddedElements
	 *	The array to which elements that were not present in OldArray but are now present in NewArray will be added.
	 */
	template <typename SrcT, typename ResultT>
	void CapturePtrDeltasWithCast(const TArray<SrcT*> OldArray,
	                              const TArray<SrcT*> NewArray,
	                              TArray<ResultT*>&   OutRemovedElements,
	                              TArray<ResultT*>&   OutAddedElements)
	{
		// Identify which elements were removed.
		for (SrcT* const Element : OldArray)
		{
			ResultT* CastElement = Cast<ResultT>(Element);

			if ((CastElement != nullptr) && !NewArray.Contains(Element))
			{
				OutRemovedElements.Add(CastElement);
			}
		}

		// Identify which elements were added.
		for (SrcT* const Element : NewArray)
		{
			ResultT* CastElement = Cast<ResultT>(Element);

			if ((CastElement != nullptr) && !OldArray.Contains(Element))
			{
				OutAddedElements.Add(CastElement);
			}
		}
	}
}
