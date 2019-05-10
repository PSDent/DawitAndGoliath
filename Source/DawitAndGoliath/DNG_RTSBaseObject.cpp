// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSBaseObject.h"
#include "DNGProperty.h"

#include "Engine.h"

// Sets default values
ADNG_RTSBaseObject::ADNG_RTSBaseObject() : Super()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	bIsHold = false;
	bIsSelected = false;

	objProperty = CreateDefaultSubobject<UDNGProperty>(TEXT("DNGProperty"));

	Tags.Add("Object");
	
	unitName = "Object";

	commandInfoArray.Add(FCommandInfo("Move", "Move Unit to Clicked Location", EKeys::M, 0, 0));
	commandInfoArray.Add(FCommandInfo("Stop", "Stop Unit Movement", EKeys::S, 0, 1));
	commandInfoArray.Add(FCommandInfo("Hold", "Hold Unit Movement", EKeys::H, 0, 2));
	commandInfoArray.Add(FCommandInfo("Patrol", "Patrol Unit point to point", EKeys::P, 0, 3));
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