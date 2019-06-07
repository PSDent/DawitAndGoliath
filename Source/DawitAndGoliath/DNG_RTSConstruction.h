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
	virtual void Die() override;
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void Server_ConstructionDie();
	void Server_ConstructionDie_Implementation();
	bool Server_ConstructionDie_Validate() { return true; }

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		virtual void Multicast_DestructionEffect();
	void Multicast_DestructionEffect_Implementation();
	bool Multicast_DestructionEffect_Validate() { return true; }

public:

private:
	FTimerHandle destructionTimer;
	FTimerDelegate destructionDele;


protected:
	float destructionTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS Construction")
		class UParticleSystem *destructionFX;
};
