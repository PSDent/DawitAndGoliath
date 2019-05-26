// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNG_RTSUnit.h"
#include "DNG_RTSUnit_Tanker.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSUnit_Tanker : public ADNG_RTSUnit
{
	GENERATED_BODY()
public:
	ADNG_RTSUnit_Tanker();
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

private:

protected:

public:

private:

protected:


};
