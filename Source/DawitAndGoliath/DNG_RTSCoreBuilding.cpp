// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSCoreBuilding.h"

void ADNG_RTSCoreBuilding::BeginPlay()
{
	Super::BeginPlay();

	if (objProperty)
	{
		objProperty->SetMaxHp(30000);
		objProperty->SetHp(30000);
	}
}