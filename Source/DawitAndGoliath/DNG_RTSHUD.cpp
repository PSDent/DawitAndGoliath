// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSHUD.h"
#include "Engine.h"

ADNG_RTSHUD::ADNG_RTSHUD() : Super()
{
	minimapPointSize = 5.0f;
}

void ADNG_RTSHUD::BeginPlay()
{
	Super::BeginPlay();

	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, "HUD On");
}

void ADNG_RTSHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADNG_RTSHUD::DrawHUD()
{
	Super::DrawHUD();
}