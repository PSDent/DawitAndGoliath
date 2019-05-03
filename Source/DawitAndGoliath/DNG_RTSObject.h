// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DNG_RTSObject.generated.h"
#include "GameFramework/CharacterMovementComponent.h"

UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSObject : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADNG_RTSObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	UCharacterMovementComponent *characterMovementComponent;

protected:

public:



};
