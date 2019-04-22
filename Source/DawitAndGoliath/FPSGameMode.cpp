// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameMode.h"
#include "DNGProperty.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AFPSGameMode::AFPSGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("/Game/CharacterBP/FPSCharacter"));
	if (PlayerPawnObject.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnObject.Class;
		
	}
	

}

void AFPSGameMode::StartPlay()
{
	Super::StartPlay();

	StartMatch();

}
