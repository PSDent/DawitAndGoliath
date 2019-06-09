// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Online.h"
#include "FindSessionsCallbackProxy.h"
#include "MyGameStateBase.h"
#include "DNGStruct.h"
#include "Engine.h"
#include "FPSCharacter.h"
#include "DNG_RTSPawn.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMyGameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Network|DNG")
		void StartOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|DNG")
		void FindOnlineGames();

	UFUNCTION(BlueprintCallable, Category = "Network|DNG")
		void JoinOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|DNG")
		void DestroySessionAndLeaveGame();

	UFUNCTION(BlueprintCallable, Category = "Network|DNG")
		void InitPlayersPawn();

	bool HostSession(TSharedPtr<const FUniqueNetId> userId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);
	void FindSessions(TSharedPtr<const FUniqueNetId> userId, bool bIsLAN, bool bIsPresence);
	void OnFindSessionsComplete(bool bWasSuccessful);
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult &SearchResult);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Network", meta = (DisplayName = "JoinOnClicked"))
		void JoinOnClicked(FBlueprintSessionResult sessionResult);
	virtual void JoinOnClicked_Implementation(FBlueprintSessionResult sessionResult);

	UFUNCTION(BlueprintCallable, Category = "Network")
		void TravelToGameLevel(FName sessionHostName);

	FTimerHandle ha;

public:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class AFPSCharacter> fpsClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class ADNG_RTSPawn> rtsClass;

	UPROPERTY(BlueprintReadOnly, Category = "Network|Test")
		TArray<FBlueprintSessionResult> sessionResultArray;

protected:

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	TSharedPtr<FOnlineSessionSettings> SessionSetting;
	FName mMapName;

	// Create & Start Sessions
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	// Find Session
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	// Join Session
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	// Destroy Session
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	// Exit Session
	//FOnEndSessionComplete

protected:
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	virtual void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

private:
	TMap<FString, FSessionPlayersInfo> sessionRoleInfo;
};