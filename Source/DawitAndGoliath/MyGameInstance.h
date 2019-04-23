// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Online.h"
#include "Engine.h"
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

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void StartOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void FindOnlineGames();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void JoinOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void DestroySessionAndLeaveGame();




	bool HostSession(TSharedPtr<const FUniqueNetId> userId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);
	void FindSessions(TSharedPtr<const FUniqueNetId> userId, bool bIsLAN, bool bIsPresence);
	void OnFindSessionsComplete(bool bWasSuccessful);
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult &SearchResult);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

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

protected:
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	virtual void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);


private:


};
