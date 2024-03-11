// OpenPF2 for UE Game Logic, Copyright 2023-2024, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2EffectCauseWrapper.h"

#include <Kismet/GameplayStatics.h>

#include <Net/UnrealNetwork.h>

APF2EffectCauseWrapper* APF2EffectCauseWrapper::Create(AActor* OwningActor, IPF2WeaponInterface* Weapon)
{
	UWorld* World = OwningActor->GetWorld();

	APF2EffectCauseWrapper* NewInstance =
		World->SpawnActorDeferred<APF2EffectCauseWrapper>(StaticClass(), FTransform(), OwningActor);

	NewInstance->FinalizeConstruction(Weapon);

	return NewInstance;
}

void APF2EffectCauseWrapper::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APF2EffectCauseWrapper, Weapon);
}

void APF2EffectCauseWrapper::FinalizeConstruction(IPF2WeaponInterface* const InWeapon)
{
	this->Weapon = InWeapon->ToDataAsset();

	UGameplayStatics::FinishSpawningActor(this, FTransform());
}
