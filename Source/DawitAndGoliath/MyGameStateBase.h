// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DNGStruct.h"
#include "MyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()


public:
	AMyGameStateBase();


	void ChoosePlayerRole(const FString &roleName, const FName &playerName);

	UFUNCTION(BlueprintCallable)
		TArray<FString> GetLobbyKey();

	UFUNCTION(BlueprintCallable)
		TArray<FName> GetLobbyValue();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;


public:
	void SetSessionPlayersInfo(FSessionPlayersInfo &sessionPlayersInfo);
	
	
	FSessionPlayersInfo GetSessionPlayersInfo();

private:

private:
	UPROPERTY(Replicated)
		TArray<FString> lobbyKey;

	UPROPERTY(Replicated)
		TArray<FName> lobbyValue;

	FSessionPlayersInfo sessionPlayersInfo;
	

};
