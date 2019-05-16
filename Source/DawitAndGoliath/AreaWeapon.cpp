// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaWeapon.h"

void UAreaWeapon::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor->IsA(ATestEnemyPawn::StaticClass()))
	{
		Targets.Add(OtherActor);
	}
}

void UAreaWeapon::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor->IsA(ATestEnemyPawn::StaticClass()))
	{
		Targets.Remove(OtherActor);
	}
}

void UAreaWeapon::AreaInit(FString name, float damage, float attackRate, float reloadTime, int maxBulletCount, USoundBase* fireSound,
	UStaticMeshComponent* areaMesh, UParticleSystem* fireParticle, UParticleSystem* muzzleFlame)
{
	WeaponInit(name, damage, attackRate, reloadTime, maxBulletCount, fireSound);
	AreaMesh = areaMesh;
	FireParticle = fireParticle;
	MuzzleFlame = muzzleFlame;

}