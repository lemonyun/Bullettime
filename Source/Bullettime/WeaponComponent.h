// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class ABullettimeCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETTIME_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Bullet)
		TSubclassOf<class ABullet> BulletClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector MuzzleOffset;

	/** Sets default values for this component's properties */
	UWeaponComponent();

	///** Attaches the actor to a FirstPersonCharacter */
	//UFUNCTION(BlueprintCallable, Category = "Weapon")
	//	void AttachWeapon(ABullettimeCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Fire();

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
		virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


private:
	/** The Character holding this weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	ABullettimeCharacter* Character;
		
};
