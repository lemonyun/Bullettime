// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BullettimeCharacter.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"



// Sets default values
ABullet::ABullet()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Bullet");
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	if (GetLocalRole() == ROLE_Authority)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &ABullet::OnBulletImpact);
	}
	
	bReplicates = true;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;
}

void ABullet::OnBulletImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	// 충돌 액터를 캐스팅했을 때 정상적으로 캐스팅이 된다면 다른 플레이어와 충돌헀을 경우를 뜻한다.
	ABullettimeCharacter* enemy = Cast<ABullettimeCharacter>(OtherActor);
	if (Owner == OtherActor)
	{
		// 충돌한 액터가 자기 자신이라면 처리하지 않음
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("It's me"));
		return;
	}
	
	// 충돌한 액터가 다른 플레이어인 경우 데미지를 가함
	if (enemy != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnDamage"));
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, Owner->GetInstigatorController(), this, DamageType);
	}
	
	Destroy();
}

void ABullet::Destroyed()
{
	FVector spawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}
