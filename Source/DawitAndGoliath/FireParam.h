// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireParam.generated.h"

UENUM()
enum class EWeaponType : uint8
{
	Rifle,
	MachineGun,
	FlameThrower
};


USTRUCT()
struct FFireParam
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		FVector Location;

	UPROPERTY()
		FRotator Rotation;

	UPROPERTY()
		FVector SocketLocation;

	UPROPERTY()
		UWorld* World;

	UPROPERTY()
		float Damage;

	UPROPERTY()
		float AttackRate;

	UPROPERTY()
		float Range;

	UPROPERTY()
		EWeaponType WeaponType;

	UPROPERTY()
		bool IsGun;
};