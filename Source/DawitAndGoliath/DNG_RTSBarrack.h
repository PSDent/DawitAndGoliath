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

	UFUNCTION(BlueprintCallable, Category = "RTS")
		void CancleCurrentSpawn();

private:
	void SpawnUnit(TSubclassOf<class ADNG_RTSUnit> unitType);
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void Server_SpawnUnit(TSubclassOf<class ADNG_RTSUnit> unitType);
	void Server_SpawnUnit_Implementation(TSubclassOf<class ADNG_RTSUnit> unitType);
	bool Server_SpawnUnit_Validate(TSubclassOf<class ADNG_RTSUnit> unitType);

	UFUNCTION(Reliable, Server, WithValidation)
		virtual void Server_SetRallyPoint(FVector dest);
	void Server_SetRallyPoint_Implementation(FVector dest);
	bool Server_SetRallyPoint_Validate(FVector dest) { return true; }

	void Spawning(float time);
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void Server_Spawning(float time);
	void Server_Spawning_Implementation(float time);
	bool Server_Spawning_Validate(float time) { return true; };

	void AddSpawnQueue(const FString &unitName);
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void Server_AddSpawnQueue(const FString &unitName);
	void Server_AddSpawnQueue_Implementation(const FString &unitName);
	bool Server_AddSpawnQueue_Validate(const FString &unitName) { return true; }

public:


protected:

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<TSubclassOf<class ADNG_RTSUnit>> spawnableUnits;
	
	UPROPERTY(BlueprintReadOnly)
		float spawnTime = 0;

private: 
	// Spawn Queue
	TQueue<TSubclassOf<ADNG_RTSUnit>> spawnQueue;

	FTimerDelegate spawningDele;
	FTimerHandle spawningTimer;

	// Rally Point
	FTimerDelegate rallyDele;
	FTimerHandle rallyTimer;
	FVector rallyPoint;
	FVector spawnPoint;

protected:

	
};
