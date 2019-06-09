// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseController.h"
#include "FPSCharacter.h"
#include "DNG_RTSPawn.h"
#include "Engine.h"

ABaseController::ABaseController() : Super()
{

}

void ABaseController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseController::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseController::Possess(APawn *pawn)
{
	Super::Possess(pawn);

	if (pawn->IsA(AFPSCharacter::StaticClass()))
	{
		Cast<AFPSCharacter>(pawn);
		Cast<APlayerController>(pawn->Controller)->bShowMouseCursor = true;
	}
	else if (pawn->IsA(ADNG_RTSPawn::StaticClass()))
	{
		Cast<ADNG_RTSPawn>(pawn)->Init();
	}
}

void ABaseController::ShowGameSetUI_Implementation()
{

}