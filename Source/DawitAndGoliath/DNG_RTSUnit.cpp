// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSUnit.h"
#include "DNG_RTSPawn.h"
#include "FPSCharacter.h"
#include "DNGProperty.h"
#include "Net/UnrealNetwork.h"
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
	deadDelay = 1.0f;

}

void ADNG_RTSUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADNG_RTSUnit, bIsDie);
	DOREPLIFETIME(ADNG_RTSUnit, bIsWalk);

}

void ADNG_RTSUnit::BeginPlay()
{
	Super::BeginPlay();

	Server_BeginPlay();

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TSubclassOf<AFPSCharacter>(), actors);

	for (auto actor : actors)
	{
		AFPSCharacter *enemy = Cast<AFPSCharacter>(actor);
		enemyPlayers.Add(enemy);
	}
}

void ADNG_RTSUnit::Server_BeginPlay_Implementation()
{
	aiController->UseBlackboard(useBB, blackBoard);
	aiController->RunBehaviorTree(useBT);

	blackBoard->SetValueAsBool(key_IsCanDeal, true);
}

void ADNG_RTSUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->Velocity.Size2D() > 0.0f)
		bIsWalk = true;
	else
		bIsWalk = false;

	if (objProperty->GetHp() <= 0.0f)
	{
		Server_Die();
	}
}

void ADNG_RTSUnit::Server_Die_Implementation()
{
	bIsDie = true;

}

void ADNG_RTSUnit::Server_AfterDie_Implementation()
{
	afterDieDele.BindLambda([&]()
	{
		Destroy();
	});
	GetWorld()->GetTimerManager().SetTimer(afterDieTimerHandle, afterDieDele, 0.1f, false, deadDelay);
}

void ADNG_RTSUnit::Move()
{
	FVector dest;

	if (!pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "No Pawn");

		return;
	}

	// ���⼭ �ڲ� Pawn�� Null�̶�� ���.
	// ���� ������ �ִ� �� ����.
	// �� �ذ� �غ���.
	pawn->SetCommandingFlag(true);
	if (/*!pawn->GetLeftMouseStatus() && */!pawn->GetRightMouseStatus())
	{
		pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Crosshairs;
	}

	commandCheckDele.BindLambda(
		[&] {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Time Ticking");

		if (pawn->GetLeftMouseStatus() || pawn->GetRightMouseStatus())
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Detect Mouse Move Click");

			Server_Move(pawn->targetPos);

			pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Default;

			GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
		}
	}
	);
	GetWorld()->GetTimerManager().SetTimer(commandCheckHandle, commandCheckDele, 0.001f, true, 0.0f);
}

void ADNG_RTSUnit::Server_Move_Implementation(FVector dest)
{
	if (aiController)
		aiController->MoveToLocation(dest);
	else
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "No AI Controller");
}


void ADNG_RTSUnit::Stop()
{
	Server_Stop();
}

void ADNG_RTSUnit::Server_Stop_Implementation()
{
	aiController->StopMovement();
}


void ADNG_RTSUnit::Hold()
{
	Server_Hold();
}

void ADNG_RTSUnit::Server_Hold_Implementation()
{
	aiController->StopMovement();
	bIsHold = true;
}

void ADNG_RTSUnit::Patrol()
{

}

void ADNG_RTSUnit::Server_Patrol_Implementation()
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

void ADNG_RTSUnit::Server_Attack_Implementation()
{
	// ���� ���� ������ �� 
	// BB�� �����Ͽ� BT�� ���� ��.
}


void ADNG_RTSUnit::Deal(AActor *targetActor)
{
	Server_Deal(targetActor);
}

void ADNG_RTSUnit::Server_Deal_Implementation(AActor *targetActor)
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
	float shortestDist = fireRange + 1.0f;
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