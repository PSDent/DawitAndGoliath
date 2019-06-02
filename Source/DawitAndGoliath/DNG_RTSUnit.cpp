
// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSUnit.h"
#include "DNG_RTSPawn.h"
#include "FPSCharacter.h"
#include "DNGProperty.h"
#include "Net/UnrealNetwork.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"

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

	fireAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("fireAudioComponent"));
	fireAudioComponent->AttachTo(RootComponent);
	fireAudioComponent->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));

	patrolPointTriggerOne = CreateDefaultSubobject<USphereComponent>(TEXT("patrolPointTriggerOne"));
	patrolPointTriggerTwo = CreateDefaultSubobject<USphereComponent>(TEXT("patrolPointTriggerTwo"));

	bIsHold = false;
	bIsWalk = false;
	deadDelay = 1.0f;

}

void ADNG_RTSUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADNG_RTSUnit, bIsDie);
	DOREPLIFETIME(ADNG_RTSUnit, bIsWalk);
	DOREPLIFETIME(ADNG_RTSUnit, target);
}

void ADNG_RTSUnit::BeginPlay()
{
	Super::BeginPlay();

	Server_BeginPlay();

	if (fireAudioComponent->IsValidLowLevelFast())
	{
		if(fireSound)
			fireAudioComponent->SetSound(fireSound);
	}

}

void ADNG_RTSUnit::Server_BeginPlay_Implementation()
{
	aiController->UseBlackboard(useBB, blackBoard);
	aiController->RunBehaviorTree(useBT);

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFPSCharacter::StaticClass(), actors);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("%d"), actors.Num()));

	for (auto actor : actors)
	{
		AFPSCharacter *enemy = Cast<AFPSCharacter>(actor);
		enemyPlayers.Add(enemy);
	}

	blackBoard->SetValueAsBool(key_IsCanDeal, true);
}

void ADNG_RTSUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	fireEffectPos = GetActorLocation();

	if (GetCharacterMovement()->Velocity.Size2D() > 0.0f)
		bIsWalk = true;
	else
		bIsWalk = false;

	if (objProperty->GetHp() <= 0.0f && !bIsDie)
	{
		Server_Die();
	}

	if (target && !bIsWalk)
	{
		TurnToTarget();
	}

}

void ADNG_RTSUnit::TurnToTarget()
{
	if (Role == ROLE_Authority)
	{
		if (target)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, "Has Target");

			FRotator deltaRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), target->GetActorLocation());
			deltaRot.Pitch = 0.0f;
			SetActorRotation(deltaRot);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, "No Target");
		}
	}
	else
	{
		Server_TurnToTarget();
	}
}

void ADNG_RTSUnit::Server_TurnToTarget_Implementation()
{
	TurnToTarget();
}

void ADNG_RTSUnit::Server_Die_Implementation()
{
	bIsDie = true;
	bIsAlive = false;
	bIsSelected = false;
	pawn->currentSupply -= supply;
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

	GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
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
	GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
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

			Server_Patrol(GetActorLocation(), pawn->targetPos);

			pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Default;

			GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
		}
	}
	);
	GetWorld()->GetTimerManager().SetTimer(commandCheckHandle, commandCheckDele, 0.001f, true, 0.0f);
}

void ADNG_RTSUnit::Server_Patrol_Implementation(const FVector &posOne, const FVector &posTwo)
{
	// nowPos == posOne;
	patrolPointOne = posOne;
	patrolPointTwo = posTwo;
	nextPatrolPoint = patrolPointTwo;

	patrolPointTriggerOne->SetWorldLocation(patrolPointOne);
	patrolPointTriggerTwo->SetWorldLocation(patrolPointTwo);
	nextPatrolPointTrigger = patrolPointTriggerTwo;

	Server_Move(nextPatrolPoint);

	blackBoard->SetValueAsBool(key_IsPatrolling, true);
}

void ADNG_RTSUnit::Server_CheckPatrol_Implementation()
{
	if (!blackBoard->GetValueAsBool(key_IsPatrolling)) return;

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "RUNNING");

	TArray<AActor*> actors;
	nextPatrolPointTrigger->SetWorldLocation(nextPatrolPoint);
	nextPatrolPointTrigger->SetSphereRadius(30.0f);
	nextPatrolPointTrigger->GetOverlappingActors(actors);
	//if ()
	//{

	if (actors.Find(this) != INDEX_NONE)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Reached");

		if (nextPatrolPoint == patrolPointOne)
		{
			nextPatrolPoint = patrolPointTwo;
			nextPatrolPointTrigger = patrolPointTriggerTwo;
		}
		else
		{
			nextPatrolPoint = patrolPointOne;
			nextPatrolPointTrigger = patrolPointTriggerOne;
		}
		Server_Move(nextPatrolPoint);
	}
	//}
}


void ADNG_RTSUnit::Attack()
{
	GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
	pawn->SetCommandingFlag(true);
	
	pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Crosshairs;

	commandCheckDele.BindLambda(
		[&] {
		if (pawn->GetLeftMouseStatus() || pawn->GetRightMouseStatus())
		{
			if (pawn->targetActor)
			{
				bIsMarkTarget = true;
				Server_Attack(pawn->targetActor);
			}
			else
			{
				bIsMarkTarget = false;
				Server_Move(pawn->targetPos);
				GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Attack Ground");
			}

			pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Default;

			GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
		}
	}
	);
	GetWorld()->GetTimerManager().SetTimer(commandCheckHandle, commandCheckDele, 0.001f, true, 0.0f);
}

