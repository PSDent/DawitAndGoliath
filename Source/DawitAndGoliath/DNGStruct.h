// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNGDelegates.h"
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

	// ��ɰ� UI�� ��� ��ư�� �������Ѽ� ������ ��.
	// ������ ��� �г��� ��ɵ��� ���� ���� ���� ���� ������ ���� ��ɵ��� ǥ��

	FCommandInfo(FString name, FString description, FKey shortCut, int row, int column, FCommandDelegate dele)
	{
		this->name = name;
		this->description = description;
		this->shortCut = shortCut;
		this->row = row;
		this->column = column;
		commandDele = dele;
	}

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CommandInfo")
		FString name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CommandInfo")
		FString description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CommandInfo")
		FKey shortCut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CommandInfo")
		int row;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CommandInfo")
		int column;

	//UPROPERTY(BlueprintAssignable)
	FCommandDelegate commandDele;
};