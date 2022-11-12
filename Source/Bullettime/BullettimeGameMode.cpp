// Copyright Epic Games, Inc. All Rights Reserved.

#include "BullettimeGameMode.h"
#include "BullettimeCharacter.h"
#include "Bullettime.h"
#include "UObject/ConstructorHelpers.h"

ABullettimeGameMode::ABullettimeGameMode()
{
	PRINT_LOG(TEXT("My Log : %s"), TEXT("Bullettime project!!"));

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//Controller
}
