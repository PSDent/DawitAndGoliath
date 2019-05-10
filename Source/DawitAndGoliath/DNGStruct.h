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

USTRUCT(BlueprintType)
struct FCommandInfo
{
	GENERATED_BODY()

public:
	FCommandInfo()
	{
		name = "None";
		description = "None";
		shortCut = EKeys::Escape;
		row = -1;
		column = -1;
	}

	// 명령과 UI의 명령 버튼과 연동시켜서 보여줄 것.
	// 보여줄 명령 패널의 명령들은 수가 가장 많은 동일 종류의 유닛 명령들을 표시

	FCommandInfo(FString name, FString description, FKey shortCut, int row, int column)
	{
		this->name = name;
		this->description = description;
		this->shortCut = shortCut;
		this->row = row;
		this->column = column;           
	}

public:
	FString name;
	FString description;
	FKey shortCut;
	int row;
	int column;
};