void ADNG_RTSUnit::Server_Attack_Implementation(AActor *targetActor)
{
	target = targetActor;
	GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Attack Target");
	Deal();
}


void ADNG_RTSUnit::Deal()
{
	Server_Deal();
}

void ADNG_RTSUnit::Server_Deal_Implementation()
{
	if (!target) return;

	bool isInRange = GetDistanceTo(target) <= fireRange ? true : false;
	bool isCoolTime = blackBoard->GetValueAsBool(key_IsCanDeal);
	bool isMe = target == Cast<AActor>(this);
	bool isMoving = GetCharacterMovement()->Velocity.Size2D() != 0;

	if (!isInRange || !isCoolTime || isMe || isMoving) return;

	if (bIsWalk)
	{
		Server_Stop();
	}

	Multicast_FireEffect(target->GetActorLocation());
	TurnToTarget();

	blackBoard->SetValueAsBool(key_IsCanDeal, false);
	GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Attack");
	GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, target->GetName());

	if (target->IsA(AFPSCharacter::StaticClass()))
	{
		Cast<AFPSCharacter>(target)->Prop->DealDamage(damage);
	}
	else if (Cast<ADNG_RTSBaseObject>(target))
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Hit!");
		Cast<ADNG_RTSBaseObject>(target)->objProperty->DealDamage(damage);
	}

	attackDelayDele.BindLambda(
		[&]()
	{
		blackBoard->SetValueAsBool(key_IsCanDeal, true);
	}
	);
	GetWorld()->GetTimerManager().SetTimer(attackDelayTimerHandle, attackDelayDele, 0.01f, false, fireRate);
}

void ADNG_RTSUnit::Multicast_FireEffect_Implementation(FVector pos)
{
	if (fireParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireParticle, fireEffectPos, FRotator(0,0,0));
	}

	if (fireSound)
	{
		fireAudioComponent->Play();
	}

	if (fireAnim)
	{
		PlayAnimMontage(fireAnim);
	}

	if (hitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hitParticle, pos, FRotator(0, 0, 0));
	}
}

void ADNG_RTSUnit::Check()
{
	CompareDistance();
}

void ADNG_RTSUnit::CompareDistance()
{
	Server_CompareDistance();
}

void ADNG_RTSUnit::Server_CompareDistance_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "CompareDist");

	ADNG_RTSBaseObject *obj = Cast<ADNG_RTSBaseObject>(target);
	if (obj)
	{
		if (obj->objProperty->GetHp() <= 0.0f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "a");

			target = nullptr;
		}
	}

	AFPSCharacter *fpsObj = Cast<AFPSCharacter>(target);
	if (fpsObj)
	{
		if (fpsObj->GetHp() <= 0.0f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "b");

			target = nullptr;
		}
	}

	if (target)
	{
		bool isMoving = GetCharacterMovement()->Velocity.Size2D() > 0;
		bool isDealing = blackBoard->GetValueAsBool(key_IsWantToDeal);
		bool isChasing = blackBoard->GetValueAsBool(key_IsChasing);

		if (isMoving && isDealing && !isChasing)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Cancle");

			target = nullptr;
			return;
		}
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "NORMAL");

		float dist = GetDistanceTo(target);

		
		if (dist <= fireRange)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Fire");

			blackBoard->SetValueAsBool(key_IsWantToDeal, true);
			blackBoard->SetValueAsBool(key_IsChasing, false);
			Server_Stop();
			return;
		}
		else if (dist <= traceRange)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Trace");

			blackBoard->SetValueAsBool(key_IsWantToDeal, false);
			blackBoard->SetValueAsBool(key_IsChasing, true);	
			return;
		}
		if (bIsMarkTarget)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Mark");

			blackBoard->SetValueAsBool(key_IsWantToDeal, false);
			blackBoard->SetValueAsBool(key_IsChasing, true);
			return;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Nobody");

			blackBoard->SetValueAsBool(key_IsWantToDeal, false);
			blackBoard->SetValueAsBool(key_IsChasing, false);
		}
	}

	// 정확히 지정한 적이 없다면 모든 적 리스트에서 거리를 찾아서 제일 짧은 거리에 있는 적을 공격
	AFPSCharacter *shortestDistEnemy = nullptr;
	float shortestDist = fireRange;
	for (auto enemy : enemyPlayers)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "com");

		float dist = GetDistanceTo(enemy);

		if (dist <= shortestDist)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Shortest");

			shortestDistEnemy = enemy;
		}
	}

	target = shortestDistEnemy;
	if (target)
	{
		Server_Stop();
	}
}

void ADNG_RTSUnit::Chase()
{
	Server_Chase();
}

void ADNG_RTSUnit::Server_Chase_Implementation()
{
	if (bIsHold || !target) return;

	Server_Move(target->GetActorLocation());
}
