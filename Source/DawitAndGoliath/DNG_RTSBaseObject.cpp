// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSBaseObject.h"
#include "Engine.h"

// Sets default values
ADNG_RTSBaseObject::ADNG_RTSBaseObject() : Super()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	/*
	characterMovementComponent = CreateDefaultSubobject<UPawnMovementComponent>(TEXT("UPawnMovementComponent"));
	characterMovementComponent->UpdatedComponent = RootComponent;*/
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	//characterMovementComponent->Velocity

	bIsHold = false;
	bIsSelected = false;
}

// Called when the game starts or when spawned
void ADNG_RTSBaseObject::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < RootComponent->GetNumChildrenComponents(); ++i)
	{
		if (RootComponent->GetChildComponent(i)->GetName() == "Decal")
		{
			ringDecal = Cast<UDecalComponent>(RootComponent->GetChildComponent(i));
			ringDecal->SetVisibility(false);

			break;
		}
	}
	aiController = Cast<ADNG_RTSUnitAIController>(Controller);
	//ringDecal->SetupAttachment(RootComponent);

	// characterMovementComponent->MaxWalkSpeed = 600.0f;
}

// Called every frame
void ADNG_RTSBaseObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADNG_RTSBaseObject::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);



}

void ADNG_RTSBaseObject::SetSelectedStatus(bool status)
{
	bIsSelected = status;

	if (bIsSelected)
	{
		ringDecal->SetVisibility(true);
	}
	else
		ringDecal->SetVisibility(false);

}

void ADNG_RTSBaseObject::Move(FVector dest)
{
	aiController->MoveToLocation(dest);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Moving");
}

void ADNG_RTSBaseObject::Stop()
{
	aiController->StopMovement();
}


void ADNG_RTSBaseObject::Hold()
{
	Stop();
	bIsHold = true;
}


void ADNG_RTSBaseObject::Patrol()
{

}