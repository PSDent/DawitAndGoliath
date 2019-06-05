// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSBarrack.h"
#include "Engine.h"
#include "DNG_RTSUnit.h"
#include "DNG_RTSUnit_Melee.h"
#include "DNG_RTSUnit_Tanker.h"
#include "DNG_RTSUnit_Range.h"
#include "DNG_RTSPawn.h"
#include "FPSCharacter.h"
#include "Net/UnrealNetwork.h"

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
	
	dele.BindUFunction(this, FName("SetRallyPointByRBClick"));
	commandInfoMap.Add(EKeys::RightMouseButton, FCommandInfo("SetRallyPointByRBClick", "", EKeys::RightMouseButton, -1, -1, dele));

	dele.BindUFunction(this, FName("CancleCurrentSpawn"));
	commandInfoMap.Add(EKeys::Escape, FCommandInfo("Cancle Producing", "Cancle Now Producing Unit", EKeys::Escape, 3, 3, dele));

	
	unitName = "Barrack";
	initial = "B";

	rallyPoint.Set(-390.0f, -470.0f, 380.0f);

	spawnTime = 0;
	spawnTotalTime = 0;
}

void ADNG_RTSBarrack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADNG_RTSBarrack, spawnQueue);
	DOREPLIFETIME(ADNG_RTSBarrack, spawnTime);
	DOREPLIFETIME(ADNG_RTSBarrack, spawnTotalTime);
}

void ADNG_RTSBarrack::BeginPlay()
{
	Super::BeginPlay();

	if (objProperty)
	{
		objProperty->SetMaxHp(12000);
		objProperty->SetHp(12000);
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

void ADNG_RTSBarrack::SetRallyPointByRBClick()
{
	Server_SetRallyPoint(pawn->targetPos);
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
		// SpawnActor로 스폰하는 경우 Controller가 자동으로 탑재 되있지 않는다
		// 이를 막기 위해 BP에서 Auto Posses AI 란을 Placed in World or Spawn으로 바꿔준다.

		ADNG_RTSUnit *spawnedUnit = GetWorld()->SpawnActor<ADNG_RTSUnit>(unitType, spawnPoint, FRotator::ZeroRotator, spawnInfo);
		spawnedUnit->SetPawn(pawn);
		spawnedUnit->Server_Move(rallyPoint, true);
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
		TSubclassOf<ADNG_RTSUnit> unitType;
		if (spawnQueue.Num())
		{
			unitType = spawnQueue[0];
			ADNG_RTSPawn *rtsPawn = Cast<ADNG_RTSPawn>(pawn);
			int nextSupply = rtsPawn->currentSupply + unitType.GetDefaultObject()->supply;
			int maxSupply = rtsPawn->maxSupply;
			bool isCanSpawn = nextSupply <= maxSupply ? true : false;
			if (!isCanSpawn) return;
		}

		if (!spawnQueue.Num()) return;

		spawnTime -= time;
		if (spawnTime <= 0)
		{
			unitType = spawnQueue[0]; // 뒤에있는걸 꺼낸다???
			spawnQueue.RemoveAt(0);
			SpawnUnit(unitType);

			if (spawnQueue.Num())
			{
				TSubclassOf<ADNG_RTSUnit> unitType;
				unitType = spawnQueue[0];
				spawnTime = unitType.GetDefaultObject()->spawnTime;
				spawnTotalTime = spawnTime;

				int deltaSupply = unitType.GetDefaultObject()->supply;
				Server_AddSupply(deltaSupply);
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
		if (spawnQueue.Num() == 5) return;

		for (int i = 0; i < spawnableUnits.Num(); ++i)
		{
			if (spawnableUnits[i].Get()->GetName().Contains(unitName))
			{
				if (!spawnQueue.Num())
				{
					spawnTime = spawnableUnits[i].GetDefaultObject()->spawnTime;
					spawnTotalTime = spawnTime;

					int deltaSupply = spawnableUnits[i].GetDefaultObject()->supply;
					Server_AddSupply(deltaSupply);
				}
				spawnQueue.Push(spawnableUnits[i]);

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
	RemoveQueueElement(0);	
}

void ADNG_RTSBarrack::RemoveQueueElement(int index)
{
	Server_RemoveQueueElement(index);
}

void ADNG_RTSBarrack::Server_RemoveQueueElement_Implementation(int index)
{
	if (spawnQueue.Num())
	{
		if (index == 0)
		{
			ADNG_RTSPawn *rtsPawn = Cast<ADNG_RTSPawn>(pawn);
			rtsPawn->currentSupply -= spawnQueue[0].GetDefaultObject()->supply;
		}

		spawnQueue.RemoveAt(index);
		if (spawnQueue.Num() && index == 0)
		{
			spawnTime = spawnableUnits[0].GetDefaultObject()->spawnTime;
			spawnTotalTime = spawnTime;
			Server_AddSupply(spawnQueue.Top().GetDefaultObject()->supply);
		}
	}
}

void ADNG_RTSBarrack::Server_AddSupply_Implementation(int deltaSupply)
{
	ADNG_RTSPawn *rtsPawn = Cast<ADNG_RTSPawn>(pawn);
	rtsPawn->currentSupply += deltaSupply;

}