// Copyright Epic Games, Inc. All Rights Reserved.

#include "BullettimeGameMode.h"
#include "BullettimeCharacter.h"
#include "Bullettime.h"
#include "UObject/ConstructorHelpers.h"
#include "Math/TransformNonVectorized.h"
#include "BullettimePlayerController.h"
#include "GenericPlatform/GenericPlatformProcess.h"

//#include "Engine/World.h"

ABullettimeGameMode::ABullettimeGameMode()
{
	//PRINT_LOG(TEXT("My Log : %s"), TEXT("Bullettime project!!"));

	//// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}

	//Controller
}
						  
void ABullettimeGameMode::Server_RespawnRequested_Implementation(ABullettimePlayerController* PlayerController, FTransform SpawnTransform)
{
	if(IsValid(PlayerController)){
		const FRotator SpawnRotation = SpawnTransform.Rotator();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = SpawnTransform.GetLocation();
		FActorSpawnParameters ActorSpawnParams;
	
		ABullettimeCharacter* Character = GetWorld()->SpawnActor<ABullettimeCharacter>(BullettimeCharacterClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

		PlayerController->Possess(Character);
	
	}
	
}
