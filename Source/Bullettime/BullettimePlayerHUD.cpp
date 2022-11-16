#include "BullettimePlayerHUD.h"
#include "BullettimePlayerController.h"
#include "BullettimeCharacter.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

void UBullettimePlayerHUD::SetHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

//void UBullettimePlayerHUD::NativeConstruct()
//{
//	FPlatformProcess::Sleep(0.2f);
//		
//	ABullettimePlayerController* BP = Cast<ABullettimePlayerController>(UGameplayStatics::GetPlayerController(this, 0));
//
//	if (BP->IsLocalPlayerController())
//	{
//		 PlayerController = BP;
//		 Character = Cast<ABullettimeCharacter>(PlayerController->GetPawn());
//		 MaxHealth = Character->GetMaxHealth();
//	}
//}

//void UBullettimePlayerHUD::Update()
//{
//	SetHealth(Character->GetCurrentHealth());
//}
