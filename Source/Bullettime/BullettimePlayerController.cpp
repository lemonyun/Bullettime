// Fill out your copyright notice in the Description page of Project Settings.


#include "BullettimePlayerController.h"
#include "GameFramework/HUD.h"

ABullettimePlayerController::ABullettimePlayerController()
{
	
}

void ABullettimePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		static ConstructorHelpers::FClassFinder<AHUD> HUDClass(TEXT("WidgetBlueprint'/Game/UI/HeadsUpDisplay.HeadsUpDisplay'"));
		
		ClientSetHUD(HUDClass.Class);
		
		SpawnDefaultHUD();
	}


}