// Copyright Epic Games, Inc. All Rights Reserved.

#include "BullettimeCharacter.h"
#include "WeaponComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "BullettimePlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "BullettimePlayerController.h"
#include "BullettimePlayerState.h"


//////////////////////////////////////////////////////////////////////////
// ABullettimeCharacter

ABullettimeCharacter::ABullettimeCharacter()
{

	GetMesh()->SetOwnerNoSee(true);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	RootComponent = GetCapsuleComponent();
	//GetCapsuleComponent()->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel2);
	// GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	// GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller	


	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent()); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	CameraComponent->bUsePawnControlRotation = true;
	// CameraComponent->SetRelativeTransform(FTransform(FVector(0.0f, -45.0f, 0.0f)));


	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(CameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P"));
	WeaponMesh1P->SetupAttachment(CameraComponent);
	WeaponMesh1P->SetOnlyOwnerSee(true);

	Weapon = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	//Weapon->SetupAttachment(WeaponMesh1P);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	//WeaponMesh1P->AttachToComponent(Mesh1P, AttachmentRules, FName(TEXT("GripPoint")));
	WeaponMesh1P->SetupAttachment(Mesh1P, FName(TEXT("GripPoint")));

	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	WeaponMesh3P->SetOwnerNoSee(true);
	//WeaponMesh3P->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("GripPoint")));
	WeaponMesh3P->SetupAttachment(GetMesh(), FName(TEXT("GripPoint")));
	
	MaxHealth = 100;
	CurHealth = MaxHealth;

	TransformTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TransformTimelineComp"));
	RotationTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimelineComp"));
	
		// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
		// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}



void ABullettimeCharacter::BeginPlay()
{
	Super::BeginPlay();
	Weapon->SetCharacter(this);
	OnUseItem.AddDynamic(Weapon, &UWeaponComponent::Server_OnFire);
	OnUseItem.AddDynamic(Weapon, &UWeaponComponent::Play1PFireMontage);

	CurHealth = MaxHealth;
	
	UpdateFunctionRotation.BindDynamic(this, &ABullettimeCharacter::UpdateRotationTimelineComp);
	UpdateFunctionTransform.BindDynamic(this, &ABullettimeCharacter::UpdateTransformTimelineComp);


	if (CameraTimelineTransformCurve)
	{
		TransformTimelineComp->AddInterpVector(CameraTimelineTransformCurve, UpdateFunctionTransform);
	}

	if (CameraTimelineRotationCurve)
	{
		RotationTimelineComp->AddInterpVector(CameraTimelineRotationCurve, UpdateFunctionRotation);
	}


	SpawnTransform = GetActorTransform();
}

void ABullettimeCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	Super::EndPlay(EndPlayReason); 
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABullettimeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ABullettimeCharacter::OnPrimaryAction);


	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ABullettimeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ABullettimeCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ABullettimeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ABullettimeCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABullettimeCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABullettimeCharacter::TouchStopped);
}

void ABullettimeCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ABullettimeCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ABullettimeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ABullettimeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ABullettimeCharacter::OnPrimaryAction()
{
	OnUseItem.Broadcast();
}

void ABullettimeCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABullettimeCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

float ABullettimeCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageTaken, DamageEvent, EventInstigator, DamageCauser);
	RecentDamageCauser = EventInstigator;
	float damageApplied = CurHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	return damageApplied;
}

void ABullettimeCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}


