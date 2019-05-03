// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
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
	FString Name;
	float Damage;
	float AttackRate;
	USoundBase* FireSound;
public:
	virtual void WeaponInit(FString name, float damage, float attackRate, USoundBase* fireSound);
	float GetDamage() { return Damage; }
	void SetDamage(float damage) { Damage = damage; }
	float GetAttackRate() { return AttackRate; }
	void SetAttackRate(float attackRate) { AttackRate = attackRate; }
	USoundBase* GetFireSound() { return FireSound; }
	FString GetName() { return Name; }
	virtual AActor* GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, AActor* ignore);

};
