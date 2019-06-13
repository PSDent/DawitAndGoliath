// Fill out your copyright notice in the Description page of Project Settings.

#include "DNGGameModeBase.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BaseController.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine.h"

ADNGGameModeBase::ADNGGameModeBase()
{
	bUseSeamlessTravel = true;
	BarrackCount = 2;
	
}

void ADNGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void ADNGGameModeBase::GameSet(bool isRts)
{
	TArray<AActor*> arr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseController::StaticClass(), arr);
	for (auto a : arr)
	{
		Cast<ABaseController>(a)->ShowGameSetUI(isRts);
	}
}

void ADNGGameModeBase::OnBarrackDestroyed()
{
	--BarrackCount;
	if (BarrackCount == 0)
	{
		GameSet(false);
	}
}

void ADNGGameModeBase::OnPlayerKilled()
{
	--CurrentPlayerCount;
	if (CurrentPlayerCount == 0)
	{
		GameSet(true);
	}
}

