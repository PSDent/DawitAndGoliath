// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNG_RTSBaseObject.h"
#include "DNG_RTSConstruction.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSConstruction : public ADNG_RTSBaseObject
{
	GENERATED_BODY()

public:
	ADNG_RTSConstruction();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

private:

protected:


public:

private:
	
protected:

	
};
