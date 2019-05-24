// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNG_RTSConstruction.h"
#include "DNG_RTSCoreBuilding.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSCoreBuilding : public ADNG_RTSConstruction
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
};
