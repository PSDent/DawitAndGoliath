// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSUnit_Melee.h"
#include "Engine.h"
#include "DNGProperty.h"
#include "DNG_RTSPawn.h"
#include "FPSCharacter.h"

ADNG_RTSUnit_Melee::ADNG_RTSUnit_Melee() : Super()
{
	FCommandDelegate dele;

	dele.BindUFunction(this, FName("Attack"));
	commandInfoMap.Add(EKeys::A, FCommandInfo("Attack", "Attack Enemy to Click Somewhere", EKeys::A, 1, 0, dele));
	
	objProperty->SetMaxHp(50.0f);
	objProperty->SetHp(50.0f);
	unitName = "MeleeUnit";

	damage = 50.0f;

}

void ADNG_RTSUnit_Melee::BeginPlay()
{
	Super::BeginPlay();
}

void ADNG_RTSUnit_Melee::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	if (objProperty->GetHp() <= 0.0f)
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "DIEEEEEEEEEEE");

		//OnDied();
}

//void ADNG_RTSUnit_Melee::OnDied()
//{
//	GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, "DIEEEEEEEEEEE");
//}