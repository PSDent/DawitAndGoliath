// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSPawn.h"
#include "Engine.h"

// Sets default values
ADNG_RTSPawn::ADNG_RTSPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	rtsCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("RTS_Cam"));
	rtsCamera->AttachTo(RootComponent);
	rtsCamera->bUsePawnControlRotation = false;

	rtsCamera->SetRelativeRotation(FRotator(-60.0f, -40.0f, 0.0f));

	camScrollBoundary = 20.0f;
	scrollSpeed = 3000.0f;
	height = 1400.0f;
	bPressedLeftMouse = false;
}

// Called when the game starts or when spawned
void ADNG_RTSPawn::BeginPlay()
{
	Super::BeginPlay();
	
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "RTSCHARACTER");
	//Cast<APlayerController>(Controller)->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetGameViewport()->SetMouseLockMode(EMouseLockMode::LockAlways);
	
	FVector newPos = GetActorLocation() + FVector::UpVector * height;
	SetActorLocation(newPos);
	
}

// Called every frame
void ADNG_RTSPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	viewPort = GEngine->GameViewport;
	check(viewPort);
	viewPort->GetViewportSize(viewportSize);


	if (viewPort->IsFocused(viewPort->Viewport))
	{
		viewPort->GetMousePosition(mousePos);
		
		MoveCam(DeltaTime);

		if (bPressedLeftMouse)
			SelectBox();
	}
}

// Called to bind functionality to input
void ADNG_RTSPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("LMousePress", IE_Pressed, this, &ADNG_RTSPawn::LMousePress);
	PlayerInputComponent->BindAction("LMousePress", IE_Released, this, &ADNG_RTSPawn::LMouseRelease);
	PlayerInputComponent->BindAction("RMousePress", IE_Pressed, this, &ADNG_RTSPawn::RMousePress);

}

void ADNG_RTSPawn::Init()
{
	Cast<APlayerController>(Controller)->bShowMouseCursor = true;
	
	userUI = CreateWidget<URTS_UI>(GetWorld(), UI_Class);

	viewPort = GEngine->GameViewport;

	if (userUI)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "UI Created");
		userUI->AddToViewport();

	}
}

void ADNG_RTSPawn::MoveCam(float DeltaTime)
{
	if (mousePos.X < camScrollBoundary)
	{
		MoveRightCam(-1.0f * DeltaTime);
	}
	else if (viewportSize.X - mousePos.X < camScrollBoundary)
	{
		MoveRightCam(1.0f * DeltaTime);
	}

	if (mousePos.Y < camScrollBoundary)
	{
		MoveUpCam(1.0f * DeltaTime);
	}
	else if (viewportSize.Y - mousePos.Y < camScrollBoundary)
	{
		MoveUpCam(-1.0f * DeltaTime);
	}
}

void ADNG_RTSPawn::MoveRightCam(float direction)
{
	float velocity = direction * scrollSpeed;

	FVector deltaMovement = velocity * (FRotator(0.0f, 90.0f, 0.0f) + FRotator(0.0f, rtsCamera->RelativeRotation.Yaw, 0.0f)).Vector();

	FVector newPos = GetActorLocation() + deltaMovement;

	SetActorLocation(newPos);
}

void ADNG_RTSPawn::MoveUpCam(float direction)
{
	float velocity = direction * scrollSpeed;

	FVector deltaMovement = velocity * FRotator(0.0f, rtsCamera->RelativeRotation.Yaw, 0.0f).Vector();

	FVector newPos = GetActorLocation() + deltaMovement;

	SetActorLocation(newPos);
}

void ADNG_RTSPawn::LMousePress()
{
	bPressedLeftMouse = true;

	mouseStartPos.X = mousePos.X;
	mouseStartPos.Y = mousePos.Y;
}

void ADNG_RTSPawn::LMouseRelease()
{
	bPressedLeftMouse = false;
	//mouseStartPos = FVector2D::ZeroVector;
	userUI->selectionBoxImage->SetVisibility(ESlateVisibility::Collapsed);
}

void ADNG_RTSPawn::RMousePress()
{

}

void ADNG_RTSPawn::SelectBox()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%f %f"), mousePos.X, mousePos.Y));
	userUI->DrawBox(mouseStartPos.X, mouseStartPos.Y, mousePos.X, mousePos.Y);
}
