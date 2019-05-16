// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSUnit_Melee.h"
#include "Engine.h"
#include "DNGProperty.h"
#include "DNG_RTSPawn.h"

ADNG_RTSUnit_Melee::ADNG_RTSUnit_Melee() : Super()
{
	FCommandDelegate dele;

	dele.BindUFunction(this, FName("Attack"));
	commandInfoMap.Add(EKeys::A, FCommandInfo("Attack", "Attack Enemy to Click Somewhere", EKeys::A, 1, 0, dele));
	
	objProperty->SetMaxHp(50.0f);
	objProperty->SetHp(50.0f);
	unitName = "MeleeUnit";
}

void ADNG_RTSUnit_Melee::BeginPlay()
{
	Super::BeginPlay();
}

void ADNG_RTSUnit_Melee::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
}

void ADNG_RTSUnit_Melee::Attack()
{
	if (/*!pawn->GetLeftMouseStatus() && */!pawn->GetRightMouseStatus())
	{
		pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Crosshairs;
	}

	commandCheckDele.BindLambda(
		[&] {
		if (pawn->GetLeftMouseStatus() || pawn->GetRightMouseStatus())
		{
			// BT에서 공격 플래그를 체크한 후
			// 해당 위치로 이동하며 계속 공격 대상을 찾는다.
			// 적을 지목하였다면 계속해서 적을 쫒아가며 공격해간다.

			// 계속해서 목표 위치를 갱신하는 함수를 작성하여
			// 루프를 돌릴 것.
			if (pawn->targetActor)
			{
				target = pawn->targetActor;
			}
			else
			{

			}
			
			aiController->MoveToLocation(pawn->targetPos);

			pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Default;

			GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
		}
	}
	);
	GetWorld()->GetTimerManager().SetTimer(commandCheckHandle, commandCheckDele, 0.001f, true, 0.0f);
}