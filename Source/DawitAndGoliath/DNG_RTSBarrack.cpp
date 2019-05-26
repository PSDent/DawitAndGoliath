// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSBarrack.h"
#include "Engine.h"
#include "DNG_RTSUnit.h"
#include "DNG_RTSUnit_Melee.h"
#include "DNG_RTSUnit_Tanker.h"
#include "DNG_RTSUnit_Range.h"
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
}

void ADNG_RTSBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADNG_RTSBarrack::SpawnMeleeUnit()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Spawn Melee Unit");
	SpawnUnit("Melee");
}

void ADNG_RTSBarrack::SpawnRangeUnit()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Spawn Range Unit");
	SpawnUnit("Range");
}

void ADNG_RTSBarrack::SpawnTankerUnit()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Spawn Tanker Unit");
	SpawnUnit("Tanker");
}

void ADNG_RTSBarrack::SetRallyPoint()
{

}

void ADNG_RTSBarrack::SpawnUnit(const FString &unitName)
{
	if (Role == ROLE_Authority) 
	{
		TSubclassOf<ADNG_RTSUnit> unit;

		for (int i = 0; i < spawnableUnits.Num(); ++i)
		{
			if (spawnableUnits[i].Get()->GetName().Contains(unitName))
			{
				unit = spawnableUnits[i];
				break;
			}
		}

		FActorSpawnParameters spawnInfo = FActorSpawnParameters();
		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnInfo.bNoFail = true;
		spawnInfo.Instigator = Instigator;

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, rallyPoint.ToString());
		// SpawnActor로 스폰하는 경우 Controller가 자동으로 탑재 되있지 않는다
		// 이를 막기 위해 BP에서 Auto Posses AI 란을 Placed in World or Spawn으로 바꿔준다.
		rallyPoint.Set(-390.0f, -470.0f, 380.0f);

		GetWorld()->SpawnActor<ADNG_RTSUnit>(unit, rallyPoint, FRotator::ZeroRotator, spawnInfo);
		DrawDebugSphere(GetWorld(), rallyPoint, 64.0f, 16, FColor::Red, false, 3.0f);
	}
	else
	{
		Server_SpawnUnit(unitName);
	}
}

void ADNG_RTSBarrack::Server_SpawnUnit_Implementation(const FString &unitName)
{
	SpawnUnit(unitName);
}

bool ADNG_RTSBarrack::Server_SpawnUnit_Validate(const FString &unitName)
{
	return true;
}