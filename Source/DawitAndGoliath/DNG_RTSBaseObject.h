// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/DecalComponent.h"
#include "Materials/Material.h"
#include "DNG_RTSUnitAIController.h"
#include "DNGStruct.h"
#include "DNG_RTSBaseObject.generated.h"

UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSBaseObject : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADNG_RTSBaseObject();

	void SetSelectedStatus(bool status);
	bool GetSelectedStatus() { return bIsSelected; };
	FString GetUnitName() { return unitName; };

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
	void Stop();
	void Hold();
	void Patrol();



public:

private:
	UDecalComponent *ringDecal;
	ADNG_RTSUnitAIController *aiController;

	FString unitName;

	bool bIsHold;
	bool bIsSelected;

protected:

public:



};