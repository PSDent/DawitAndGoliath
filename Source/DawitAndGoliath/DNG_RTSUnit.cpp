
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
	arriveTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("arriveTrigger"));

	//patrolPointTriggerOne->AttachTo(RootComponent);
	//patrolPointTriggerTwo->AttachTo(RootComponent);
	//arriveTrigger->AttachTo(RootComponent);

	//arriveTrigger->SetRelativeLocation(FVector(0, 0, 0));

	patrolPointTriggerOne->SetSphereRadius(30.0f);
	patrolPointTriggerTwo->SetSphereRadius(30.0f);
	arriveTrigger->SetSphereRadius(30.0f);
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
	DOREPLIFETIME(ADNG_RTSUnit, blackBoard);
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
	blackBoard->SetValueAsBool(key_IsAlive, true);
	blackBoard->SetValueAsBool(key_IsPatrolling, false);

	//arriveTriggerRad = 20.0f;
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
		Server_Die();

	if (target && !bIsWalk)
		TurnToTarget();
}

void ADNG_RTSUnit::Server_CheckStopped_Implementation()
{
	bool isJustMoving = blackBoard->GetValueAsBool(key_IsJustMoving);
	bool isPatrolling = blackBoard->GetValueAsBool(key_IsPatrolling);

	if (!isJustMoving || isPatrolling) return;

	TArray<AActor*> actors;
	arriveTrigger->GetOverlappingActors(actors);

	AActor *me = Cast<AActor>(this);
	for (auto actor : actors)
	{
		if (actor == me)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "CheckStopped");

			Server_SetValueBool(key_IsJustMoving, false);
			return;
		}
	}
}

void ADNG_RTSUnit::TurnToTarget()
{
	if (Role == ROLE_Authority)
	{
		if (target)
		{
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

void ADNG_RTSUnit::Die()
{
	if (Role == ROLE_Authority)
	{
		Super::Die();

		bIsDie = true;
		bIsAlive = false;
		bIsSelected = false;
		blackBoard->SetValueAsBool(key_IsPatrolling, false);
		blackBoard->SetValueAsBool(key_IsWantToDeal, false);
		blackBoard->SetValueAsBool(key_IsChasing, false);
		blackBoard->SetValueAsBool(key_IsCanDeal, false);
		blackBoard->SetValueAsBool(key_IsAlive, false);
		aiController->StopMovement();

		Cast<UCapsuleComponent>(RootComponent)->SetCollisionProfileName("NoCollision");
		GetMesh()->SetCollisionProfileName("NoCollision");

		pawn->currentSupply -= supply;
	}
	else
	{
		Server_UnitDie();
	}
}

void ADNG_RTSUnit::Server_UnitDie_Implementation()
{
	Die();
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

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "MoveCLICK");

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

			Server_SetValueBool(key_IsPatrolling, false);
			Server_Move(pawn->targetPos, true);

			pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Default;

			GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
		}
	}
	);
	GetWorld()->GetTimerManager().SetTimer(commandCheckHandle, commandCheckDele, 0.001f, true, 0.0f);
}

void ADNG_RTSUnit::Server_Move_Implementation(FVector dest, bool justMoveVal)
{
	if (!bIsAlive) return;

	target = nullptr;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "MoveMove");

	blackBoard->SetValueAsBool(key_IsJustMoving, justMoveVal);
	blackBoard->SetValueAsBool(key_IsChasing, false);
	blackBoard->SetValueAsBool(key_IsWantToDeal, false);
	aiController->MoveToLocation(dest);

	arriveTrigger->SetWorldLocation(dest); // 요놈이 문제였다
	DrawDebugSphere(GetWorld(), dest, 64.0f, 16, FColor::Blue, false, 3.0f);
}

void ADNG_RTSUnit::Stop()
{
	Server_Stop();
}

void ADNG_RTSUnit::Server_SetValueBool_Implementation(FName key, bool val)
{
	blackBoard->SetValueAsBool(key, val);
}

void ADNG_RTSUnit::Server_Stop_Implementation()
{
	//GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
	aiController->StopMovement();
	blackBoard->SetValueAsBool(key_IsPatrolling, false);
	blackBoard->SetValueAsBool(key_IsJustMoving, false);
}


void ADNG_RTSUnit::Hold()
{
	Server_Hold();
}

void ADNG_RTSUnit::Server_Hold_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(commandCheckHandle);
	blackBoard->SetValueAsBool(key_IsPatrolling, false);
	blackBoard->SetValueAsBool(key_IsJustMoving, false);

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
	patrolPointOne = posOne;
	patrolPointTwo = posTwo;
	nextPatrolPoint = posTwo;

	patrolPointTriggerOne->SetWorldLocation(patrolPointOne);
	patrolPointTriggerTwo->SetWorldLocation(patrolPointTwo);
	nextPatrolPointTrigger = patrolPointTriggerTwo;

	Server_Move(nextPatrolPoint, false);
	blackBoard->SetValueAsBool(key_IsPatrolling, true);
}

