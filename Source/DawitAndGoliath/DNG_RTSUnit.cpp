
// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSUnit.h"
#include "DNG_RTSPawn.h"
#include "FPSCharacter.h"
#include "DNGProperty.h"
#include "Net/UnrealNetwork.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"

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

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TSubclassOf<AFPSCharacter>(), actors);

	for (auto actor : actors)
	{
		AFPSCharacter *enemy = Cast<AFPSCharacter>(actor);
		enemyPlayers.Add(enemy);
	}

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
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, "Has Target");

			FRotator deltaRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), target->GetActorLocation());
			deltaRot.Pitch = 0.0f;
			SetActorRotation(deltaRot);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, "No Target");

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
			//Server_Attack();
			if (pawn->targetActor)
			{
				Server_Attack(pawn->targetActor);
				//target = pawn->targetActor;
				//GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Attack Target");
				//Deal(target);
			}
			else
			{
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
	Deal(target);
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
	bool isMe = targetActor == Cast<AActor>(this);

	if (!isInRange)
	{
	}

	if (!isInRange || !isCoolTime || isMe) return;

	if (bIsWalk)
	{
		Server_Stop();
	}

	if (target)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "TARGET O");

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "TARGET X");

	}

	Multicast_FireEffect(targetActor->GetActorLocation());
	TurnToTarget();

	blackBoard->SetValueAsBool(key_IsCanDeal, false);
	GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "Attack");
	GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, targetActor->GetName());

	if (targetActor->IsA(AFPSCharacter::StaticClass()))
	{
		Cast<AFPSCharacter>(targetActor)->Prop->DealDamage(damage);
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