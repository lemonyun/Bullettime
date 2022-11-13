#include "BullettimePlayerHUD.h"
#include "Components/ProgressBar.h"

void UBullettimePlayerHUD::SetHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
}
