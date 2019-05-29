// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSBarrack.h"
#include "Engine.h"
#include "DNG_RTSUnit.h"
#include "DNG_RTSUnit_Melee.h"
#include "DNG_RTSUnit_Tanker.h"
#include "DNG_RTSUnit_Range.h"
#include "DNG_RTSPawn.h"
#include "FPSCharacter.h"

ADNG_RTSBarrack::ADNG_RTSBarrack() : Super()
{
	FCommandDelegate dele;
	bReplicates = true;

	dele.BindUFunction(this, FName("SpawnMeleeUnit"));
	commandInfoMap.Add(EKeys::M, FCommandInfo("Melee", "This Unit has good Balance.", EKeys::M, 0, 0, dele));

	dele.BindUFunction(this, FName("SpawnRangeUnit"));
	commandInfoMap.Add(EKeys::R, FCommandInfo("Range", "Fire your Enemys", EKeys::R, 0, 1, dele));

	dele.BindUFunction(this, FName("SpawnTankerUnit"));
	commandInfoMap.Add(EKeys::T, FCommandInfo("Tank", "If you looking for Tanker, Spawn this.", EKeys::T, 0, 2, dele));

	dele.BindUFunction(this, FName("SetRallyPoint"));
	commandInfoMap.Add(EKeys::Y, FCommandInfo("SetRallyPoint", "Set Units go to the Clicked Point", EKeys::Y, 1, 3, dele));
	unitName = "Barrack";


	rallyPoint.Set(-390.0f, -470.0f, 380.0f);
}

void ADNG_RTSBarrack::BeginPlay()
{
	Super::BeginPlay();

	if (objProperty)
	{
		objProperty->SetMaxHp(3000);
		objProperty->SetHp(3000);
	}

	spawnPoint = RootComponent->GetChildComponent(3)->GetComponentLocation();
}

void ADNG_RTSBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Spawning(DeltaTime);
	
}

void ADNG_RTSBarrack::SpawnMeleeUnit()
{
	AddSpawnQueue("Melee");
}

void ADNG_RTSBarrack::SpawnRangeUnit()
{
	AddSpawnQueue("Range");
}

void ADNG_RTSBarrack::SpawnTankerUnit()
{
	AddSpawnQueue("Tanker");
}

void ADNG_RTSBarrack::SetRallyPoint()
{
	pawn->SetCommandingFlag(true);

	pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Crosshairs;

	rallyDele.BindLambda(
		[&] {
		if (pawn->GetLeftMouseStatus() || pawn->GetRightMouseStatus())
		{
			Server_SetRallyPoint(pawn->targetPos);

			pawn->GetPlayerController()->CurrentMouseCursor = EMouseCursor::Default;

			GetWorld()->GetTimerManager().ClearTimer(rallyTimer);
		}
	}
	);
	GetWorld()->GetTimerManager().SetTimer(rallyTimer, rallyDele, 0.001f, true, 0.0f);
}

void ADNG_RTSBarrack::Server_SetRallyPoint_Implementation(FVector dest)
{
	rallyPoint = dest;
}


void ADNG_RTSBarrack::SpawnUnit(TSubclassOf<ADNG_RTSUnit> unitType)
{
	if (Role == ROLE_Authority) 
	{
		FActorSpawnParameters spawnInfo = FActorSpawnParameters();
		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnInfo.bNoFail = true;
		spawnInfo.Instigator = Instigator;
		// SpawnActor�� �����ϴ� ��� Controller�� �ڵ����� ž�� ������ �ʴ´�
		// �̸� ���� ���� BP���� Auto Posses AI ���� Placed in World or Spawn���� �ٲ��ش�.

		ADNG_RTSUnit *spawnedUnit = GetWorld()->SpawnActor<ADNG_RTSUnit>(unitType, spawnPoint, FRotator::ZeroRotator, spawnInfo);
		spawnedUnit->Server_Move(rallyPoint);
	}
	else
	{
		Server_SpawnUnit(unitType);
	}
}

void ADNG_RTSBarrack::Server_SpawnUnit_Implementation(TSubclassOf<ADNG_RTSUnit> unitType)
{
	SpawnUnit(unitType);
}

bool ADNG_RTSBarrack::Server_SpawnUnit_Validate(TSubclassOf<ADNG_RTSUnit> unitType)
{
	return true;
}

void ADNG_RTSBarrack::Spawning(float time)
{
	if (Role == ROLE_Authority)
	{
		if (spawnQueue.IsEmpty()) return;

		spawnTime -= time;
		if (spawnTime <= 0)
		{
			TSubclassOf<ADNG_RTSUnit> unitType;
			spawnQueue.Dequeue(unitType);
			SpawnUnit(unitType);
			--queueNum;

			if (!spawnQueue.IsEmpty())
			{
				TSubclassOf<ADNG_RTSUnit> unitType;
				spawnQueue.Peek(unitType);
				spawnTime = unitType.GetDefaultObject()->spawnTime;
			}
		}
	}
	else
	{
		Server_Spawning(time);
	}
}

void ADNG_RTSBarrack::Server_Spawning_Implementation(float time)
{
	Spawning(time);
}
void ADNG_RTSBarrack::AddSpawnQueue(const FString &unitName)
{
	if (Role == ROLE_Authority)
	{
		if (queueNum == 5) return;

		for (int i = 0; i < spawnableUnits.Num(); ++i)
		{
			if (spawnableUnits[i].Get()->GetName().Contains(unitName))
			{
				if (spawnQueue.IsEmpty())
				{
					spawnTime = spawnableUnits[i].GetDefaultObject()->spawnTime;
				}
				++queueNum;
				spawnQueue.Enqueue(spawnableUnits[i]);
				break;
			}
		}
	}
	else
	{
		Server_AddSpawnQueue(unitName);
	}
}

void ADNG_RTSBarrack::Server_AddSpawnQueue_Implementation(const FString &unitName)
{
	AddSpawnQueue(unitName);
}


void ADNG_RTSBarrack::CancleCurrentSpawn()
{

}