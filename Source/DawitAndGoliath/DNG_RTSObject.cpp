// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSObject.h"
#include "Engine.h"

// Sets default values
ADNG_RTSObject::ADNG_RTSObject() : Super()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//RootComponent = RootComponent->

	//ringDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("RingDecal"));
	////ringDecal->SetupAttachment(RootComponent);
	////
	//ringDecal->SetDecalMaterial(ringMaterial);
	//ringDecal->RelativeLocation = FVector(0.f, 0.0f, 40.0f);
	//ringDecal->DecalSize = FVector(32.0f, 64.0f, 64.0f);
	//ringDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	//ringDecal->SetVisibility(true);

	//characterMovementComponent->MaxWalkSpeed = 600.0f;
	//characterMovementComponent->Velocity

	bisSelected = false;
}

// Called when the game starts or when spawned
void ADNG_RTSObject::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < RootComponent->GetNumChildrenComponents(); ++i)
	{
		if (RootComponent->GetChildComponent(i)->GetName() == "Decal")
		{
			ringDecal = Cast<UDecalComponent>(RootComponent->GetChildComponent(i));
			break;
		}
	}
	ringDecal->SetVisibility(false);
	aiController = Cast<ADNG_RTSUnitAIController>(Controller);
	//ringDecal->SetupAttachment(RootComponent);

	// characterMovementComponent->MaxWalkSpeed = 600.0f;
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

void ADNG_RTSObject::SetSelectedStatus(bool status)
{
	bisSelected = status;

	if (bisSelected)
	{
		ringDecal->SetVisibility(true);
	}
	else
		ringDecal->SetVisibility(false);

}

void ADNG_RTSObject::Move(FVector dest)
{
	// aiController->MoveToLocation(dest);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Moving");
}

