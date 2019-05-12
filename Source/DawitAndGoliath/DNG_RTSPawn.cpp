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
	selectionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SelectionCapsule"));

	camScrollBoundary = 20.0f;
	scrollSpeed = 3000.0f;
	height = 1400.0f;
	
	unitsPlacementOffset = 300.0f;
	selectionAllRadius = 2000.0f;

	bPressedLeftMouse = false;
	bPressedShiftKey = false;
	bPressedCtrlKey = false;
	bIsDoubleClicked = false;
	bIsCommanding = false;

	baseUnit = nullptr;

	selectedUnits.Empty();
	EKeys::GetAllKeys(keys);
	commandArray.Init(CommandDelegate(), keys.Num());

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

	FindMostUnit();
}

// Called to bind functionality to input
void ADNG_RTSPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("LMousePress", IE_Pressed, this, &ADNG_RTSPawn::LMousePress);
	PlayerInputComponent->BindAction("LMousePress", IE_Released, this, &ADNG_RTSPawn::LMouseRelease);

	PlayerInputComponent->BindAction("RMousePress", IE_Pressed, this, &ADNG_RTSPawn::RMousePress);

	PlayerInputComponent->BindAction("Shift", IE_Pressed, this, &ADNG_RTSPawn::PressShiftKey);
	PlayerInputComponent->BindAction("Shift", IE_Released, this, &ADNG_RTSPawn::ReleasedShiftKey);

	PlayerInputComponent->BindAction("Ctrl", IE_Pressed, this, &ADNG_RTSPawn::PressCtrlKey);
	PlayerInputComponent->BindAction("Ctrl", IE_Released, this, &ADNG_RTSPawn::ReleasedCtrlKey);
	
	PlayerInputComponent->BindAction("LMousePress", IE_DoubleClick, this, &ADNG_RTSPawn::SelectAllSameType);
	PlayerInputComponent->BindAction("CtrlLMouse", IE_Pressed, this, &ADNG_RTSPawn::SelectAllSameType);
}

void ADNG_RTSPawn::Init()
{
	playerController = Cast<APlayerController>(Controller);

	playerController->bShowMouseCursor = true;
	
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
	playerController->GetHitResultUnderCursor(ECC_Visibility, false, outHit);
	selectionStartPos = outHit.Location;
	baseUnit = Cast<ADNG_RTSBaseObject>(outHit.GetActor());

	if (bPressedCtrlKey)
	{
		SelectAllSameType();
		baseUnit = nullptr;
	}
	// 이후 해당 유닛의 정보를 UI 화면 중앙 밑에 띄우게 할 것.
	// 또한 유닛들을 2개 이상 선택했을 때, 정보창에 그 개체수들을 띄우도록 할 것.
}

void ADNG_RTSPawn::LMouseRelease()
{
	bPressedLeftMouse = false;
	userUI->selectionBoxImage->SetVisibility(ESlateVisibility::Collapsed);
	
	if (bIsDoubleClicked)
	{
		bIsDoubleClicked = false;
		return;
	}
	else if (bPressedCtrlKey)
	{
		return;
	}

	FHitResult outHit;
	playerController->GetHitResultUnderCursor(ECC_Visibility, false, outHit);
	selectionEndPos = outHit.Location;

	SelectionUnitsInBox();
}

// 더블클릭 or Ctrl + 좌클릭
void ADNG_RTSPawn::SelectAllSameType()
{
	if (!baseUnit) return;

	// Shift키를 눌렀다면 선택 유닛 리스트에 추가한다

	// 아니라면 그냥 그 유닛 뭉치들만 유닛 리스트에 추가한다.
	// 별도의 변수들을 유저 폰 자체에 넣어서 이 함수를 작동시킨다.
	selectionCapsule->SetCapsuleRadius(selectionAllRadius);
	selectionCapsule->SetCapsuleHalfHeight(1000.0f); 
	selectionCapsule->SetWorldLocation(baseUnit->GetActorLocation());

	DrawDebugCapsule(GetWorld(), baseUnit->GetActorLocation(), 1000.0f, selectionAllRadius, FQuat::Identity, FColor::Orange, true, 3.0f);

	TArray<AActor*> selectedAllUnits;
	selectionCapsule->GetOverlappingActors(selectedAllUnits, TSubclassOf<ADNG_RTSBaseObject>());
	
	selectedUnits.Empty();

	FString baseUnitName = baseUnit->GetUnitName();

	for (auto actor : selectedAllUnits)
	{
		ADNG_RTSBaseObject *unit = Cast<ADNG_RTSBaseObject>(actor);

		if (!unit) continue;

		if (baseUnitName == unit->GetUnitName())
		{
			unit->SetSelectedStatus(true);
			selectedUnits.Add(unit);
		}
	}

	bIsDoubleClicked = true;
	baseUnit = nullptr;
}

