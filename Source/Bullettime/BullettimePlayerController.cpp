// Fill out your copyright notice in the Description page of Project Settings.


#include "BullettimePlayerController.h"
#include "Engine/World.h"
#include "GameFramework/GamestateBase.h"
#include "BullettimePlayerState.h"
#include "BullettimeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BullettimePlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "BullettimeCharacter.h"

// PlayerController는 SetupInputComponent를 구현하여 커스텀 입력 바인딩을 설정할 수 있다.
// Pawn의 SetupPlayerInputComponent와 비슷한 느낌?

// 캐릭터의 SetupPlayerInputComponent에는 캐릭터 이동과 관련된 키를 설정하고
// 여기에서는 캐릭터 이동과 관련 없는 키 (Esc, Tab) 같은 키를 설정하는 듯?

ABullettimePlayerController::ABullettimePlayerController()
{
	static ConstructorHelpers::FClassFinder<UBullettimePlayerHUD> PlayerHUDBPClass(TEXT("WidgetBlueprint'/Game/UI/BP_PlayerHUD.BP_PlayerHUD_C'"));

	if (PlayerHUDBPClass.Class != NULL)
	{
		PlayerHUDClass = PlayerHUDBPClass.Class;
	}
}

void ABullettimePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("ViewInfo", IE_Pressed, this, &ABullettimePlayerController::ViewPlayersInfo);


}

void ABullettimePlayerController::ViewPlayersInfo()
{


	for(TObjectPtr<APlayerState> PS : GetWorld()->GetGameState()->PlayerArray)
	{
		if (PS)
		{

			ABullettimePlayerState* BPS = Cast<ABullettimePlayerState>(PS);
			if (BPS) {

				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("PlayerID = %d Kill = %d Death = %d"), BPS->PlayerId, BPS->GetPlayerKill(), BPS->GetPlayerDeath()));
				
			}
		}
		
	}

}

void ABullettimePlayerController::BeginPlay()
{
	if (IsLocalPlayerController() && PlayerHUDClass)
	{
		PlayerHUD = CreateWidget<UBullettimePlayerHUD>(this, PlayerHUDClass);
		PlayerHUD->AddToPlayerScreen();
		PlayerHUD->SetHealth(100.0f, 100.0f);
	}
}

void ABullettimePlayerController::Server_RespawnPawn_Implementation(FTransform SpawnTransform)
{
	
	Cast<ABullettimeGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->Server_RespawnRequested(this, SpawnTransform);
}
