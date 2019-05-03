// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

void UGun::GunInit(FString name, float damage, float attackRate, float range, float splitRange, USoundBase * fireSound)
{
	WeaponInit(name, damage, attackRate, fireSound);
	Range = range;
	SplitRange = splitRange;
}

AActor* UGun::GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, AActor* ignore)
{
	FCollisionQueryParams params(FName(TEXT("PlayerAimCheck")), true);
	params.bTraceAsyncScene = true;
	params.bReturnPhysicalMaterial = true;
	params.AddIgnoredActor(ignore);

	FHitResult hit(ForceInit);

	FVector start = loc;
	FVector end = loc + (rot.Vector() * Range);

	DrawDebugLine(world, start, end, FColor::Red, false, 3.0f);

	if (world->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel1, params))
	{
		world->LineTraceSingleByChannel(
			hit,
			socLoc,
			end,
			ECC_GameTraceChannel1,
			params
		);

		if (!hit.GetActor())
		{
			DrawDebugLine(world, start, end, FColor::Red, false, 3.0f);
			return nullptr;
		}

		if (!hit.GetActor()->IsA(APawn::StaticClass()))
			DrawDebugLine(world, start, end, FColor::Red, false, 3.0f);
		else
			DrawDebugLine(world, socLoc, hit.Location, FColor::Yellow, false, 3.0f);

		UGameplayStatics::SpawnEmitterAtLocation(world, FireParticle, hit.Location, FRotator::ZeroRotator);
	}

	return hit.GetActor();
}

void UGun::SetParticle(UParticleSystem* fireParticle, UParticleSystem* muzzleFlame)
{
	FireParticle = fireParticle;
	MuzzleFlame = muzzleFlame;
}
