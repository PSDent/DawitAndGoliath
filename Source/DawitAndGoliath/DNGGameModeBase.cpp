// Fill out your copyright notice in the Description page of Project Settings.

#include "DNGGameModeBase.h"
#include "Engine.h"

ADNGGameModeBase::ADNGGameModeBase()
{
	bUseSeamlessTravel = true;
	BarrackCount = 2;
	PlayerCount = 1;
}

void ADNGGameModeBase::GameSet()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Game Set");
}

void ADNGGameModeBase::OnBarrackDestroyed()
{
	--BarrackCount;
	if (BarrackCount == 0) GameSet();
}

void ADNGGameModeBase::OnPlayerKilled()
{
	--PlayerCount;
	if (PlayerCount == 0) GameSet();
}

