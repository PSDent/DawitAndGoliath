// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNG_RTSBaseObject.h"
#include "DNG_RTSUnit.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSUnit : public ADNG_RTSBaseObject
{
	GENERATED_BODY()
	
public:
	ADNG_RTSUnit();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Command
	UFUNCTION()
		void Move();
	UFUNCTION()
		void Stop();
	UFUNCTION()
		void Hold();
	UFUNCTION()
		void Patrol();

private:


protected:
	AActor *target;


public:

private:

protected:

	float damage;

	bool bIsHold;

};
