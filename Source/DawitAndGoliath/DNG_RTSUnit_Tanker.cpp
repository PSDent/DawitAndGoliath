// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSUnit_Tanker.h"
#include "DNGProperty.h"

ADNG_RTSUnit_Tanker::ADNG_RTSUnit_Tanker() : Super()
{
	FCommandDelegate dele;

	dele.BindUFunction(this, FName("Attack"));
	commandInfoMap.Add(EKeys::A, FCommandInfo("Attack", "Click Somewhere to Attack Enemy", EKeys::A, 1, 0, dele));

	objProperty->SetMaxHp(500.0f);
	objProperty->SetHp(500.0f);
	unitName = "TankerUnit";

	damage = 10.0f;
	fireRate = 1.0f;
	fireRange = 100.0f;

	ringDecal->DecalSize = FVector(32.f, 64.f, 64.f);
	
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	spawnTime = 15;
	supply = 6;

	initial = "T";
}

void ADNG_RTSUnit_Tanker::BeginPlay()
{
	Super::BeginPlay();
}

void ADNG_RTSUnit_Tanker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
