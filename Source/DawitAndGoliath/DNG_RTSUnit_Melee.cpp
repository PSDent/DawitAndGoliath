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
			// BT���� ���� �÷��׸� üũ�� ��
			// �ش� ��ġ�� �̵��ϸ� ��� ���� ����� ã�´�.
			// ���� �����Ͽ��ٸ� ����ؼ� ���� �i�ư��� �����ذ���.

			// ����ؼ� ��ǥ ��ġ�� �����ϴ� �Լ��� �ۼ��Ͽ�
			// ������ ���� ��.
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