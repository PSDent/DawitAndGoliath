// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSBarrack.h"
#include "Engine.h"
#include "DNG_RTSUnit.h"
#include "DNG_RTSUnit_Melee.h"
#include "DNG_RTSUnit_Tanker.h"
#include "DNG_RTSUnit_Range.h"

ADNG_RTSBarrack::ADNG_RTSBarrack() : Super()
{
	FCommandDelegate dele;

	dele.BindUFunction(this, FName("SpawnMeleeUnit"));
	commandInfoMap.Add(EKeys::M, FCommandInfo("Melee", "This Unit has good Balance.", EKeys::M, 0, 0, dele));

	dele.BindUFunction(this, FName("SpawnRangeUnit"));
	commandInfoMap.Add(EKeys::R, FCommandInfo("Range", "Fire your Enemys", EKeys::R, 0, 1, dele));

	dele.BindUFunction(this, FName("SpawnTankerUnit"));
	commandInfoMap.Add(EKeys::T, FCommandInfo("Tank", "If you looking for Tanker, Spawn this.", EKeys::T, 0, 2, dele));

	dele.BindUFunction(this, FName("SetRallyPoint"));
	commandInfoMap.Add(EKeys::Y, FCommandInfo("SetRallyPoint", "Set Units go to the Clicked Point", EKeys::Y, 1, 3, dele));

	rallyPoint.Set(-290.0f, -470.0f, 380.0f);
}

void ADNG_RTSBarrack::BeginPlay()
{
	Super::BeginPlay();

	if (objProperty)
	{
		objProperty->SetMaxHp(3000);
		objProperty->SetHp(3000);
	}
}

void ADNG_RTSBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ADNG_RTSBarrack::SpawnMeleeUnit()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Spawn Melee Unit");

	//for (int i = 0; i < spawnableUnits.Num(); ++i)
	//{
	//	if (spawnableUnits[i].Get()->GetName().Find("Melee"))
	//	{
	//		SpawnUnit(spawnableUnits[i]);
	//		break;
	//	}
	//}

	SpawnUnit(spawnableUnit);
}

void ADNG_RTSBarrack::SpawnRangeUnit()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Spawn Range Unit");

}

void ADNG_RTSBarrack::SpawnTankerUnit()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Spawn Tanker Unit");

}

void ADNG_RTSBarrack::SetRallyPoint()
{

}

void ADNG_RTSBarrack::SpawnUnit(TSubclassOf<ADNG_RTSUnit> unit)
{
	FActorSpawnParameters spawnInfo;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Spawn Unit");
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, unit.Get()->GetName());

	DrawDebugSphere(GetWorld(), rallyPoint, 32.0f, 16, FColor::Green, false, 4.0f);
	// 요기 스폰이 좀 이상하다 고치래이

	//if(GetWorld())
	GetWorld()->SpawnActor<ADNG_RTSUnit_Melee>(spawnableUnit, rallyPoint, FRotator::ZeroRotator, spawnInfo);
	//GetWorld()->SpawnActor<ADNG_RTSUnit_Melee>(ADNG_RTSUnit_Melee::StaticClass(), rallyPoint, FRotator::ZeroRotator, spawnInfo);
	//GetWorld()->SpawnActor<ADNG_RTSUnit_Melee>(unit, spawnInfo);
}