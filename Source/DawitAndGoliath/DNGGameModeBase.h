// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DNGGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API ADNGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADNGGameModeBase();

	void GameSet(bool isRts);
	void OnBarrackDestroyed();
	void OnPlayerKilled();

protected:
	int BarrackCount;
	UPROPERTY(BlueprintReadWrite)
	int CurrentPlayerCount;
	UPROPERTY(BlueprintReadWrite)
	int PlayerCount;
};
