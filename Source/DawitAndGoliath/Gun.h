// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "CollisionQueryParams.h"
#include "Gun.generated.h"

/**
 *
 */
UCLASS()
class DAWITANDGOLIATH_API UGun : public UWeapon
{
	GENERATED_BODY()

protected:
	float Range;
	float SplitRange;
	UParticleSystem* FireParticle;
	UParticleSystem* MuzzleFlame;

public:
	virtual AActor* GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, AActor* ignore) override;

	void GunInit(FString name, float damage, float attackRate, float range, float splitRange, USoundBase* fireSound);
	virtual void SetParticle(UParticleSystem* fireParticle, UParticleSystem* muzzleFlame);
	float GetSplitRange() { return SplitRange; }
};
