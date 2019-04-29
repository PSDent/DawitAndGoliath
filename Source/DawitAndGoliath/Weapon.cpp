// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Engine.h"

Weapon::Weapon()
{
}

Gun::Gun()
{

}

AActor* Weapon::GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, Weapon weapon)
{
	return nullptr;
}

AActor* Gun::GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, Weapon gun)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Raycast"));
	FCollisionQueryParams params(FName(TEXT("PlayerAimCheck")), true);
	params.bTraceAsyncScene = true;
	params.bReturnPhysicalMaterial = true;

	FHitResult hit(ForceInit);

	FVector start = loc;
	FVector end = loc + (rot.Vector() * Cast<Gun>(gun).Range);

	if (world->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params))
	{
		world->LineTraceSingleByChannel(
			hit,
			socLoc,
			end,
			ECC_Visibility,
			params
		);

		if (!hit.GetActor()) return nullptr;

		if (!hit.GetActor()->IsA(TargetClass))
			DrawDebugLine(world, start, end, FColor::Red, false, 3.0f);
		else
			DrawDebugLine(world, socLoc, hit.Location, FColor::Yellow, false, 3.0f);

		//UGameplayStatics::SpawnEmitterAtLocation(world, FireParticle, hit.Location, FRotator::ZeroRotator);
	}

	////FRotator socRot = rot.Add(0, 90, 0);
	//UGameplayStatics::SpawnEmitterAtLocation(world, MuzzleFlame,
	//	soc.GetLocation(), rot);

	return hit.GetActor();
}
