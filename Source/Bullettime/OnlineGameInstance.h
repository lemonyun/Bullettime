// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FCreateServerInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		FString ServerName;

	UPROPERTY(BlueprintReadWrite)
		int32 MaxPlayers;

	UPROPERTY(BlueprintReadWrite)
		bool IsLan;
};

USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FString ServerName;
	UPROPERTY(BlueprintReadOnly)
		FString PlayerCountStr;
	UPROPERTY(BlueprintReadOnly)
		bool IsLan;
	UPROPERTY(BlueprintReadOnly)
		int32 Ping;
	UPROPERTY(BlueprintReadOnly)
		int32 ServerArrayIndex;

	int32 CurrentPlayers;
	int32 MaxPlayers;


	void SetPlayerCount()
	{
		PlayerCountStr = FString(FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(MaxPlayers));
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDel, FServerInfo, ServerListDel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerSearchingDel, bool, SearchingForServer);

/**
 * 
 */


UCLASS()
class BULLETTIME_API UOnlineGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
	UOnlineGameInstance();


protected:
	FName MySessionName;

	UPROPERTY(BlueprintAssignable)
		FServerDel ServerListDel;

	UPROPERTY(BlueprintAssignable)
		FServerSearchingDel SearchingForServer;
	

	// 세션 인터페이스
	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void CreateServer(FCreateServerInfo ServerInfo);

	UFUNCTION(BlueprintCallable)
	void SearchServer();

	UFUNCTION(BlueprintCallable)
	void JoinServer(int32 ArrayIndex);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(Replicated)
	int NumberOfPlayers;


};
