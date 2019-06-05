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

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;


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

	UFUNCTION(BlueprintCallable, Category = "RTS")
		void SetRallyPointByRBClick();

	// Getter
	TArray<TSubclassOf<ADNG_RTSUnit>>& GetSpawnQueue() { return spawnQueue; }
	void RemoveQueueElement(int index);
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_RemoveQueueElement(int index);
	void Server_RemoveQueueElement_Implementation(int index);
	bool Server_RemoveQueueElement_Validate(int index) { return true; }

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

	UFUNCTION(Reliable, Server, WithValidation)
		virtual void Server_AddSupply(int deltaSupply);
	void Server_AddSupply_Implementation(int deltaSupply);
	bool Server_AddSupply_Validate(int deltaSupply) { return true; }


public:


protected:

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<TSubclassOf<class ADNG_RTSUnit>> spawnableUnits;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
		float spawnTime;

	UPROPERTY(BlueprintReadOnly, Replicated)
		float spawnTotalTime;

private: 
	// Spawn Queue
	UPROPERTY(Replicated)
		TArray<TSubclassOf<ADNG_RTSUnit>> spawnQueue;

	FTimerDelegate spawningDele;
	FTimerHandle spawningTimer;

	// Rally Point
	FTimerDelegate rallyDele;
	FTimerHandle rallyTimer;
	FVector rallyPoint;
	FVector spawnPoint;

protected:

	
};
