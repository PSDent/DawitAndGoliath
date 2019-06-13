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

		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADNG_RTSBaseObject::StaticClass(), actors);

		for (auto obj : actors)
		{
			ADNG_RTSBaseObject *unit = Cast<ADNG_RTSBaseObject>(obj);
			unit->SetOwner(pawn);
			unit->Controller->SetOwner(pawn);
			//unit->Client_AfterInit();
		}
	}
}

void ABaseController::ShowGameSetUI_Implementation(bool isRts)
{

}