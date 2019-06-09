// Fill out your copyright notice in the Description page of Project Settings.

#include "DNGGameModeBase.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BaseController.h"
#include "Engine.h"

ADNGGameModeBase::ADNGGameModeBase()
{
	bUseSeamlessTravel = true;
	BarrackCount = 2;
	
}

void ADNGGameModeBase::GameSet()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Game Set");
	TArray<AActor*> arr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseController::StaticClass(), arr);
	for (auto a : arr)
	{
		Cast<ABaseController>(a)->ShowGameSetUI();

	}
}

void ADNGGameModeBase::OnBarrackDestroyed()
{
	--BarrackCount;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Barrack left : %d"), BarrackCount));
	if (BarrackCount == 0)
	{
		GameSet();
	}
}

void ADNGGameModeBase::OnPlayerKilled()
{
	--CurrentPlayerCount;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Player left : %d"), CurrentPlayerCount));
	if (CurrentPlayerCount == 0)
	{
		GameSet();
	}
}

