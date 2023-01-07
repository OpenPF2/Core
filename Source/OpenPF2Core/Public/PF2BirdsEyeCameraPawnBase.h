// OpenPF2 for UE Game Logic, Copyright 2022-2023, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Portions of this code were adapted from or inspired by the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick
// Pruehs, provided under the MIT License. Copyright (c) 2017 Nick Pruehs.

#pragma once

#include <Components/SceneComponent.h>
#include <Camera/CameraComponent.h>

#include <GameFramework/Pawn.h>

#include "PF2BirdsEyeCameraPawnBase.generated.h"

/**
 * Base class for invisible pawns that are possessed while in a top-down, tactical view for encounters.
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2BirdsEyeCameraPawnBase : public APawn
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	explicit APF2BirdsEyeCameraPawnBase();

	// =================================================================================================================
	// Public Methods - AActor Overrides
	// =================================================================================================================
	virtual void Tick(float DeltaSeconds) override;

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
     * Focuses the camera on a specific actor.
     *
     * @param Actor
     *	The actor to focus on.
     */
	UFUNCTION(BlueprintCallable)
	void FocusCameraOnActor(AActor* Actor);

	/**
	 * Focuses the camera on showing all of the specified actors.
	 *
	 * @param Actors
	 *	The actors to focus on.
	 */
	UFUNCTION(BlueprintCallable)
	void FocusCameraOnActors(TArray<AActor*> Actors);

	/**
	 * Focuses the camera on a specific world location.
	 *
	 * @param NewCameraLocation
	 *	The location to focus on.
	 */
	UFUNCTION(BlueprintCallable)
	void FocusCameraOnLocation(FVector2D NewCameraLocation);

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The horizontal and vertical camera movement speed in cm/sec.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Control", meta = (ClampMin = 0))
	float CameraSpeed;

	/**
	 * The speed (in cm/sec) at which the camera zooms in and out.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Control", meta = (ClampMin = 0))
	float CameraZoomSpeed;

	/**
	 * How fast to tilt the camera while rotating, in percentage of input/sec, where (0 => 0%, and 1.0 => 100%).
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Control", meta = (ClampMin = 0.0f, ClampMax = 5.0f))
	float CameraTiltZoomSpeed;

	/**
	 * The minimum distance of the camera from the pawn, in cm.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Control", meta = (ClampMin = 0))
	float MinCameraDistance;

	/**
	 * The maximum distance of the camera from the pawn, in cm.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Camera Control", meta = (ClampMin = 0))
	float MaxCameraDistance;

	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The amount of horizontal (in screen space) camera movement to apply.
	 *
	 * This is the amount that was set by axis inputs in the previous frame. "Right" is a positive value, while "left"
	 * is a negative value.
	 */
	float CameraRightLeftAxisValue;

	/**
	 * The amount of vertical (in screen space) camera movement to apply.
	 *
	 * This is the amount that was set by axis inputs in the previous frame. "Up" is a positive value, while "down" is a
	 * negative value.
	 */
	float CameraUpDownAxisValue;

	/**
	 * The amount of zoom (in screen space) camera movement to apply.
	 *
	 * This is the amount that was set by axis inputs in the previous frame. "Out" is a positive value, while "in" is a
	 * negative value.
	 */
	float CameraZoomAxisValue;

	/**
	 * The amount of tilt-and-zoom camera movement to apply.
	 *
	 * This is the amount that was set by axis inputs in the previous frame. Tilting up and zooming out is a positive
	 * value, while tilting down and zooming in is a negative value.
	 */
	float CameraTiltZoomAxisValue;

	// =================================================================================================================
	// Protected Methods - APawn Overrides
	// =================================================================================================================
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the camera component inside this pawn that the player will control and use to see the world.
	 *
	 * @return
	 *	The camera component.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="OpenPF2|Bird's Eye Camera Pawns")
	USceneComponent* GetCameraComponent() const;

	/**
	 * Applies a horizontal axis input to camera movement.
	 *
	 * @param Value
	 *	The amount, as a float between -1.0 and 1.0, to move the camera during the next frame:
	 *	  -  1.0 represents 100% of camera movement speed to the right (in screen space).
	 *	  - -1.0 represents 100% of camera movement speed to the left (in screen space).
	 */
	void MoveCameraRightLeft(const float Value);

	/**
	 * Applies a vertical axis input to camera movement.
	 *
	 * @param Value
	*	The amount, as a float between -1.0 and 1.0, to move the camera during the next frame:
	 *	  -  1.0 represents 100% of camera movement speed up (in screen space).
	 *	  - -1.0 represents 100% of camera movement speed down (in screen space).
	 */
	void MoveCameraUpDown(const float Value);

	/**
	 * Applies a zoom input to camera movement.
	 *
	 * @param Value
	 *	The amount, as a float between -1.0 and 1.0, to move the camera in or out during the next frame:
	 *	  -  1.0 represents 100% of camera movement speed outwards (in screen space).
	 *	  - -1.0 represents 100% of camera movement speed inwards (in screen space).
	 */
	void ZoomCamera(const float Value);

	/**
	 * Applies a tilt zoom input to camera movement.
	 *
	 * @param Value
	 *	The amount, as a float between -1.0 and 1.0, to tilt and zoom the camera down-in or up-out during the next
	 *	frame:
	 *	  -  1.0 represents 100% of camera movement speed up and outwards (in screen space).
	 *	  - -1.0 represents 100% of camera movement speed down and inwards (in screen space).
	 */
	void TiltZoomCamera(const float Value);

	/**
	 * Gets the distance from the camera to an object in the level.
	 *
	 * @return
	 *	The distance from the camera to the object, in centimeters (cm).
	 */
	float GetCameraDistance() const;

	// =================================================================================================================
	// Blueprint Implementable Events
	// =================================================================================================================
	/**
	 * BP event invoked to apply a camera tilt-zoom input to the bird's eye camera.
	 *
	 * This is invoked during the tick for this pawn whenever there is a non-zero input for tilt zoom to apply.
	 *
	 * @param TiltAmount
	 *   The amount of tilt zoom to apply, as a float from -1.0 to 1.0. This amount has already been scaled by the tilt
	 *   zoom camera speed and should be added to the current tilt zoom amount.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		meta=(DisplayName="On Apply Tilt Zoom"),
		Category="OpenPF2|Bird's Eye Camera Pawns"
	)
	void BP_OnApplyTiltZoom(const float TiltAmount);
};