void ABullettimeCharacter::OnHealthUpdate() {
	
	//데미지를 입은 캐릭터가 클라이언트의 플레이어 컨트롤러가 가지는 캐릭터인 경우 
	// 혹은 리슨서버의 플레이어 컨트롤러가 가지는 캐릭터인 경우
	if (IsLocallyControlled() || (GetLocalRole() == ROLE_Authority && GetRemoteRole() == ROLE_SimulatedProxy))
	{
		
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		GetController<ABullettimePlayerController>()->PlayerHUD->SetHealth(CurHealth, MaxHealth);

		if (CurHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);

			FTimerHandle WaitHandle;
			float WaitTime = 4.0f;
			GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
				{
					// 로컬 플레이어컨트롤러의 HealthBar를 4초 후에 초기화한다.
					GetController<ABullettimePlayerController>()->PlayerHUD->SetHealth(MaxHealth, MaxHealth);

				}), WaitTime, false); 
			
			

			// 카메라가 죽은 캐릭터를 천천히 비추는 연출
			CameraWork();

			// 1인칭 메시를 보이지 않게 하고 3인칭 메시는 보이도록 만들기
			ChangeThirdPerson();

			
		}

	}

	//데미지를 입은 캐릭터가 서버
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		//GetController<ABullettimePlayerController>()->PlayerHUD->SetHealth(CurHealth, MaxHealth);

		if (CurHealth <= 0.f)
		{
			// 플레이어 죽음 처리

			// 서버만 
			// 
			// 점수 누적
			AddScore();
			// 리스폰 타이머 세팅
			SetRespawnTimer();

			// 서버 - 클라이언트 모두
			Multi_OnPlayerDie();

			
		}
	}

}



void ABullettimeCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();

	}
}

void ABullettimeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//현재 체력 리플리케이트
	DOREPLIFETIME(ABullettimeCharacter, CurHealth);
}


void ABullettimeCharacter::Multi_OnPlayerDie_Implementation()
{
	// 플레이어 메시를 물리 시뮬레이션 시키기
	DoRagdoll();

}


void ABullettimeCharacter::DoRagdoll()
{

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);

	// GetMesh()->SetCollisionProfileName("Ragdoll");

	GetMesh()->SetSimulatePhysics(true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetCharacterMovement()->DisableMovement();

}


void ABullettimeCharacter::AddScore()
{
	ABullettimePlayerState* ThisPlayerState = GetController()->GetPlayerState<ABullettimePlayerState>();
	ThisPlayerState->AddPlayerDeath();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("DeathAddScore %d"), ThisPlayerState->PlayerId));


	ABullettimePlayerController* OtherController = Cast<ABullettimePlayerController>(RecentDamageCauser);
	if (OtherController)
	{
		ABullettimePlayerState* OtherPlayerState = OtherController->GetPlayerState<ABullettimePlayerState>();
		if (OtherPlayerState)
		{
			OtherPlayerState->AddPlayerKill();
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("killAddScore %d"), OtherPlayerState->PlayerId));
		}
	}

}

void ABullettimeCharacter::SetRespawnTimer()
{
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ABullettimeCharacter::Client_RespawnCharacter, 5.0f, false);
	FTimerHandle WaitHandle;
	float WaitTime = 6.0f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			Destroy();

		}), WaitTime, false);

}

void ABullettimeCharacter::CameraWork()
{
	//bUseControllerRotationYaw = false;
	//CameraComponent->bUsePawnControlRotation = false;
	//CameraComponent->bUseAttachParentBound = false;

	RotationTimelineComp->Play();
	TransformTimelineComp->Play();
}

void ABullettimeCharacter::ChangeThirdPerson()
{
	// 1인칭 팔 메시와 1인칭 총 메시를 안보이게 함
	Mesh1P->SetVisibility(false, true);

	GetMesh()->SetOwnerNoSee(false);

}

void ABullettimeCharacter::Client_RespawnCharacter_Implementation()
{
	GetController<ABullettimePlayerController>()->Server_RespawnPawn(SpawnTransform);
	
}

void ABullettimeCharacter::UpdateRotationTimelineComp(FVector Output)
{
	//FRotator CameraNewRotation = FRotator();
	// rotator
	CameraComponent->SetRelativeRotation(FRotator(Output.Y, 0.f, 0.f));

}

void ABullettimeCharacter::UpdateTransformTimelineComp(FVector Output)
{
	FTransform CameraNewPosition = FTransform(Output);
	CameraComponent->SetRelativeTransform(CameraNewPosition);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%s"), *(Output.ToString())));
}