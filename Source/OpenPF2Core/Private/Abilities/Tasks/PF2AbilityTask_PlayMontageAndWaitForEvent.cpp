// Copyright 2021-2023 Guy Elsmore-Paddock. All Rights Reserved.
// Adapted from content that is Copyright Epic Games, Inc. (Action RPG Sample).
// Licensed only for use with Unreal Engine.

#include "Abilities/Tasks/PF2AbilityTask_PlayMontageAndWaitForEvent.h"

#include <GameFramework/Character.h>
#include <AbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>
#include <AbilitySystemLog.h>

#include <Animation/AnimInstance.h>

UPF2AbilityTask_PlayMontageAndWaitForEvent* UPF2AbilityTask_PlayMontageAndWaitForEvent::CreatePlayMontageAndWaitForEvent(
	UGameplayAbility*           OwningAbility,
	const FName                 TaskInstanceName,
	UAnimMontage*               MontageToPlay,
	const FGameplayTagContainer EventTags,
	float                       Rate,
	const FName                 StartSection,
	const bool                  bStopWhenAbilityEnds,
	const float                 AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UPF2AbilityTask_PlayMontageAndWaitForEvent* Task =
		NewAbilityTask<UPF2AbilityTask_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);

	// Sadly, we can't do this through the constructor because this is a UObject that UE handles construction on. So, we
	// have to settle for setting the fields after construction.
	Task->MontageToPlay                  = MontageToPlay;
	Task->EventTags                      = EventTags;
	Task->Rate                           = Rate;
	Task->StartSection                   = StartSection;
	Task->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	Task->bStopWhenAbilityEnds           = bStopWhenAbilityEnds;

	return Task;
}

UPF2AbilityTask_PlayMontageAndWaitForEvent::UPF2AbilityTask_PlayMontageAndWaitForEvent(
	const FObjectInitializer& ObjectInitializer) :
		Super(ObjectInitializer),
		MontageToPlay(nullptr),
		Rate(1.0f),
		AnimRootMotionTranslationScale(0),
		bStopWhenAbilityEnds(true)
{
}

void UPF2AbilityTask_PlayMontageAndWaitForEvent::ExternalCancel()
{
	check(this->AbilitySystemComponent.IsValid());

	this->Native_OnAbilityCancelled();

	Super::ExternalCancel();
}

FString UPF2AbilityTask_PlayMontageAndWaitForEvent::GetDebugString() const
{
	const UAnimMontage* PlayingMontage = nullptr;

	if (this->HasAbility())
	{
		const FGameplayAbilityActorInfo* ActorInfo    = Ability->GetCurrentActorInfo();
		const UAnimInstance*             AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			if (AnimInstance->Montage_IsActive(this->MontageToPlay))
			{
				PlayingMontage = this->MontageToPlay;
			}
			else
			{
				PlayingMontage = AnimInstance->GetCurrentActiveMontage();
			}
		}
	}

	FString DebugString =
		FString::Printf(
			TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"),
			*GetNameSafe(this->MontageToPlay),
			*GetNameSafe(PlayingMontage)
		);

	return DebugString;
}

