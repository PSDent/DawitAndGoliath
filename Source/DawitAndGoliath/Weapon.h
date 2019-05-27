// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "CollisionQueryParams.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API UWeapon : public UObject
{
	GENERATED_BODY()
public:
	UWeapon();
	~UWeapon();
protected:
	UPROPERTY()
		FString Name;
	UPROPERTY()
		float Range;
	UPROPERTY()
		float Damage;
	UPROPERTY()
		float AttackRate;
	USoundBase* FireSound;
	UPROPERTY()
		UParticleSystem* FireParticle;
	UPROPERTY()
		UParticleSystem* MuzzleFlame;
	UPROPERTY()
		int CurrentBulletCount;
	UPROPERTY()
		int MaxBulletCount;
	UPROPERTY()
		float ReloadTime;

public:
	virtual void WeaponInit(FString name, float damage, float attackRate, float reloadTime, int MaxBulletCount, USoundBase* fireSound);
	float GetDamage() { return Damage; }
	void SetDamage(float damage) { Damage = damage; }
	virtual float GetRange() { return Range; }
	float GetAttackRate() { return AttackRate; }
	void SetAttackRate(float attackRate) { AttackRate = attackRate; }
	float GetReloadTime() { return ReloadTime; }
	int GetBulletCount() { return CurrentBulletCount; }
	int GetMaxBulletCount() { return MaxBulletCount; }
	int UseBullet()
	{
		return --CurrentBulletCount;
	}
	void Reload()
	{
		CurrentBulletCount = MaxBulletCount;
	}
	USoundBase* GetFireSound() { return FireSound; }
	FString GetName() { return Name; }
	virtual FHitResult GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, AActor* ignore, float range);
	virtual void SetParticle(UParticleSystem* fireParticle, UParticleSystem* muzzleFlame);
};
