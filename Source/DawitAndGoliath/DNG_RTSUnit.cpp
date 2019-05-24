// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSUnit.h"
#include "DNG_RTSPawn.h"
#include "FPSCharacter.h"
#include "DNGProperty.h"
#include "Engine.h"

ADNG_RTSUnit::ADNG_RTSUnit() : Super()
{
	FCommandDelegate dele;
	bReplicates = true;

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

	if (!aiController)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "No AI Controller");
	else
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, "Ai Controller Exist");

	// ���⿡ ���� ������ �ִ� �����.
	//aiController->UseBlackboard(useBB, blackBoard);
	//aiController->RunBehaviorTree(useBT);

	//blackBoard->SetValueAsBool(key_IsCanDeal, true);

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
	if (Role == ROLE_Authority)
	{
		FVector dest;

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Move!");

		// return;
		// �� �̻����� �ڵ尡 �Ѿ�� �ñ�
		// ���� ���ų� �÷��̾� ��Ʈ�ѷ��� �̻��Ѱ� ��;

		pawn->SetCommandingFlag(true);
		if (/*!pawn->GetLeftMouseStatus() && */!pawn->GetRightMouseStatus())
		{
			pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Crosshairs;
		}

		commandCheckDele.BindLambda(
			[&] {
			if (pawn->GetLeftMouseStatus() || pawn->GetRightMouseStatus())
			{
				if (aiController)
					aiController->MoveToLocation(pawn->targetPos);
				else
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "No AI Controller");

				pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Default;

				GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
			}
		}
		);
		GetWorld()->GetTimerManager().SetTimer(commandCheckHandle, commandCheckDele, 0.001f, true, 0.0f);
	}
	else
	{
		Server_Move();
	}
}

void ADNG_RTSUnit::Server_Move_Implementation()
{
	Move();
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
	pawn->SetCommandingFlag(true);
	
	pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Crosshairs;

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
				GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Attack Target");
				Deal(target);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Attack Ground");
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
	bool isInRange = GetDistanceTo(targetActor) <= fireRange ? true : false;
	bool isCoolTime = blackBoard->GetValueAsBool(key_IsCanDeal);
	if (!isInRange || !isCoolTime) return;

	blackBoard->SetValueAsBool(key_IsCanDeal, false);
	GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Attack");
	GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, targetActor->GetName());

	if (targetActor->IsA(AFPSCharacter::StaticClass()))
	{
		// FPS ĳ���� HP ���� ����
		//Cast<AFPSCharacter>(target)
	}
	else if (Cast<ADNG_RTSBaseObject>(targetActor))
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Hit!");
		Cast<ADNG_RTSBaseObject>(targetActor)->objProperty->DealDamage(damage);
	}

	attackDelayDele.BindLambda(
		[&]()
	{
		blackBoard->SetValueAsBool(key_IsCanDeal, true);
	}
	);
	GetWorld()->GetTimerManager().SetTimer(attackDelayTimerHandle, attackDelayDele, 0.01f, false, fireRate);
}

void ADNG_RTSUnit::Check()
{
	//CompareDistance();
}

void ADNG_RTSUnit::CompareDistance()
{
	// �����Ѵ�.
	if (target)
	{
		Deal(target);
		return;
	}

	// ��Ȯ�� ������ ���� ���ٸ� ��� �� ����Ʈ���� �Ÿ��� ã�Ƽ� ���� ª�� �Ÿ��� �ִ� ���� ����
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

	if(shortestDist <= fireRange)
		Deal(shortestDistEnemy);
}