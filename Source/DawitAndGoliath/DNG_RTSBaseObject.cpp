// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSBaseObject.h"
#include "DNGProperty.h"
#include "DNGDelegates.h"
#include "DNG_RTSPawn.h"
#include "RTS_UI.h"

#include "Net/UnrealNetwork.h"
#include "Engine.h"

// Sets default values
ADNG_RTSBaseObject::ADNG_RTSBaseObject() : Super()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	ringDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionRing"));
	ringDecal->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UMaterial> decalMaterial(TEXT("Material'/Game/Blueprints/FriendlyRing.FriendlyRing'"));
	if (decalMaterial.Succeeded())
	{
		ringDecal->SetDecalMaterial(decalMaterial.Object);
		ringDecal->RelativeLocation = FVector(0.0f, 0.0f, -80.0f);
		ringDecal->DecalSize = FVector(32.f, 64.f, 64.f);
		ringDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
		ringDecal->SetVisibility(false);
	}

	objProperty = CreateDefaultSubobject<UDNGProperty>(TEXT("DNGProperty"));

	Tags.Add("Object");
	
	unitName = "Object";

	bIsSelected = false;

	
}

void ADNG_RTSBaseObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADNG_RTSBaseObject, aiController);
	DOREPLIFETIME(ADNG_RTSBaseObject, pawn);
}

// Called when the game starts or when spawned
void ADNG_RTSBaseObject::BeginPlay()
{
	Super::BeginPlay();

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

