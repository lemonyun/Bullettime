// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UOnlineGameInstance::UOnlineGameInstance()
{
	MySessionName = FName("My Session");
}

void UOnlineGameInstance::Init()
{
	
	if (IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get())
	{
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			// delegate ���ε��ϱ�
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UOnlineGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UOnlineGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UOnlineGameInstance::OnJoinSessionComplete);
			
		}
	}
}

void UOnlineGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete, Succeeded: %d"), Succeeded);

	if (Succeeded)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");

	}
}

void UOnlineGameInstance::OnFindSessionComplete(bool Succeeded)
{
	SearchingForServer.Broadcast(false);

	UE_LOG(LogTemp, Warning, TEXT("OnFind SessionComplete, Succeeded: %d"), Succeeded);
	if (Succeeded)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		int32 ArrayIndex = 0;


		for (FOnlineSessionSearchResult Result : SearchResults)
		{
			
			if(!Result.IsValid())
				continue;

			FServerInfo Info;
			FString ServerName = "Empty Server Name";
			
			Result.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), ServerName);
			
			Info.ServerName = ServerName;
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;
			Info.ServerArrayIndex = ArrayIndex;
			Info.SetPlayerCount();

			Info.IsLan = Result.Session.SessionSettings.bIsLANMatch;
			Info.Ping = Result.PingInMs;


			// �������Ʈ���� ���ε��� �Լ��� �����(���� ����)
			ServerListDel.Broadcast(Info);

			++ArrayIndex;
		}
		UE_LOG(LogTemp, Warning, TEXT("SearchResults, Server Count: %d"), SearchResults.Num());

	}
}

void UOnlineGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete, SessionName : %s"), *SessionName.ToString());

	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if(JoinAddress != "")
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
	}
}

// MainMenu_UI�� HostButton Onclick �̺�Ʈ�� ȣ���
void UOnlineGameInstance::CreateServer(FCreateServerInfo ServerInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("CreateServer"));

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	if(IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
		SessionSettings.bIsLANMatch = false;
	else
		SessionSettings.bIsLANMatch = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.NumPublicConnections = ServerInfo.MaxPlayers;

	SessionSettings.Set(FName("SERVER_NAME_KEY"), ServerInfo.ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//SessionSettings.Set

	// hosting player id, ���� �̸�, ���� ���� �Ķ���� ����ü
	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void UOnlineGameInstance::SearchServer()
{
	SearchingForServer.Broadcast(true); 
	
	UE_LOG(LogTemp, Warning, TEXT("JoinServer"));

	SessionSearch = MakeShareable(new FOnlineSessionSearch()) ;
	if (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
		SessionSearch->bIsLanQuery = false; // LAN���� ã����?
	else
		SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 150000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // ���� �˻��� ����� ����

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UOnlineGameInstance::JoinServer(int32 ArrayIndex)
{
	FOnlineSessionSearchResult Result = SessionSearch->SearchResults[ArrayIndex];
	if (Result.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("JOINING SESSION AT INDEX: %d"), ArrayIndex);
		SessionInterface->JoinSession(0, MySessionName, Result);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO JOIN SERVER AT INDEX: %d"), ArrayIndex);
	}

}

void UOnlineGameInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOnlineGameInstance, NumberOfPlayers);
}