void ADNG_RTSPawn::SelectionUnitsInBox()
{
	float extentX = (selectionEndPos.X - selectionStartPos.X) * 0.5f;
	float extentY = (selectionEndPos.Y - selectionStartPos.Y) * 0.5f;
	float extentZ = 2000.0f;
	FVector extent(extentX, extentY, extentZ);

	float midPosX = selectionStartPos.X + extentX;
	float midPosY = selectionStartPos.Y + extentY;
	float midPosZ = selectionStartPos.Z - extentZ * 0.5f;
	FVector midPos(midPosX, midPosY, midPosZ);

	TArray<AActor*> selectedActors;

	extent = FVector(abs(extent.X), abs(extent.Y), abs(extent.Z));
	selectionBox->SetWorldLocation(midPos);
	selectionBox->SetBoxExtent(extent);
	selectionBox->GetOverlappingActors(selectedActors, TSubclassOf<ADNG_RTSBaseObject>());

	bool bIsEmpty = false;

	for (auto actor : selectedActors)
	{
		ADNG_RTSBaseObject *unit = Cast<ADNG_RTSBaseObject>(actor);

		if (!unit)
			selectedActors.Remove(actor);
	}

	if (!bPressedShiftKey)
	{
		for (auto actor : selectedActors)
		{
			ADNG_RTSBaseObject *unit = Cast<ADNG_RTSBaseObject>(actor);

			if (!bIsEmpty)
			{
				bIsEmpty = true;

				for (int i = 0; i < selectedUnits.Num(); ++i)
				{
					selectedUnits[i]->SetSelectedStatus(false);
				}

				selectedUnits.Empty();
			}

			unit->SetSelectedStatus(true);
			selectedUnits.Add(unit);
		}
	}
	else if (bPressedShiftKey)
	{
		if (selectedActors.Num() == 1)
		{
			ADNG_RTSBaseObject *unit = Cast<ADNG_RTSBaseObject>(selectedActors[0]);

			bool status = !unit->GetSelectedStatus();
			unit->SetSelectedStatus(status);
			if(status)
				selectedUnits.Add(unit);
			else
				selectedUnits.Remove(unit);

		}
		else if (selectedActors.Num() > 1)
		{
			for (auto actor : selectedActors)
			{
				ADNG_RTSBaseObject *unit = Cast<ADNG_RTSBaseObject>(actor);
				unit->SetSelectedStatus(true);
				selectedUnits.Add(unit);
			}
		}
	}
}

void ADNG_RTSPawn::RMousePress()
{
	FHitResult outHit;
	Cast<APlayerController>(Controller)->GetHitResultUnderCursor(ECC_Visibility, false, outHit);
	FVector dest = outHit.Location;
	
	// 적 클릭 시 공격

	// 땅 바닥 클릭 시 이동
	MoveUnits(dest);


}

void ADNG_RTSPawn::MoveUnits(FVector dest)
{
	int size = selectedUnits.Num();
	float root = sqrt(size);
	float side = (int)root;

	if (size == 0) return;

	if (root != side)
	{
		side += 1;
	}

	int offsetX = (size / side + (size % (int)side == 0 ? 0 : 1)) - 1;
	int offsetY = side - 1;

	// X -> 위 아래, Y -> 왼 오른

	float startPointX = unitsPlacementOffset * (offsetX * 0.5f);
	float startPointY = unitsPlacementOffset * (-offsetY * 0.5f);

	float x = dest.X + startPointX, y = dest.Y + startPointY;

	// GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, FString::Printf(TEXT("%d"), side));
	for (int i = 0; i < (int)side; ++i)
	{
		float nX = x - (unitsPlacementOffset * i);
		for (int j = 0; ((j < (int)side) && (i * (int)side + j < size)); ++j)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, "LOOPING");

			float nY = y + (unitsPlacementOffset * j);

			FVector pos(nX, nY, dest.Z);
			int idx = i * (int)side + j;
			DrawDebugSphere(GetWorld(), pos, 32.0f, 16, FColor::Yellow, 5.0f);
			selectedUnits[idx]->Move(pos);
		} 
	}
}

void ADNG_RTSPawn::DrawSelectBox()
{
	userUI->DrawBox(mouseStartPos, mousePos);
}

void ADNG_RTSPawn::FindMostUnit()
{
	int len = selectedUnits.Num();

	if (len == 0)
	{
		// 이 곳에 명령 패널을 초기화하는 로직을 넣어둘 것.
		mostUnit = nullptr;
		return;
	}

	TMap<FString, int> unitCount;

	for (auto unit : selectedUnits)
	{
		FString name = unit->GetUnitName();
		
		if(!unitCount.Find(name))
			unitCount.Add(name, 1);
		else
			++unitCount[name];

		if (!mostUnit)
		{
			mostUnit = unit;
		}
		else
		{
			FString mostUnitName = mostUnit->GetUnitName();
			if (unitCount[name] > unitCount[mostUnitName])
			{
				mostUnit = unit;
			}
		}
	}
	MappingCmdPanel();
}

void ADNG_RTSPawn::MappingCmdPanel()
{
	TArray<FCommandInfo>& cmdInfo = mostUnit->GetCmdInfoArray();

	for (int i = 0; i < cmdInfo.Num(); ++i)
	{
		userUI->SetCommandOnPanel(cmdInfo[i]);
	}

}

void ADNG_RTSPawn::CheckKeys()
{
	for (int i = 0; i < keys.Num(); ++i)
	{
		if (playerController->IsInputKeyDown(keys[i]))
		{
			commandArray[i].ExecuteIfBound();
		}
	}
}