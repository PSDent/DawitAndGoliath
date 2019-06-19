// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "BaseController.h"

#include "MyPlayerState.h"
#include "DNG_RTSBarrack.h"

UMyGameInstance::UMyGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnStartOnlineGameComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnJoinSessionComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnDestroySessionComplete);
}

bool UMyGameInstance::HostSession(TSharedPtr<const FUniqueNetId> userId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem *const onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		IOnlineSessionPtr Sessions = onlineSub->GetSessionInterface();

		if (Sessions.IsValid() && userId.IsValid())
		{
			SessionSetting = MakeShareable(new FOnlineSessionSettings());

			SessionSetting->bIsLANMatch = bIsLAN;
			SessionSetting->bUsesPresence = bIsPresence;
			SessionSetting->NumPublicConnections = MaxNumPlayers;
			SessionSetting->NumPrivateConnections = 0; 
			SessionSetting->bAllowInvites = true;
			SessionSetting->bAllowJoinInProgress = true;
			SessionSetting->bShouldAdvertise = true;
			SessionSetting->bAllowJoinViaPresence = true;
			SessionSetting->bAllowJoinViaPresenceFriendsOnly = false;
			SessionSetting->Set(SETTING_MAPNAME, FString("NewMap"), EOnlineDataAdvertisementType::ViaOnlineService);
			
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			return Sessions->CreateSession(*userId, SessionName, *SessionSetting);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Fail");
	}

	return false;
}

// 게임 시작할 때 
// Game Instance에 방 정보를 저장 후
// 저장 된 정보를 통해 플레이어의 Player Controller를 달리 한다. (RTS / TPS)
void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
				Sessions->StartSession(SessionName);
			}
		}
	}

}

void UMyGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			FString serverName = Sessions.Get()->GetNamedSession(SessionName)->OwningUserName;
			sessionRoleInfo.Add(serverName, FSessionPlayersInfo());
		}

		UGameplayStatics::OpenLevel(GetWorld(), "Lobby", true, "listen");
	}
}

void UMyGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> userId, bool bIsLAN, bool bIsPresence)
{
	IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && userId.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;
			
			if (bIsPresence)
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			Sessions->FindSessions(*userId, SearchSettingsRef);
		}
	}
	else
	{
		OnFindSessionsComplete(false);
	}

}

void UMyGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem * const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			UGameplayStatics::OpenLevel(GetWorld(), "ServerList_Level", true, "listen");

			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			if (SessionSearch->SearchResults.Num() > 0)
			{
				int sessionNum = SessionSearch->SearchResults.Num();
				sessionResultArray.Init(FBlueprintSessionResult(), sessionNum);
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); ++SearchIdx)
				{
					sessionResultArray[SearchIdx].OnlineResult = SessionSearch->SearchResults[SearchIdx];
				}
			}

		}
	} 
}

// 호스트가 레벨을 옮기면
// 세션이 종료되고 새로운 세션이 만들어지거나 파괴되는것으로 보임.
// 아니면, 호스트를 제외한 클라이언트들이 호스트의 레벨을 따라오지 못하여
// 생기는 문제로서 판단이 됨.
// 이를 염두에 둘 것.
bool UMyGameInstance::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult &SearchResult)
{
	bool bSuccessful = false;

	IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
			//Sessions->Exit
		}
	}

	return bSuccessful;
}

void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
			APlayerController * const PlayerController = GetFirstLocalPlayerController();
			
			FString TravelURL;
			
			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute, true);
				AGameStateBase *gameStateBase =  UGameplayStatics::GetGameState(GetWorld());
				TArray<APlayerState*> playerArr = gameStateBase->PlayerArray;

			}
		}
	}
}

void UMyGameInstance::StartOnlineGame()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	if(Player)
		HostSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, true, true, 4);
}

void UMyGameInstance::FindOnlineGames()
{
	ULocalPlayer * const Player = GetFirstGamePlayer();

	FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId(), true, true);
}

void UMyGameInstance::JoinOnlineGame()
{
	ULocalPlayer * const Player = GetFirstGamePlayer();

	FOnlineSessionSearchResult SearchResult;

	if (SessionSearch->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); ++i)
		{
			if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId())
			{
				SearchResult = SessionSearch->SearchResults[i];

				JoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, SearchResult);
				break;
			}
		}
	}

}

void UMyGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			Sessions->DestroySession(GameSessionName);
		}
	}
}

void UMyGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{

	IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			if (bWasSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), "MainMenu", true);
			}
		}
	}
}

void UMyGameInstance::JoinOnClicked_Implementation(FBlueprintSessionResult sessionResult)
{
	ULocalPlayer * const Player = GetFirstGamePlayer();
	
	JoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, sessionResult.OnlineResult);
}

void UMyGameInstance::TravelToGameLevel(FName sessionHostName)
{
	AMyGameStateBase *gameState = Cast<AMyGameStateBase>(GetWorld()->GetGameState());
	TArray<FString> keys = gameState->GetLobbyKey();
	TArray<FName> values = gameState->GetLobbyValue();
	TArray<APlayerState*> playerStates = gameState->PlayerArray;

	for (int i = 0; i < 5; ++i)
	{
		if (values[i] == "None") continue;

		for (int j = 0; j < playerStates.Num(); ++j)
		{
			FString name = playerStates[j]->GetPlayerName();

			if (name == values[i].ToString())
			{
				AMyPlayerState *state = Cast<AMyPlayerState>(playerStates[j]);
				FString roleName;

				if (keys[i].Contains("Shooter"))
					roleName = "Shooter";
				else if (keys[i].Contains("RTS"))
					roleName = "RTS";

				state->SetPlayRole(roleName);
				break;
			}
		}
	}

	GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;
	GetWorld()->ServerTravel("/Game/Blueprints/MainMap");
}

void UMyGameInstance::InitPlayersPawn()
{
	AMyGameStateBase *gameState = Cast<AMyGameStateBase>(GetWorld()->GetGameState());
	IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

	if (Sessions.IsValid())
	{
		FString serverName = Sessions.Get()->GetNamedSession(GameSessionName)->OwningUserName;
		TArray<AActor*> playerCtrlArr;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), playerCtrlArr);

		TArray<AActor*> startingPoint;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), startingPoint);
	
		FVector fpsStartPos;
		FVector rtsStartPos;

		for (int i = 0; i < startingPoint.Num(); ++i)
		{
			if (startingPoint[i]->GetName().Contains("RTS"))
				rtsStartPos = startingPoint[i]->GetActorLocation();
			else
				fpsStartPos = startingPoint[i]->GetActorLocation();
		}
		
		for (int i = 0; i < playerCtrlArr.Num(); ++i)
		{
			//if (sessionRoleInfo.Find(serverName))
			//{
			APawn *pawn = nullptr;
			FActorSpawnParameters spawnInfo;
			spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FString roleName = Cast<AMyPlayerState>(Cast<APlayerController>(playerCtrlArr[i])->PlayerState)->playRoleName;
			FRotator rot(0, 0, 0);

			if (roleName == "Shooter")
				pawn = GetWorld()->SpawnActor<AFPSCharacter>(fpsClass, fpsStartPos, rot, spawnInfo);
			else if (roleName == "RTS")
				pawn = GetWorld()->SpawnActor<ADNG_RTSPawn>(rtsClass, rtsStartPos, rot, spawnInfo);

			if (!pawn)
			{
				return;
			}

			ABaseController* playerController = Cast<ABaseController>(playerCtrlArr[i]);
			playerController->Possess(pawn);
		}
	}
}