void ADNG_RTSUnit::Server_CheckPatrol_Implementation()
{
	static bool test = false;
	if (!blackBoard->GetValueAsBool(key_IsPatrolling) || test) return;

	TArray<AActor*> objects;
	//nextPatrolPointTrigger->SetSphereRadius(32.0f);		

	//nextPatrolPointTrigger->GetOverlappingActors(objects, ADNG_RTSUnit::StaticClass());
	//DrawDebugSphere(GetWorld(), arriveTrigger->GetComponentLocation(), 32.0f, 16, FColor::White, false, 1.0f);

	nextPatrolPointTrigger->SetSphereRadius(16.0f);
	nextPatrolPointTrigger->SetWorldLocation(nextPatrolPoint);
	nextPatrolPointTrigger->GetOverlappingActors(objects, ADNG_RTSUnit::StaticClass());

	DrawDebugSphere(GetWorld(), nextPatrolPointTrigger->GetComponentLocation(), 512.0f, 16, FColor::Red, false, 1.0f);
	//상대위치 관련있을지도
	DrawDebugSphere(GetWorld(), arriveTrigger->GetComponentLocation(), 64.0f, 16, FColor::Cyan, false, 0.1f);

	if (objects.Find(this) != INDEX_NONE)
	{

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Reached");

		if (nextPatrolPoint == patrolPointOne)
		{
			nextPatrolPoint = patrolPointTwo;
			nextPatrolPointTrigger = patrolPointTriggerTwo;
		}
		else if(nextPatrolPoint == patrolPointTwo)
		{
			nextPatrolPoint = patrolPointOne;
			nextPatrolPointTrigger = patrolPointTriggerOne;
		}

		Server_Move(nextPatrolPoint, false);
	}
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
				Server_Move(pawn->targetPos, false);
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
	blackBoard->SetValueAsBool(key_IsPatrolling, false);
	blackBoard->SetValueAsBool(key_IsJustMoving, false);

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

	blackBoard->SetValueAsBool(key_IsPatrolling, false);

	bool isInRange = GetDistanceTo(target) <= fireRange ? true : false;
	bool isCoolTime = blackBoard->GetValueAsBool(key_IsCanDeal);
	bool isMe = target == Cast<AActor>(this);
	bool isMoving = GetCharacterMovement()->Velocity.Size2D() != 0;
	bool isJustMoving = blackBoard->GetValueAsBool(key_IsJustMoving);

	if (!isInRange || !isCoolTime || isMe || isMoving || isJustMoving || !bIsAlive)
	{
		return;
	}
	
	if (bIsWalk)
	{
		Server_Stop();
	}

	Multicast_FireEffect(target->GetActorLocation());
	TurnToTarget();

	blackBoard->SetValueAsBool(key_IsCanDeal, false);
	Multicast_GiveDamage(target, damage);

	attackDelayDele.BindLambda(
		[&]()
	{
		blackBoard->SetValueAsBool(key_IsCanDeal, true);
	}
	);
	GetWorld()->GetTimerManager().SetTimer(attackDelayTimerHandle, attackDelayDele, 0.01f, false, fireRate);
}

void ADNG_RTSUnit::Multicast_GiveDamage_Implementation(AActor *target, float damage)
{
	TArray<UActorComponent*> arr;
	target->GetComponents(arr);

	for (UActorComponent* c : arr)
		if (c->IsA(UDNGProperty::StaticClass()))
		{
			UDNGProperty* prop = Cast<UDNGProperty>(c);
			if (prop)
				prop->DealDamage(damage);
			return;
		}
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
	Server_CheckStopped();
}

void ADNG_RTSUnit::CompareDistance()
{
	Server_CompareDistance();
}

void ADNG_RTSUnit::Server_CompareDistance_Implementation()
{
	bool isJustMoving = blackBoard->GetValueAsBool(key_IsJustMoving);
	if (isJustMoving) return;

	ADNG_RTSBaseObject *obj = Cast<ADNG_RTSBaseObject>(target);
	if (obj)
	{
		if (obj->objProperty->GetHp() <= 0.0f)
			target = nullptr;
	}

	AFPSCharacter *fpsObj = Cast<AFPSCharacter>(target);
	if (fpsObj)
	{
		if (fpsObj->GetHp() <= 0.0f)
			target = nullptr;
	}

	if (target)
	{
		bool isMoving = GetCharacterMovement()->Velocity.Size2D() > 0;
		bool isDealing = blackBoard->GetValueAsBool(key_IsWantToDeal);
		bool isChasing = blackBoard->GetValueAsBool(key_IsChasing);

		if ((isMoving && isDealing && !isChasing))
		{
			target = nullptr;
			return;
		}

		float dist = GetDistanceTo(target);

		if (dist <= fireRange && !isJustMoving)
		{
			blackBoard->SetValueAsBool(key_IsWantToDeal, true);
			blackBoard->SetValueAsBool(key_IsChasing, false);
			Server_Stop(); 
			return;
		}
		else if ((dist <= traceRange && !isJustMoving) || bIsMarkTarget)
		{
			blackBoard->SetValueAsBool(key_IsWantToDeal, false);
			blackBoard->SetValueAsBool(key_IsChasing, true);
			return;
		}
		else
		{
			blackBoard->SetValueAsBool(key_IsWantToDeal, false);
			blackBoard->SetValueAsBool(key_IsChasing, false);
		}
	}

	// 정확히 지정한 적이 없다면 모든 적 리스트에서 거리를 찾아서 제일 짧은 거리에 있는 적을 공격
	AFPSCharacter *shortestDistEnemy = nullptr;
	float shortestDist = traceRange;
	for (auto enemy : enemyPlayers)
	{
		float dist = GetDistanceTo(enemy);

		if (dist <= shortestDist)
			shortestDistEnemy = enemy;
	}
	target = shortestDistEnemy;
}

void ADNG_RTSUnit::Chase()
{
	Server_Chase();
}

void ADNG_RTSUnit::Server_Chase_Implementation()
{
	bool isJustMoving = blackBoard->GetValueAsBool(key_IsJustMoving);

	if (bIsHold || !target || isJustMoving) return;

	Server_Move(target->GetActorLocation(), false);
}