void UPF2AbilityTask_PlayMontageAndWaitForEvent::Activate()
{
	if (this->Ability == nullptr)
	{
		return;
	}

	bool                     bPlayedMontage = false;
	UAbilitySystemComponent* Asc            = this->GetTargetAsc();

	if (Asc != nullptr)
	{
		const FGameplayAbilityActorInfo* ActorInfo    = this->Ability->GetCurrentActorInfo();
		UAnimInstance*                   AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			// Ask the ASC to notify us if a Gameplay Event with the given tag(s) is received.
			this->EventHandle = Asc->AddGameplayEventTagContainerDelegate(
				this->EventTags,
				FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
					this,
					&UPF2AbilityTask_PlayMontageAndWaitForEvent::Native_OnGameplayEvent
				)
			);

			const float MontageDuration =
				Asc->PlayMontage(
					this->Ability,
					this->Ability->GetCurrentActivationInfo(),
					this->MontageToPlay,
					this->Rate,
					this->StartSection
				);

			if (MontageDuration > 0.0f)
			{
				// Playing a montage could potentially fire off a callback into game code which could kill this ability!
				// Early out if we are pending kill.
				if (this->ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				this->CancelledHandle =
					this->Ability->OnGameplayAbilityCancelled.AddUObject(
						this,
						&UPF2AbilityTask_PlayMontageAndWaitForEvent::Native_OnAbilityCancelled
					);

				this->BlendingOutDelegate.BindUObject(
					this,
					&UPF2AbilityTask_PlayMontageAndWaitForEvent::Native_OnMontageBlendingOut
				);

				AnimInstance->Montage_SetBlendingOutDelegate(this->BlendingOutDelegate, this->MontageToPlay);

				this->MontageEndedDelegate.BindUObject(
					this,
					&UPF2AbilityTask_PlayMontageAndWaitForEvent::Native_OnMontageEnded
				);

				AnimInstance->Montage_SetEndDelegate(this->MontageEndedDelegate, this->MontageToPlay);

				ACharacter*    Character = Cast<ACharacter>(this->GetAvatarActor());
				const ENetRole LocalRole = Character->GetLocalRole();

				if ((Character != nullptr) &&
					(LocalRole == ROLE_Authority ||
					 (LocalRole == ROLE_AutonomousProxy &&
					  this->Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(this->AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			ABILITY_LOG(Warning, TEXT("UPF2AbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
		}
	}
	else
	{
		ABILITY_LOG(
			Warning,
			TEXT("UPF2AbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent.")
		);
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(
			Warning,
			TEXT("UPF2AbilityTask_PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."),
			*(GetNameSafe(this->Ability)),
			*(GetNameSafe(this->MontageToPlay)),
			*this->InstanceName.ToString()
		);

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			this->OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	this->SetWaitingOnAvatar();
}

void UPF2AbilityTask_PlayMontageAndWaitForEvent::OnDestroy(const bool bAbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next
	// montage plays. (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (this->HasAbility())
	{
		this->Ability->OnGameplayAbilityCancelled.Remove(this->CancelledHandle);

		if (bAbilityEnded && this->bStopWhenAbilityEnds)
		{
			// ReSharper disable once CppExpressionWithoutSideEffects
			this->StopPlayingMontage();
		}
	}

	UAbilitySystemComponent* Asc = this->GetTargetAsc();

	if (Asc != nullptr)
	{
		Asc->RemoveGameplayEventTagContainerDelegate(this->EventTags, this->EventHandle);
	}

	Super::OnDestroy(bAbilityEnded);
}

bool UPF2AbilityTask_PlayMontageAndWaitForEvent::StopPlayingMontage() const
{
	const FGameplayAbilityActorInfo* ActorInfo = this->Ability->GetCurrentActorInfo();

	if (ActorInfo == nullptr)
	{
		return false;
	}

	const UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

	if (AnimInstance == nullptr)
	{
		return false;
	}

	UAbilitySystemComponent* Asc = this->GetTargetAsc();

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if ((Asc != nullptr) && this->HasAbility() && (Asc->GetAnimatingAbility() == this->Ability) &&
		(Asc->GetCurrentMontage() == this->MontageToPlay))
	{
		// Unbind delegates so they don't get called as well
		FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(this->MontageToPlay);

		if (MontageInstance != nullptr)
		{
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
			MontageInstance->OnMontageEnded.Unbind();
		}

		Asc->CurrentMontageStop();

		return true;
	}

	return false;
}

void UPF2AbilityTask_PlayMontageAndWaitForEvent::Native_OnGameplayEvent(const FGameplayTag        EventTag,
                                                                        const FGameplayEventData* Payload) const
{
	if (this->ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;

		TempData.EventTag = EventTag;

		this->OnEventReceived.Broadcast(EventTag, TempData);
	}
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UPF2AbilityTask_PlayMontageAndWaitForEvent::Native_OnMontageEnded(UAnimMontage* Montage, const bool bInterrupted)
{
	if (!bInterrupted && (Montage == this->MontageToPlay) && this->ShouldBroadcastAbilityTaskDelegates())
	{
		this->OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
	}

	this->EndTask();
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UPF2AbilityTask_PlayMontageAndWaitForEvent::Native_OnMontageBlendingOut(UAnimMontage* Montage,
                                                                             const bool    bInterrupted) const
{
	if (this->HasAbility() &&
		(this->Ability->GetCurrentMontage() == this->MontageToPlay) &&
		(Montage == this->MontageToPlay))
	{
		UAbilitySystemComponent* Asc = this->GetTargetAsc();

		if (Asc != nullptr)
		{
			Asc->ClearAnimatingAbility(this->Ability);
		}

		// Reset AnimRootMotionTranslationScale
		ACharacter*    Character = Cast<ACharacter>(this->GetAvatarActor());
		const ENetRole LocalRole = Character->GetLocalRole();

		if ((Character != nullptr) &&
			(LocalRole == ROLE_Authority ||
			 (LocalRole == ROLE_AutonomousProxy &&
			  this->Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
		{
			Character->SetAnimRootMotionTranslationScale(1.0f);
		}
	}

	if (bInterrupted)
	{
		if (this->ShouldBroadcastAbilityTaskDelegates())
		{
			this->OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else if (this->ShouldBroadcastAbilityTaskDelegates())
	{
		this->OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UPF2AbilityTask_PlayMontageAndWaitForEvent::Native_OnAbilityCancelled() const
{
	// Let the BP handle the interrupt as well
	if (this->StopPlayingMontage() && this->ShouldBroadcastAbilityTaskDelegates())
	{
		this->OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}
