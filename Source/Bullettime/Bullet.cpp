// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BullettimeCharacter.h"
// Sets default values
ABullet::ABullet()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Bullet");
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnBulletOverlap);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;	

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ABullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	// �浹 ���͸� ĳ�������� �� ���������� ĳ������ �ȴٸ� �ٸ� �÷��̾�� �浹���� ��츦 ���Ѵ�.
	ABullettimeCharacter* enemy = Cast<ABullettimeCharacter>(OtherActor);
	if (Owner == OtherActor)
	{
		// �浹�� ���Ͱ� �ڱ� �ڽ��̶�� ó������ ����
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("It's me"));
		return;
	}
	
	// �浹�� ���Ͱ� �ٸ� �÷��̾��� ��� �������� ����
	if (enemy != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnDamage"));
		enemy->OnDamage();
	}
	
	Destroy();
}
