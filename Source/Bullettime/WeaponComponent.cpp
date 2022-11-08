// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "BullettimeCharacter.h"
#include "Bullet.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UWeaponComponent::Fire()
{

	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	
	// Try and fire a projectile
	if (BulletClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = PlayerController->PlayerCameraManager->GetCameraLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			ActorSpawnParams.Owner = Cast<AActor>(Character);

			// Spawn the projectile at the muzzle
			ABullet *bullet = World->SpawnActor<ABullet>(BulletClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			
		}
	}

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation1P != nullptr)
	{
		UAnimInstance* AnimInstance1P = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance1P != nullptr)
		{
			AnimInstance1P->Montage_Play(FireAnimation1P, 1.f);

		}
	}
	if (FireAnimation3P != nullptr)
	{
		
		UAnimInstance* AnimInstance3P = Character->GetMesh()->GetAnimInstance();

		if (AnimInstance3P != nullptr)
		{
			
			float check = AnimInstance3P->Montage_Play(FireAnimation3P, 1.f);
		}
	}

}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character != nullptr)
	{
		// Unregister from the OnUseItem Event
		Character->OnUseItem.RemoveDynamic(this, &UWeaponComponent::Fire);
	}
}