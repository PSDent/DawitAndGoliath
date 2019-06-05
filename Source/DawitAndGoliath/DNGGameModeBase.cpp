// Fill out your copyright notice in the Description page of Project Settings.

#include "DNGGameModeBase.h"
#include "Engine.h"

ADNGGameModeBase::ADNGGameModeBase()
{
	bUseSeamlessTravel = true;

}

void ADNGGameModeBase::GameSet()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Game Set");
}

void ADNGGameModeBase::OnBarrackDestroyed()
{

}

void ADNGGameModeBase::OnPlayerKilled()
{

}

