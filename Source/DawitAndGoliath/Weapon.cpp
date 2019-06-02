// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "UnrealNetwork.h"

UWeapon::UWeapon()
{
}

UWeapon::~UWeapon()
{
}

void UWeapon::WeaponInit(FString name, float damage, float attackRate, float reloadTime, int maxBulletCount, USoundBase* fireSound)
{
	Name = name;
	Damage = damage;
	AttackRate = attackRate;
	MaxBulletCount = CurrentBulletCount = maxBulletCount;
	ReloadTime = reloadTime;
	FireSound = fireSound;
}

//void FWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(FWeapon, bReplicatedFlag);
//}


void UWeapon::SetParticle(UParticleSystem* fireParticle, UParticleSystem* muzzleFlame)
{
	FireParticle = fireParticle;
	MuzzleFlame = muzzleFlame;
}