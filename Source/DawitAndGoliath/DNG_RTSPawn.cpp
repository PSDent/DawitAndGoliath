// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSPawn.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"

// Sets default values
ADNG_RTSPawn::ADNG_RTSPawn() : Super()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	rtsCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("RTS_Cam"));
	rtsCamera->AttachTo(RootComponent);
	rtsCamera->bUsePawnControlRotation = false;
	//rtsCamera->SetRelativeRotation(FRotator(-60.0f, -40.0f, 0.0f));
	rtsCamera->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));

	selectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SelectionBox"));

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
			DrawSelectBox();
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

	FHitResult outHit;
	Cast<APlayerController>(Controller)->GetHitResultUnderCursor(ECC_Visibility, false, outHit);
	Cast<APlayerController>(Controller)->GetMousePosition(selectionStartPos.X, selectionStartPos.Y);
	//selectionStartPos = outHit.Location;
	//DrawDebugSphere(GetWorld(), selectionStartPos, 64.0f, 64, FColor::Red, true, 5.0f);
}

void ADNG_RTSPawn::LMouseRelease()
{
	bPressedLeftMouse = false;
	//mouseStartPos = FVector2D::ZeroVector;
	userUI->selectionBoxImage->SetVisibility(ESlateVisibility::Collapsed);

	FHitResult outHit;
	Cast<APlayerController>(Controller)->GetHitResultUnderCursor(ECC_Visibility, false, outHit);
	Cast<APlayerController>(Controller)->GetMousePosition(selectionEndPos.X, selectionEndPos.Y);

	//selectionEndPos = outHit.Location;
	//DrawDebugSphere(GetWorld(), selectionEndPos, 64.0f, 64, FColor::Red, true, 5.0f);


	SelectionUnitsInBox();
}

// 여기 박스
// 원래 하던 방식 말고
// 바꾸려는 방식 기억하고 있을거야
// 그 방식으로 좀 바꿔봐
// 안그러면 선택이 잘 안된다야 
void ADNG_RTSPawn::SelectionUnitsInBox()
{
	float extentX = (selectionEndPos.X - selectionStartPos.X) * 0.5f;
	float extentY = (selectionEndPos.Y - selectionStartPos.Y) * 0.5f;
	float extentZ = 2000.0f;
	FVector extent(extentX, extentY, extentZ);
	
	float midPosX = selectionStartPos.X + extentX * 0.5f;
	float midPosY = selectionStartPos.Y + extentY * 0.5f;
	//float midPosZ = selectionStartPos.Z/* - extentZ */;
	//FVector midPos(midPosX, midPosY, midPosZ);
	
	//FRotator rot(-rtsCamera->GetComponentRotation().Pitch, rtsCamera->GetComponentRotation().Yaw, rtsCamera->GetComponentRotation().Roll);
	//selectionBox->SetWorldRotation(rot);
		
	//selectionBox->SetWorldLocation(midPos);
	
	TArray<ADNG_RTSBaseObject*> selectedActors;
	GetWorld()->GetFirstPlayerController()->GetHUD()->GetActorsInSelectionRectangle<ADNG_RTSBaseObject>(selectionStartPos, selectionEndPos, selectedActors, false, false);
	//selectionBox->SetBoxExtent(extent);


	//selectionBox->GetOverlappingActors(selectedActors);

	//DrawDebugBox(GetWorld(), midPos, extent, rot.Quaternion(), FColor::Red, true, 5.0f);

	bool bIsThereUnit = false;

	for (auto unit : selectedActors)
	{
		if (unit)
		{
			if (!bIsThereUnit)
			{

				for (int i = 0; i < selectedUnits.Num(); ++i)
				{
					selectedUnits[i]->SetSelectedStatus(false);
				}

				selectedUnits.Empty();
			}
			bIsThereUnit = true;

			GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, "UNIT");
			unit->SetSelectedStatus(true);
			selectedUnits.Add(unit);
		}
	}

}

void ADNG_RTSPawn::RMousePress()
{
	FHitResult outHit;
	Cast<APlayerController>(Controller)->GetHitResultUnderCursor(ECC_Visibility, false, outHit);
	FVector dest = outHit.Location;
	// 적을 눌렀을 시 공격

	DrawDebugSphere(GetWorld(), dest, 64, 16, FColor::Red, true, 5.0f);

	// 땅을 눌렀을 시 이동
	for (auto unit : selectedUnits)
	{
		unit->Move(dest);
	}

}

void ADNG_RTSPawn::DrawSelectBox()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%f %f"), mousePos.X, mousePos.Y));
	userUI->DrawBox(mouseStartPos, mousePos);
}
