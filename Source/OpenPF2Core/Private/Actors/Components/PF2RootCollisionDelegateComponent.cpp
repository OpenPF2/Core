// Adapted from "Runtime/Engine/Private/Components/PrimitiveComponent.cpp" (Unreal Engine 5.1), which is Copyright
// Epic Games, Inc. Licensed only for use with Unreal Engine.

#include "Actors/Components/PF2RootCollisionDelegateComponent.h"

#include <Algo/Copy.h>

#include <Components/SkeletalMeshComponent.h>

#include <GameFramework/CheatManager.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerController.h>

#include "UEPrimitiveComponentDefs.h"

// ReSharper disable once IdentifierTypo
#define LOCTEXT_NAMESPACE "PF2CollisionDelegateComponent"

bool UPF2RootCollisionDelegateComponent::MoveComponentImpl(const FVector&      Delta,
                                                           const FQuat&        NewRotationQuat,
                                                           bool                bSweep,
                                                           FHitResult*         OutHit,
                                                           EMoveComponentFlags MoveFlags,
                                                           ETeleportType       Teleport)

{
	// --- Start Difference from UPrimitiveComponent
	UPrimitiveComponent* Collision = this->CollisionComponent;

	if (Collision == nullptr)
	{
		// We have no other component on which to perform a collision check, so perform collision checks on this
		// component instead.
		return Super::MoveComponentImpl(Delta, NewRotationQuat, bSweep, OutHit, MoveFlags, Teleport);
	}
	// --- End Difference from UPrimitiveComponent

	// static things can move before they are registered (e.g. immediately after streaming), but not after.
	if (!IsValid(this) || this->CheckStaticMobilityAndWarn(PrimitiveComponentStatics::MobilityWarnText))
	{
		if (OutHit != nullptr)
		{
			OutHit->Init();
		}

		return false;
	}

	this->ConditionalUpdateComponentToWorld();

	// --- Start Difference from UPrimitiveComponent
	Collision->ConditionalUpdateComponentToWorld();
	// --- End Difference from UPrimitiveComponent

	// Set up
	const FVector TraceStart = this->GetComponentLocation();
	const FVector TraceEnd   = TraceStart + Delta;

	// --- Start Difference from UPrimitiveComponent
	const FVector CollisionTraceStart  = Collision->GetComponentLocation();
	const FVector CollisionTraceEnd    = CollisionTraceStart + Delta;

	const FQuat InitialRotationQuat = Collision->GetComponentTransform().GetRotation();

	// Recalc here to account for precision loss of float addition
	float CollisionDeltaSizeSq = (CollisionTraceEnd - CollisionTraceStart).SizeSquared();
	// --- End Difference from UPrimitiveComponent

	// ComponentSweepMulti does nothing if moving < KINDA_SMALL_NUMBER in distance, so it's important to not try to
	// sweep distances smaller than that.
	const float MinMovementDistSq = (bSweep ? FMath::Square(4.0f * KINDA_SMALL_NUMBER) : 0.0f);

	if (CollisionDeltaSizeSq <= MinMovementDistSq)
	{
		// Skip if no vector or rotation.
		if (NewRotationQuat.Equals(InitialRotationQuat, SCENECOMPONENT_QUAT_TOLERANCE))
		{
			// copy to optional output param
			if (OutHit != nullptr)
			{
				// --- Start Difference from UPrimitiveComponent
				OutHit->Init(CollisionTraceStart, CollisionTraceEnd);
				// --- End Difference from UPrimitiveComponent
			}

			return true;
		}

		CollisionDeltaSizeSq = 0.0f;
	}

	const bool bSkipPhysicsMove = ((MoveFlags & MOVECOMP_SkipPhysicsMove) != MOVECOMP_NoFlags);

	// WARNING: HitResult is only partially initialized in some paths. All data is valid only if bFilledHitResult is
	// true.
	FHitResult              BlockingHit(NoInit);
	bool                    bFilledHitResult       = false;
	bool                    bMoved                 = false;
	bool                    bIncludesOverlapsAtEnd = false;
	bool                    bRotationOnly          = false;
	AActor* const           Actor                  = this->GetOwner();
	TInlineOverlapInfoArray PendingOverlaps;

	BlockingHit.bBlockingHit = false;
	BlockingHit.Time         = 1.f;

	if (bSweep)
	{
		FVector NewLocation = TraceStart;

		// Perform movement collision checking if needed for this actor.
		const bool    bCollisionEnabled = this->IsQueryCollisionEnabled();
		UWorld* const MyWorld           = this->GetWorld();

		if ((MyWorld != nullptr) && bCollisionEnabled && (CollisionDeltaSizeSq > 0.0f))
		{
			TArray<FHitResult> Hits;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			if (!this->IsRegistered() && !MyWorld->bIsTearingDown)
			{
				if (Actor == nullptr)
				{
					//-V523
					ensureMsgf(
						this->IsRegistered(),
						TEXT("Non-actor MovedComponent %s not registered during sweep"),
						*this->GetFullName()
					);
				}
				else
				{
					ensureMsgf(
						this->IsRegistered(),
						TEXT("%s MovedComponent %s not registered during sweep (IsValid %d)"),
						*Actor->GetName(),
						*this->GetName(),
						IsValid(Actor)
					);
				}
			}
#endif

			static const FName       TraceTagName         = TEXT("MoveComponent");
			const bool               bForceGatherOverlaps = !this->ShouldCheckOverlapFlagToQueueOverlaps(*this);
			FComponentQueryParams    Params(SCENE_QUERY_STAT(MoveComponent), Actor);
			FCollisionResponseParams ResponseParam;

			this->InitSweepCollisionParams(Params, ResponseParam);

			Params.bIgnoreTouches |= !(this->GetGenerateOverlapEvents() || bForceGatherOverlaps);
			Params.TraceTag        = TraceTagName;

			// --- Start Difference from UPrimitiveComponent
			bool const bHadBlockingHit =
				MyWorld->ComponentSweepMulti(
					Hits,
					Collision,
					CollisionTraceStart,
					CollisionTraceEnd,
					InitialRotationQuat,
					Params
				);
			// --- End Difference from UPrimitiveComponent

			if (Hits.Num() > 0)
			{
				const float DeltaSize = FMath::Sqrt(CollisionDeltaSizeSq);

				for (int32 HitIdx = 0; HitIdx < Hits.Num(); HitIdx++)
				{
					// --- Start Difference from UPrimitiveComponent
					PullBackHit(Hits[HitIdx], DeltaSize);
					// --- End Difference from UPrimitiveComponent
				}
			}

			// If we had a valid blocking hit, store it.
			// If we are looking for overlaps, store those as well.
			int32 FirstNonInitialOverlapIdx = INDEX_NONE;

			if (bHadBlockingHit || (this->GetGenerateOverlapEvents() || bForceGatherOverlaps))
			{
				int32 BlockingHitIndex          = INDEX_NONE;
				float BlockingHitNormalDotDelta = UE_BIG_NUMBER;

				for (int32 HitIdx = 0; HitIdx < Hits.Num(); HitIdx++)
				{
					const FHitResult& TestHit = Hits[HitIdx];

					if (TestHit.bBlockingHit)
					{
						if (!ShouldIgnoreHitResult(MyWorld, TestHit, Delta, Actor, MoveFlags))
						{
							if (TestHit.bStartPenetrating)
							{
								// We may have multiple initial hits, and want to choose the one with the normal most
								// opposed to our movement.
								const float NormalDotDelta = (TestHit.ImpactNormal | Delta);

								if (NormalDotDelta < BlockingHitNormalDotDelta)
								{
									BlockingHitNormalDotDelta = NormalDotDelta;
									BlockingHitIndex          = HitIdx;
								}
							}
							else if (BlockingHitIndex == INDEX_NONE)
							{
								// First non-overlapping blocking hit should be used, if an overlapping hit was not.
								// This should be the only non-overlapping blocking hit, and last in the results.
								BlockingHitIndex = HitIdx;
								break;
							}
						}
					}
					else if (this->GetGenerateOverlapEvents() || bForceGatherOverlaps)
					{
						UPrimitiveComponent* OverlapComponent = TestHit.Component.Get();

						if ((OverlapComponent != nullptr) &&
						    (OverlapComponent->GetGenerateOverlapEvents() || bForceGatherOverlaps))
						{
							// --- Start Difference from UPrimitiveComponent
							bool bShouldIgnoreOverlapResult =
								ShouldIgnoreOverlapResult(
									MyWorld,
									Actor,
									*Collision,
									TestHit.HitObjectHandle.FetchActor(),
									*OverlapComponent,
									/*bCheckOverlapFlags=*/ !bForceGatherOverlaps
								);
							// --- End Difference from UPrimitiveComponent

							if (!bShouldIgnoreOverlapResult)
							{
								// don't process touch events after initial blocking hits
								if ((BlockingHitIndex >= 0) && (TestHit.Time > Hits[BlockingHitIndex].Time))
								{
									break;
								}

								if ((FirstNonInitialOverlapIdx == INDEX_NONE) && (TestHit.Time > 0.0f))
								{
									// We are about to add the first non-initial overlap.
									FirstNonInitialOverlapIdx = PendingOverlaps.Num();
								}

								// cache touches
								AddUniqueOverlapFast(PendingOverlaps, FOverlapInfo(TestHit));
							}
						}
					}
				}

				// Update blocking hit, if there was a valid one.
				if (BlockingHitIndex >= 0)
				{
					BlockingHit      = Hits[BlockingHitIndex];
					bFilledHitResult = true;
				}
			}

			// Update NewLocation based on the hit result
			if (BlockingHit.bBlockingHit)
			{
				check(bFilledHitResult);

				NewLocation = TraceStart + (BlockingHit.Time * (TraceEnd - TraceStart));

				// Sanity check
				const FVector ToNewLocation = (NewLocation - TraceStart);

				if (ToNewLocation.SizeSquared() <= MinMovementDistSq)
				{
					// We don't want really small movements to put us on or inside a surface.
					NewLocation      = TraceStart;
					BlockingHit.Time = 0.0f;

					// Remove any pending overlaps after this point, we are not going as far as we swept.
					if (FirstNonInitialOverlapIdx != INDEX_NONE)
					{
						constexpr bool bAllowShrinking = false;

						PendingOverlaps.SetNum(FirstNonInitialOverlapIdx, bAllowShrinking);
					}
				}
			}
			else
			{
				NewLocation = TraceEnd;
			}

			bIncludesOverlapsAtEnd =
				this->AreSymmetricRotations(InitialRotationQuat, NewRotationQuat, Collision->GetComponentScale());

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			if (UCheatManager::IsDebugCapsuleSweepPawnEnabled() && BlockingHit.bBlockingHit && !Collision->IsZeroExtent())
			{
				// When debugging, the sole purpose for this is to find how capsule trace information was when hit to
				// resolve stuck or improve our movement system - To turn this on, use DebugCapsuleSweepPawn.
				APawn const* const ActorPawn = (Actor ? Cast<APawn>(Actor) : nullptr);

				if ((ActorPawn != nullptr) && (ActorPawn->Controller != nullptr) &&
					ActorPawn->Controller->IsLocalPlayerController())
				{
					APlayerController const* const PC = CastChecked<APlayerController>(ActorPawn->Controller);

					if (PC->CheatManager)
					{
						FVector CylExtent =
							ActorPawn->GetSimpleCollisionCylinderExtent() * FVector(1.001f, 1.001f, 1.0f);

						FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CylExtent);

						// --- Start Difference from UPrimitiveComponent
						PC->CheatManager->AddCapsuleSweepDebugInfo(
							CollisionTraceStart,
							CollisionTraceEnd,
							BlockingHit.ImpactPoint,
							BlockingHit.Normal,
							BlockingHit.ImpactNormal,
							BlockingHit.Location,
							CapsuleShape.GetCapsuleHalfHeight(),
							CapsuleShape.GetCapsuleRadius(),
							true,
							(BlockingHit.bStartPenetrating && BlockingHit.bBlockingHit) ? true : false
						);
						// --- End Difference from UPrimitiveComponent
					}
				}
			}
#endif
		}
		else if (CollisionDeltaSizeSq > 0.0f)
		{
			// apply move delta even if components has collisions disabled
			NewLocation           += Delta;
			bIncludesOverlapsAtEnd = false;
		}
		else if ((CollisionDeltaSizeSq == 0.0f) && bCollisionEnabled)
		{
			bIncludesOverlapsAtEnd =
				this->AreSymmetricRotations(InitialRotationQuat, NewRotationQuat, Collision->GetComponentScale());

			bRotationOnly = true;
		}

		// Update the location.  This will teleport any child components as well (not sweep).
		bMoved = this->InternalSetWorldLocationAndRotation(NewLocation, NewRotationQuat, bSkipPhysicsMove, Teleport);
	}
	else
	{
		// not sweeping, just go directly to the new transform
		bMoved = this->InternalSetWorldLocationAndRotation(TraceEnd, NewRotationQuat, bSkipPhysicsMove, Teleport);

		bRotationOnly = (CollisionDeltaSizeSq == 0);

		bIncludesOverlapsAtEnd =
			bRotationOnly &&
			(this->AreSymmetricRotations(InitialRotationQuat, NewRotationQuat, Collision->GetComponentScale())) &&
			Collision->IsQueryCollisionEnabled();
	}

	// Handle overlap notifications.
	if (bMoved)
	{
		if (this->IsDeferringMovementUpdates())
		{
			// Defer UpdateOverlaps until the scoped move ends.
			FScopedMovementUpdate* ScopedUpdate = this->GetCurrentScopedMovement();

			if (bRotationOnly && bIncludesOverlapsAtEnd)
			{
				ScopedUpdate->KeepCurrentOverlapsAfterRotation(bSweep);
			}
			else
			{
				ScopedUpdate->AppendOverlapsAfterMove(PendingOverlaps, bSweep, bIncludesOverlapsAtEnd);
			}
		}
		else
		{
			if (bIncludesOverlapsAtEnd)
			{
				TInlineOverlapInfoArray OverlapsAtEndLocation;
				bool                    bHasEndOverlaps       = false;

				if (bRotationOnly)
				{
					bHasEndOverlaps =
						this->ConvertRotationOverlapsToCurrentOverlaps(
							OverlapsAtEndLocation,
							this->OverlappingComponents
						);
				}
				else
				{
					bHasEndOverlaps =
						this->ConvertSweptOverlapsToCurrentOverlaps(
							OverlapsAtEndLocation,
							PendingOverlaps,
							0,
							this->GetComponentLocation(),
							this->GetComponentQuat()
						);
				}

				TOverlapArrayView PendingOverlapsView(PendingOverlaps);
				TOverlapArrayView OverlapsAtEndView(OverlapsAtEndLocation);

				this->UpdateOverlaps(&PendingOverlapsView, true, bHasEndOverlaps ? &OverlapsAtEndView : nullptr);
			}
			else
			{
				TOverlapArrayView PendingOverlapsView(PendingOverlaps);

				this->UpdateOverlaps(&PendingOverlapsView, true, nullptr);
			}
		}
	}

	// Handle blocking hit notifications. Avoid if pending kill (which could happen after overlaps).
	const bool bAllowHitDispatch =
		!BlockingHit.bStartPenetrating || !(MoveFlags & MOVECOMP_DisableBlockingOverlapDispatch);

	if (BlockingHit.bBlockingHit && bAllowHitDispatch && IsValid(this))
	{
		check(bFilledHitResult);

		if (this->IsDeferringMovementUpdates())
		{
			FScopedMovementUpdate* ScopedUpdate = this->GetCurrentScopedMovement();

			ScopedUpdate->AppendBlockingHitAfterMove(BlockingHit);
		}
		else
		{
			this->DispatchBlockingHit(*Actor, BlockingHit);
		}
	}

	// copy to optional output param
	if (OutHit != nullptr)
	{
		if (bFilledHitResult)
		{
			*OutHit = BlockingHit;
		}
		else
		{
			// --- Start Difference from UPrimitiveComponent
			OutHit->Init(CollisionTraceStart, CollisionTraceEnd);
			// --- End Difference from UPrimitiveComponent
		}
	}

	// Return whether we moved at all.
	return bMoved;
}

