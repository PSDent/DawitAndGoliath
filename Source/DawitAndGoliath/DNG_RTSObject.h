// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/Material.h"
#include "DNG_RTSUnitAIController.h"
#include "DNG_RTSObject.generated.h"

UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSObject : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADNG_RTSObject();

	void SetSelectedStatus(bool status);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Basic RTS Object Command
	void Move(FVector dest);


public:
	
private:
	UPawnMovementComponent *characterMovementComponent;
	UDecalComponent *ringDecal;
	ADNG_RTSUnitAIController *aiController;
	

	bool bisSelected;

protected:

public:



};