// Adapted from "Runtime/Engine/Private/Components/PrimitiveComponent.cpp" (Unreal Engine 5.1), which is Copyright
// Epic Games, Inc. Licensed only for use with Unreal Engine.

#pragma once

#include <Components/PrimitiveComponent.h>

#include <Engine/OverlapInfo.h>

#include <GameFramework/WorldSettings.h>

#include <Internationalization/Text.h>

// ReSharper disable once IdentifierTypo
#define LOCTEXT_NAMESPACE "PrimitiveComponent"

// =====================================================================================================================
// Globals Variables
// =====================================================================================================================
namespace PrimitiveComponentStatics
{
	extern const FText MobilityWarnText;
}

// =====================================================================================================================
// Console Variables (CVars)
// =====================================================================================================================
// Each of these mirrors the CVars declared in PrimitiveComponent.h that are required for the functions we've copied or
// modified from UPrimitiveComponent. UE does not export the raw values for linking, so have to look up the CVars at
// run time. These are in a Pf2-specific namespace to avoid definition clashing with what's been defined in
// PrimitiveComponent.h.
namespace PF2PrimitiveComponentCVars
{
	bool IsFastOverlapCheckEnabled();
	bool AreCachedOverlapsAllowed();
	float GetInitialOverlapTolerance();
	float GetHitDistanceTolerance();

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	bool ShouldShowInitialOverlaps();
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
}

// =====================================================================================================================
// Template Methods
// =====================================================================================================================
/**
 * Searches for an specific overlap in an array of overlaps.
 *
 * (This was copied from PrimitiveComponent.cpp, since it was a private definition).
 *
 * @tparam AllocatorType
 *	The type of the allocator used by the overlaps array.
 *
 * @param OverlapArray
 *	The array of overlaps to search.
 * @param SearchItem
 *	The overlap to search for in the array of overlaps.
 *
 * @return
 *	The index of the target overlap in the array of overlaps, or INDEX_NONE if no match was found.
 */
template <class AllocatorType>
FORCEINLINE_DEBUGGABLE int32 IndexOfOverlapFast(const TArray<FOverlapInfo, AllocatorType>& OverlapArray,
                                                const FOverlapInfo&                        SearchItem)
{
	return OverlapArray.IndexOfByPredicate(FFastOverlapInfoCompare(SearchItem));
}

/**
 * Adds an overlap to an array of overlaps, if it does not already exist.
 *
 * (This was copied from PrimitiveComponent.cpp, since it was a private definition).
 *
 * @tparam AllocatorType
 *	The type of the allocator used by the overlaps array.
 *
 * @param OverlapArray
 *	The array of overlaps into which the overlap should be added.
 * @param NewOverlap
 *	The overlap to add to the array.
 */
template <class AllocatorType>
FORCEINLINE_DEBUGGABLE void AddUniqueOverlapFast(TArray<FOverlapInfo, AllocatorType>& OverlapArray,
                                                 FOverlapInfo&&                       NewOverlap)
{
	if (IndexOfOverlapFast(OverlapArray, NewOverlap) == INDEX_NONE)
	{
		OverlapArray.Add(NewOverlap);
	}
}

// =====================================================================================================================
// Inlined Utility Methods
// =====================================================================================================================
/**
 * Determines if overlap between two actors and their associated components should be ignored.
 *
 * (This was copied from PrimitiveComponent.cpp, since it was a private definition).
 *
 * Overlap should be ignored in any of the following situations:
 *  - A component overlapped with itself (self-overlap)
 *  - The 'bCheckOverlapFlags' parameter is true and both components return false for GetGenerateOverlapEvents().
 *  - Either actor pointer is null.
 *  - The 'World' pointer is null or the 'OtherActor' is the world settings or the 'OtherActor' is not initialized
 *
 * @param World
 *	The world in which the overlap occurred.
 * @param ThisActor
 *	The actor that owns 'ThisComponent'.
 * @param ThisComponent
 *	The first component that was involved in the overlap.
 * @param OtherActor
 *	The actor that owns 'OtherComponent'.
 * @param OtherComponent
 *	The second component that was involved in the overlap.
 * @param bCheckOverlapFlags
 *	Indicates if GetGenerateOverlapEvents() should be checked for both components.
 *
 * @return
 *	- true to ignore the overlap.
 *	- false not to ignore the overlap.
 */
FORCEINLINE_DEBUGGABLE bool ShouldIgnoreOverlapResult(const UWorld*              World,
                                                      const AActor*              ThisActor,
                                                      const UPrimitiveComponent& ThisComponent,
                                                      const AActor*              OtherActor,
                                                      const UPrimitiveComponent& OtherComponent,
                                                      const bool                 bCheckOverlapFlags)
{
	// Don't overlap with self
	if (&ThisComponent == &OtherComponent)
	{
		return true;
	}

	if (bCheckOverlapFlags)
	{
		// Both components must set GetGenerateOverlapEvents()
		if (!ThisComponent.GetGenerateOverlapEvents() || !OtherComponent.GetGenerateOverlapEvents())
		{
			return true;
		}
	}

	if ((ThisActor == nullptr) || (OtherActor == nullptr))
	{
		return true;
	}

	if ((World == nullptr) || (OtherActor == World->GetWorldSettings()) || !OtherActor->IsActorInitialized())
	{
		return true;
	}

	return false;
}

// =====================================================================================================================
// Global Utility Methods
// =====================================================================================================================
/**
 * "Pulls back" the time of impact along a hit trace to prevent precision problems with adjacent geometry.
 *
 * (This was copied from PrimitiveComponent.cpp, since it was a private definition).
 *
 * @param Hit
 *	A reference to the hit to pull back.
 * @param Dist
 *	The distance to pull back.
 */
extern void PullBackHit(FHitResult& Hit, const float Dist);

/**
 * Determines if the given hit result should be ignored.
 *
 * Non-blocking hit results are never ignored. Blocking hit results are ignored in the following situations:
 *	- The 'MoveFlags' parameter includes the 'MOVECOMP_IgnoreBases' flag, and either the moving actor is based
 *	  on/attached to the hit actor or vice-versa.
 *	- The 'MoveFlags' parameter does not include the 'MOVECOMP_NeverIgnoreBlockingOverlaps' flag, and:
 *	  - The penetration distance is less than the hit distance tolerance CVar; OR
 *	  - The direction of penetration is out of penetration rather than towards penetration (to avoid getting stuck in
 *	    walls).
 *
 * (This was copied from PrimitiveComponent.cpp, since it was a private definition).
 *
 * @param InWorld
 *	The world in which the hit result occurred.
 * @param TestHit
 *	The hit result to check.
 * @param MovementDirDenormalized
 *	The movement direction of the object that hit.
 * @param MovingActor
 *	The actor that is moving and hit something.
 * @param MoveFlags
 *	Flags controlling how hits should be interpreted.
 *
 * @return
 *	- true if the hit result should be ignored.
 *	- false if the hit result should not be ignored.
 */
extern bool ShouldIgnoreHitResult(const UWorld*       InWorld,
                                  FHitResult const&   TestHit,
                                  FVector const&      MovementDirDenormalized,
                                  const AActor*       MovingActor,
                                  EMoveComponentFlags MoveFlags);

#undef LOCTEXT_NAMESPACE
