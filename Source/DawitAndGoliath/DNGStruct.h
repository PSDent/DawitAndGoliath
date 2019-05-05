// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNGStruct.generated.h"

USTRUCT()
struct FSessionPlayersInfo
{
	GENERATED_BODY()

public:
	TMap<FString, FString> playersRole_TMap;

};

USTRUCT()
struct FCommandShortcut
{
	GENERATED_BODY()

public:


};