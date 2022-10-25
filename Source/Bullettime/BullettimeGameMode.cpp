// Copyright Epic Games, Inc. All Rights Reserved.

#include "BullettimeGameMode.h"
#include "BullettimeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABullettimeGameMode::ABullettimeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
