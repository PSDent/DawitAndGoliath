// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSObject.h"

// Sets default values
ADNG_RTSObject::ADNG_RTSObject()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADNG_RTSObject::BeginPlay()
{
	Super::BeginPlay();
	
	characterMovementComponent->MaxWalkSpeed = 600.0f;
}

// Called every frame
void ADNG_RTSObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADNG_RTSObject::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}