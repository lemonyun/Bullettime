// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BullettimePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BULLETTIME_API ABullettimePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ABullettimePlayerController();

	virtual void SetupInputComponent() override;

	void ViewPlayersInfo();

	UFUNCTION(Server, Reliable)
	void Server_RespawnPawn(FTransform SpawnTransform);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UBullettimePlayerHUD> PlayerHUDClass;

	UPROPERTY()
	class UBullettimePlayerHUD* PlayerHUD;

protected:
	virtual void BeginPlay() override;


};
