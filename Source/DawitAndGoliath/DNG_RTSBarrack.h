// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNG_RTSConstruction.h"
#include "DNG_RTSBarrack.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSBarrack : public ADNG_RTSConstruction
{
	GENERATED_BODY()

public:
	ADNG_RTSBarrack();
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "RTS")
		void SpawnMeleeUnit();

	UFUNCTION(BlueprintCallable, Category = "RTS")
		void SpawnRangeUnit();

	UFUNCTION(BlueprintCallable, Category = "RTS")
		void SpawnTankerUnit();

	UFUNCTION(BlueprintCallable, Category = "RTS")
		void SetRallyPoint();
private:
	void SpawnUnit(TSubclassOf<class ADNG_RTSUnit> unit);

protected:

public:
	//UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	//	TArray<TSubclassOf<class ADNG_RTSUnit>> spawnableUnits;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TSubclassOf<class ADNG_RTSUnit_Melee> spawnableUnit;
private:
	FVector rallyPoint;

protected:

	
};
