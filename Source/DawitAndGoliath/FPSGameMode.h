// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FPSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API AFPSGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	

protected:
	AFPSGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void StartPlay();

};
