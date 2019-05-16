// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNG_RTSUnit.h"
#include "DNG_RTSUnit_Melee.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSUnit_Melee : public ADNG_RTSUnit
{
	GENERATED_BODY()

public:
	ADNG_RTSUnit_Melee();
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

public:
	UFUNCTION()
		void Attack();

private:

protected:
	
};
