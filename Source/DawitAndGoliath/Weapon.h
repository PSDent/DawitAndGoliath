// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"

class DAWITANDGOLIATH_API Weapon
{
protected:
	FString Name;
	float Damage;
	float AttackRate;
	USoundBase* FireSound;
	UClass* TargetClass;

public:
	//Weapon(float damage, float attackRate, USoundBase* FireSound);
	Weapon();

	Weapon(FString name, float damage, float attackRate, USoundBase* fireSound, UClass* targetClass);
	float GetDamage() { return Damage; }
	void SetDamage(float damage) { Damage = damage; }
	float GetAttackRate() { return AttackRate; }
	void SetAttackRate(float attackRate) { AttackRate = attackRate; }
	USoundBase* GetFireSound() { return FireSound; }
	FString GetName() { return Name; }
	UClass* GetTargetClass() { return TargetClass; }
	virtual AActor* GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, Weapon weapon);
};

class DAWITANDGOLIATH_API Gun : public Weapon
{
protected:
	float Range;
	float SplitRange;

public:
	Gun();

	Gun(FString name, float damage, float attackRate, float range, float splitRange, USoundBase* fireSound, UClass* targetClass);
	virtual AActor* GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, Weapon gun) override;
};