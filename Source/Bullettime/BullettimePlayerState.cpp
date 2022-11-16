// Fill out your copyright notice in the Description page of Project Settings.


#include "BullettimePlayerState.h"
#include "Net/UnrealNetwork.h"

ABullettimePlayerState::ABullettimePlayerState()
{
	PlayerKill = 0;
	PlayerDeath = 0;
}

void ABullettimePlayerState::AddPlayerKill()
{
	PlayerKill += 1;
}

void ABullettimePlayerState::AddPlayerDeath()
{
	PlayerDeath += 1;
}

int ABullettimePlayerState::GetPlayerKill()
{
	return PlayerKill;
}

int ABullettimePlayerState::GetPlayerDeath()
{
	return PlayerDeath;
}

void ABullettimePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 킬 데스 리플리케이트
	DOREPLIFETIME(ABullettimePlayerState, PlayerKill);
	DOREPLIFETIME(ABullettimePlayerState, PlayerDeath);
}