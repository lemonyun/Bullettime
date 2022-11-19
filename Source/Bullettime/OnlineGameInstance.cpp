// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UOnlineGameInstance::UOnlineGameInstance()
{
	
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

void UOnlineGameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete, Succeeded: %d"), Succeeded);

	if (Succeeded)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");

	}
}

void UOnlineGameInstance::OnFindSessionComplete(bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFind SessionComplete, Succeeded: %d"), Succeeded);
	if (Succeeded)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		for (FOnlineSessionSearchResult Result : SearchResults)
		{
			if(!Result.IsValid())
				continue;

			FServerInfo Info;
			Info.ServerName = "Test Server Name";
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;

			// �������Ʈ���� ���ε��� �Լ��� �����(���� ����)
			ServerListDel.Broadcast(Info);
		}
		UE_LOG(LogTemp, Warning, TEXT("SearchResults, Server Count: %d"), SearchResults.Num());

		//if (SearchResults.Num())
		//{

		//	UE_LOG(LogTemp, Warning, TEXT("Joining Server"));
		//	// ����userid �����̸� FOnlineSearchResult

		//	SessionInterface->JoinSession(0, "My Session", SearchResults[0]);
		//}

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
void UOnlineGameInstance::CreateServer()
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
	SessionSettings.NumPublicConnections = 5;

	// hosting player id, ���� �̸�, ���� ���� �Ķ���� ����ü
	SessionInterface->CreateSession(0, FName("My Session"), SessionSettings);
}

void UOnlineGameInstance::SearchServer()
{
	
	UE_LOG(LogTemp, Warning, TEXT("JoinServer"));

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
		SessionSearch->bIsLanQuery = false; // LAN���� ã����?
	else
		SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 150000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // ���� �˻��� ����� ����

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UOnlineGameInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOnlineGameInstance, NumberOfPlayers);
}