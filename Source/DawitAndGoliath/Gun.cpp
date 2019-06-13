// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UGun::GunInit(FString name, float damage, float attackRate, float range, float splitRange, float reloadTime, int maxBulletCount, USoundBase * fireSound)
{
	WeaponInit(name, damage, attackRate, reloadTime, maxBulletCount, fireSound);
	Range = range;
	SplitRange = splitRange;
}

FHitResult UGun::GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, AActor* ignore, float range)
{
	FCollisionQueryParams params(FName(TEXT("PlayerAimCheck")), true);
	params.bTraceAsyncScene = true;
	params.bReturnPhysicalMaterial = true;
	params.AddIgnoredActor(ignore);

	FHitResult hit(ForceInit);

	FVector start = loc;
	FVector end = loc + (rot.Vector() * range);

	if (world->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel1, params))
	{
		world->LineTraceSingleByChannel(
			hit,
			socLoc,
			end,
			ECC_GameTraceChannel1,
			params
		);
	}

	return hit;
}
