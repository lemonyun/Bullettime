// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UOnlineGameInstance::UOnlineGameInstance()
{

}

void UOnlineGameInstance::HostSession(int NumPlayers)
{
	//CreateSession()
}

void UOnlineGameInstance::Init()
{
}

void UOnlineGameInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOnlineGameInstance, NumberOfPlayers);
}