void UPF2RootCollisionDelegateComponent::SetCollisionComponent(UPrimitiveComponent* Component)
{
	this->CollisionComponent = Component;
}

template<typename AllocatorType>
bool UPF2RootCollisionDelegateComponent::ConvertSweptOverlapsToCurrentOverlaps(
	TArray<FOverlapInfo, AllocatorType>& OverlapsAtEndLocation,
	const TOverlapArrayView&             SweptOverlaps,
	int32                                SweptOverlapsIndex,
	const FVector&                       EndLocation,
	const FQuat&                         EndRotationQuat)
{
	checkSlow(SweptOverlapsIndex >= 0);

	bool       bResult              = false;
	const bool bForceGatherOverlaps = !ShouldCheckOverlapFlagToQueueOverlaps(*this);

	if ((this->GetGenerateOverlapEvents() || bForceGatherOverlaps) &&
		PF2PrimitiveComponentCVars::AreCachedOverlapsAllowed())
	{
		const AActor* Actor = this->GetOwner();

		if ((Actor != nullptr) && (Actor->GetRootComponent() == this))
		{
			// We know we are not overlapping any new components at the end location. Children are ignored here (see
			// note below).
			if (PF2PrimitiveComponentCVars::IsFastOverlapCheckEnabled())
			{
				// Check components we hit during the sweep, keep only those still overlapping
				const FCollisionQueryParams UnusedQueryParams(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				const int32                 NumSweptOverlaps = SweptOverlaps.Num();

				OverlapsAtEndLocation.Reserve(OverlapsAtEndLocation.Num() + NumSweptOverlaps);

				for (int32 Index = SweptOverlapsIndex; Index < NumSweptOverlaps; ++Index)
				{
					const FOverlapInfo&  OtherOverlap   = SweptOverlaps[Index];
					UPrimitiveComponent* OtherPrimitive = OtherOverlap.OverlapInfo.GetComponent();

					if ((OtherPrimitive != nullptr) &&
					    (OtherPrimitive->GetGenerateOverlapEvents() || bForceGatherOverlaps))
					{
						if (OtherPrimitive->bMultiBodyOverlap)
						{
							// Not handled yet. We could do it by checking every body explicitly and track each body
							// index in the overlap test, but this seems like a rare need.
							return false;
						}
						else if ((Cast<USkeletalMeshComponent>(OtherPrimitive) != nullptr) ||
						         (Cast<USkeletalMeshComponent>(this) != nullptr))
						{
							// SkeletalMeshComponent does not support this operation, and would return false in the test
							// when an actual query could return true.
							return false;
						}
						else if (OtherPrimitive->ComponentOverlapComponent(this, EndLocation, EndRotationQuat, UnusedQueryParams))
						{
							OverlapsAtEndLocation.Add(OtherOverlap);
						}
					}
				}

				// Note: we don't worry about adding any child components here, because they are not included in the sweep results.
				// Children test for their own overlaps after we update our own, and we ignore children in our own update.
				checkfSlow(
					OverlapsAtEndLocation.FindByPredicate(FPredicateOverlapHasSameActor(*Actor)) == nullptr,
					TEXT("Child overlaps should not be included in the SweptOverlaps() array in UPF2RootCollisionDelegateComponent::ConvertSweptOverlapsToCurrentOverlaps()."));

				bResult = true;
			}
			else
			{
				if (SweptOverlaps.Num() == 0 && this->AreAllCollideableDescendantsRelative())
				{
					// Add overlaps with components in this actor.
					this->GetOverlapsWithActor_Template(Actor, OverlapsAtEndLocation);

					bResult = true;
				}
			}
		}
	}

	return bResult;
}

template<typename AllocatorType>
bool UPF2RootCollisionDelegateComponent::ConvertRotationOverlapsToCurrentOverlaps(
	TArray<FOverlapInfo, AllocatorType>& OutOverlapsAtEndLocation,
	const TOverlapArrayView&             CurrentOverlaps)
{
	bool       bResult              = false;
	const bool bForceGatherOverlaps = !ShouldCheckOverlapFlagToQueueOverlaps(*this);

	if ((this->GetGenerateOverlapEvents() || bForceGatherOverlaps) &&
		PF2PrimitiveComponentCVars::AreCachedOverlapsAllowed())
	{
		const AActor* Actor = this->GetOwner();

		if ((Actor != nullptr) && (Actor->GetRootComponent() == this))
		{
			if (PF2PrimitiveComponentCVars::IsFastOverlapCheckEnabled())
			{
				// Add all current overlaps that are not children. Children test for their own overlaps after we update
				// our own, and we ignore children in our own update.
				OutOverlapsAtEndLocation.Reserve(OutOverlapsAtEndLocation.Num() + CurrentOverlaps.Num());
				Algo::CopyIf(CurrentOverlaps, OutOverlapsAtEndLocation, FPredicateOverlapHasDifferentActor(*Actor));

				bResult = true;
			}
		}
	}

	return bResult;
}

template<typename AllocatorType>
bool UPF2RootCollisionDelegateComponent::GetOverlapsWithActor_Template(
	const AActor*                        Actor,
	TArray<FOverlapInfo, AllocatorType>& OutOverlaps) const
{
	const int32 InitialCount = OutOverlaps.Num();

	if (Actor != nullptr)
	{
		for (int32 OverlapIdx = 0; OverlapIdx < this->OverlappingComponents.Num(); ++OverlapIdx)
		{
			UPrimitiveComponent const* const PrimComp =
				this->OverlappingComponents[OverlapIdx].OverlapInfo.Component.Get();

			if ((PrimComp != nullptr) && (PrimComp->GetOwner() == Actor))
			{
				OutOverlaps.Add(this->OverlappingComponents[OverlapIdx]);
			}
		}
	}

	return InitialCount != OutOverlaps.Num();
}

#undef LOCTEXT_NAMESPACE
