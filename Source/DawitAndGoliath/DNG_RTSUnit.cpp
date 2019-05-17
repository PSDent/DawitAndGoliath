// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSUnit.h"
#include "DNG_RTSPawn.h"
#include "FPSCharacter.h"
#include "Engine.h"

ADNG_RTSUnit::ADNG_RTSUnit() : Super()
{
	FCommandDelegate dele;

	dele.BindUFunction(this, FName("Move"));
	commandInfoMap.Add(EKeys::M, FCommandInfo("Move", "Move Unit to Clicked Location", EKeys::M, 0, 0, dele));

	dele.BindUFunction(this, FName("Stop"));
	commandInfoMap.Add(EKeys::S, FCommandInfo("Stop", "Stop Unit Movement", EKeys::S, 0, 1, dele));

	dele.BindUFunction(this, FName("Hold"));
	commandInfoMap.Add(EKeys::H, FCommandInfo("Hold", "Hold Unit Movement", EKeys::H, 0, 2, dele));

	dele.BindUFunction(this, FName("Patrol"));
	commandInfoMap.Add(EKeys::P, FCommandInfo("Patrol", "Patrol Unit point to point", EKeys::P, 0, 3, dele));

	bIsHold = false;
	bIsWalk = false;
}

void ADNG_RTSUnit::BeginPlay()
{
	Super::BeginPlay();

	blackBoard->SetValueAsBool(bIsCanDeal, true);

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TSubclassOf<AFPSCharacter>(), actors);
	
	for (auto actor : actors)
	{
		AFPSCharacter *enemy = Cast<AFPSCharacter>(actor);
		enemyPlayers.Add(enemy);
	}
}

void ADNG_RTSUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->Velocity.Size2D() > 0.0f)
		bIsWalk = true;
	else
		bIsWalk = false;
}

void ADNG_RTSUnit::Move()
{
	FVector dest;

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Move!");

	if (/*!pawn->GetLeftMouseStatus() && */!pawn->GetRightMouseStatus())
	{
		pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Crosshairs;
	}

	commandCheckDele.BindLambda(
		[&] {
		if (pawn->GetLeftMouseStatus() || pawn->GetRightMouseStatus())
		{
			aiController->MoveToLocation(pawn->targetPos);

			pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Default;

			GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
		}
	}
	);
	GetWorld()->GetTimerManager().SetTimer(commandCheckHandle, commandCheckDele, 0.001f, true, 0.0f);
}

void ADNG_RTSUnit::Stop()
{
	aiController->StopMovement();
}


void ADNG_RTSUnit::Hold()
{
	Stop();
	bIsHold = true;
}


void ADNG_RTSUnit::Patrol()
{

}

void ADNG_RTSUnit::Attack()
{

	pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Crosshairs;

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

void ADNG_RTSUnit::Deal(AActor *targetActor)
{
	blackBoard->SetValueAsBool(bIsCanDeal, false);
	GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Attack");


	if (targetActor->IsA(AFPSCharacter::StaticClass()))
	{
		//Cast<AFPSCharacter>(target)
	}
	else if (targetActor->IsA(ADNG_RTSBaseObject::StaticClass()))
	{
		Cast<ADNG_RTSBaseObject>(targetActor)->objProperty->DealDamage(damage);
	}

	attackDelayDele.BindLambda(
		[&]()
	{
		blackBoard->SetValueAsBool(bIsCanDeal, true);
	}
	);
	GetWorld()->GetTimerManager().SetTimer(attackDelayTimerHandle, attackDelayDele, 0.0f, false, fireRate);
}

void ADNG_RTSUnit::Check()
{
	CompareDistance();
}

void ADNG_RTSUnit::CompareDistance()
{
	// 우선, 공격을 정확히 지정한 적의 거리를 잰다
	// 공격한다.
	if (target)
	{
		Deal(target);
		return;
	}

	// 정확히 지정한 적이 없다면 모든 적 리스트에서 거리를 찾아서 제일 짧은 거리에 있는 적을 공격
	AFPSCharacter *shortestDistEnemy = nullptr;
	float shortestDist = 100000.0f;
	for (auto enemy : enemyPlayers)
	{
		float dist = GetDistanceTo(enemy);
		
		if (dist <= fireRange)
		{
			if (dist <= shortestDist)
				shortestDistEnemy = enemy;
		}
	}
	Deal(shortestDistEnemy);
}