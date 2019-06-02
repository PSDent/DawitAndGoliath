// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSUnit_Range.h"
#include "DNGProperty.h"

ADNG_RTSUnit_Range::ADNG_RTSUnit_Range() : Super()
{
	FCommandDelegate dele;

	dele.BindUFunction(this, FName("Attack"));
	commandInfoMap.Add(EKeys::A, FCommandInfo("Attack", "Click Somewhere to Attack Enemy", EKeys::A, 1, 0, dele));

	objProperty->SetMaxHp(50.0f);
	objProperty->SetHp(50.0f);
	unitName = "RangeUnit";

	damage = 1.0f;
	fireRate = 1.0f;
	fireRange = 600.0f;
	traceRange = 1000.0f;

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	spawnTime = 5;
	supply = 3;

	initial = "R";
}

void ADNG_RTSUnit_Range::BeginPlay()
{
	Super::BeginPlay();

	muzzle = GetMesh()->GetSocketByName("Muzzle_Front");
}

void ADNG_RTSUnit_Range::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	fireEffectPos = GetMesh()->GetSocketLocation("Muzzle_Front");
}
