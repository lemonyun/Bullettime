#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BullettimePlayerHUD.generated.h"

UCLASS(Abstract)
class BULLETTIME_API UBullettimePlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealth(float CurrentHealth, float MaxHealth);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* HealthBar;

	/*UPROPERTY()
	class ABullettimeCharacter* Character;

	UPROPERTY()
	class ABullettimePlayerController* PlayerController;*/

	//virtual void NativeConstruct() override;

	//void Update();

	//UPROPERTY()
	//float MaxHealth = 100.0f;

};