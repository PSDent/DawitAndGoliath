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

public:
	void SetSessionPlayersInfo(FSessionPlayersInfo &sessionPlayersInfo);
	FSessionPlayersInfo GetSessionPlayersInfo();


private:
	
private:
	FSessionPlayersInfo sessionPlayersInfo;
	
};
