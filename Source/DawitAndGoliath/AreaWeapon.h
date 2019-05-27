// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestEnemyPawn.h"
#include "Weapon.h"
#include "AreaWeapon.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API UAreaWeapon : public UWeapon
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TArray<AActor*> Targets;
	UPROPERTY()
	UStaticMeshComponent* AreaMesh;
public:
	TArray<AActor*> GetTargets() const { return Targets; }
	UStaticMeshComponent* GetAreaMesh() const { return AreaMesh; }
	void AreaInit(FString name, float damage, float attackRate, float reloadTime, int maxBulletCount, USoundBase* fireSound,
		UStaticMeshComponent* areaMesh, UParticleSystem* fireParticle, UParticleSystem* muzzleFlame);
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};
