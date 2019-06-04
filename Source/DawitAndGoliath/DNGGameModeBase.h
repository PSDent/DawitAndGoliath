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

	static void GameSet();
	static void OnBarrackDestroyed();
	static void OnPlayerKilled();

private:
	static int BarrackCount;
	static int PlayerCount;
};
