// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Components/PrimitiveComponent.h>

#include "PF2RootCollisionDelegateComponent.generated.h"

/**
 * A primitive component that delegates its collision checks to one of its attached sub-components (another primitive).
 *
 * This is useful when the actual collision component needs to move relative to the root component of an actor. Without
 * this, the actor itself has to be moved in order to affect collision, since default engine movement components don't
 * perform collision checks on actors unless their root component is a collision primitive.
 *
 * @see UMovementComponent::SetUpdatedComponent
 */
UCLASS(
	ClassGroup="Collision",
	EditInlineNew,
	HideCategories=(Object, LOD, Lighting, TextureStreaming),
	meta=(BlueprintSpawnableComponent, DisplayName="Collision Delegate")
)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2RootCollisionDelegateComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

protected:
	/**
	 * The component against which collision checks will be performed.
	 *
	 * This component must be attached to this component, either as a child or grandchild, for movement to work in a
	 * sensible way, as this component will not perform any direct movement of the collision component. Said
	 * differently, movement components will move this root component, which will result in its child components moving
	 * automatically, so if the collision component is expected to move with this root component, it needs to be
	 * attached to this component.
	 */
	UPROPERTY(BlueprintReadOnly, Transient)
	UPrimitiveComponent* CollisionComponent;

public:
	// =================================================================================================================
	// Public Methods - UPrimitiveComponent Overrides
	// =================================================================================================================
	virtual bool MoveComponentImpl(const FVector&      Delta,
                                   const FQuat&        NewRotationQuat,
                                   bool                bSweep,
                                   FHitResult*         OutHit,
                                   EMoveComponentFlags MoveFlags,
                                   ETeleportType       Teleport) override;

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Sets the child component ("sub-component") of this component that will be used for collision checking.
	 *
	 * The component provided must be attached to this component, either as a child or grandchild, for movement to work
	 * in a sensible way, as this component will not perform any direct movement of the collision component. Said
	 * differently, movement components will move this root component, which will result in its child components moving
	 * automatically, so if the collision component is expected to move with this root component, it needs to be
	 * attached to this component.
	 *
	 * @param Component
	 *	The sub-component against which collision checking should be done.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Root Collision Delegates")
	virtual void SetCollisionComponent(UPrimitiveComponent* Component);

private:
	/**
	 * Converts a set of overlaps from a sweep to a subset that includes only those at the end location.
	 *
	 * (This was copied from the UPrimitiveComponent base class, since this method is private in that class).
	 *
	 * @see UPrimitiveComponent::ConvertSweptOverlapsToCurrentOverlaps
	 *
	 * @tparam AllocatorType
	 *	The type of the allocator used by the overlaps array.
	 *
	 * @param OutOverlapsAtEndLocation
	 *	A reference to an array that is modified to include the array of only overlaps at the end location of the sweep.
	 * @param SweptOverlaps
	 *	All of the overlaps of the sweep.
	 * @param SweptOverlapsIndex
	 *	The offset of the first overlap to evaluate out of those swept.
	 * @param EndLocation
	 *	The end location of the sweep.
	 * @param EndRotationQuat
	 *	The rotation of the component at the end of the sweep.
	 *
	 * @return
	 *	- true if it was possible to return only overlaps at the end location.
	 *	- false if it was not possible to return only overlaps at the end location.
	 */
	template<typename AllocatorType>
	bool ConvertSweptOverlapsToCurrentOverlaps(TArray<FOverlapInfo, AllocatorType>& OutOverlapsAtEndLocation,
	                                           const TOverlapArrayView&             SweptOverlaps,
	                                           int32                                SweptOverlapsIndex,
	                                           const FVector&                       EndLocation,
	                                           const FQuat&                         EndRotationQuat);

	/**
	 * Converts a set of overlaps from a symmetric change in rotation to a subset of only those at the end location.
	 *
	 * (This was copied from the UPrimitiveComponent base class, since this method is private in that class).
	 *
	 * @see UPrimitiveComponent::ConvertRotationOverlapsToCurrentOverlaps
	 *
	 * @tparam AllocatorType
	 *	The type of the allocator used by the overlaps array.
	 *
	 * @param OutOverlapsAtEndLocation
	 *	A reference to an array that is modified to include the array of only overlaps at the end location of the
	 *	rotation.
	 * @param CurrentOverlaps
	 *	The overlaps encountered during the full rotation.
	 *
	 * @return
	 *	- true if it was possible to return only overlaps at the end location.
	 *	- false if it was not possible to return only overlaps at the end location.
	 */
	template<typename AllocatorType>
	bool ConvertRotationOverlapsToCurrentOverlaps(TArray<FOverlapInfo, AllocatorType>& OutOverlapsAtEndLocation,
	                                              const TOverlapArrayView&             CurrentOverlaps);

	/**
	 * Identifies all the overlaps with components that are owned by the given actor.
	 *
	 * (This was copied from the UPrimitiveComponent base class, since this method is private in that class).
	 *
	 * @see UPrimitiveComponent::GetOverlapsWithActor_Template
	 *
	 * @tparam AllocatorType
	 *	The type of the allocator used by the overlaps array.
	 *
	 * @param Actor
	 *	The actor owning the components of interest.
	 * @param OutOverlaps
	 *	A reference to an array that is modified to include all overlaps with components owned by the given actor.
	 *
	 * @return
	 *	- true if there is at least one overlap with a component owned by the given actor.
	 *	- false if there were either no overlaps, or no overlaps with components of the given actor.
	 */
	template<typename AllocatorType>
	bool GetOverlapsWithActor_Template(const AActor* Actor, TArray<FOverlapInfo, AllocatorType>& OutOverlaps) const;
};
