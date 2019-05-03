// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

UWeapon::UWeapon()
{
}

UWeapon::~UWeapon()
{
}

void UWeapon::WeaponInit(FString name, float damage, float attackRate, USoundBase* fireSound)
{
	Name = name;
	Damage = damage;
	AttackRate = attackRate;
	FireSound = fireSound;
}

AActor* UWeapon::GetTarget(FVector loc, FVector socLoc, FRotator rot, UWorld* world, AActor* ignore)
{
	return nullptr;
}