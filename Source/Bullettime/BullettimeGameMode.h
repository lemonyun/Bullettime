// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BullettimeGameMode.generated.h"


UCLASS(minimalapi)
class ABullettimeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABullettimeGameMode();
	
	UFUNCTION(Server, Reliable)
	void Server_RespawnRequested(class ABullettimePlayerController* PlayerController, FTransform SpawnTransform);
	
	UPROPERTY(EditDefaultsOnly, Category = Character)
	TSubclassOf<class ABullettimeCharacter> BullettimeCharacterClass;

	
};



