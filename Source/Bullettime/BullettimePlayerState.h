// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BullettimePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BULLETTIME_API ABullettimePlayerState : public APlayerState
{
	GENERATED_BODY()

	ABullettimePlayerState();

private:
	UPROPERTY(Replicated)
	int PlayerKill;

	UPROPERTY(Replicated)
	int PlayerDeath;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	void AddPlayerKill();

	void AddPlayerDeath();


	int GetPlayerKill();

	int GetPlayerDeath();
	
};
