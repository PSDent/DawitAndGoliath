// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

/**
 *
 */
UCLASS()
class DAWITANDGOLIATH_API UGun : public UWeapon
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	float SplitRange;

public:
	static FHitResult GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, AActor* ignore, float range);

	void GunInit(FString name, float damage, float attackRate, float range, float splitRange, float reloadTime, int maxBulletCount, USoundBase* fireSound);
	virtual float GetSplitRange() { return SplitRange; }
};
