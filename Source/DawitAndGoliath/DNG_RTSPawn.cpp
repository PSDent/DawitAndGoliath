// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSPawn.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

#include "DNG_RTSBarrack.h"
#include "DNG_RTSUnit.h"
#include "DNG_RTSUnit_Melee.h"
#include "FPSCharacter.h"

// Sets default values
ADNG_RTSPawn::ADNG_RTSPawn() : Super()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;
	
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
	bIsTargeted = false;
	bIsClickedPanel = false;
	bIsCommanding = false;

	bIsInitialized = false;
	baseUnit = nullptr;

	selectedUnits.Empty();
	EKeys::GetAllKeys(keys);
	
	targetActor = nullptr;

	currentSupply = 0;
	maxSupply = 100;
}

void ADNG_RTSPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADNG_RTSPawn, rtsCamera);
	DOREPLIFETIME(ADNG_RTSPawn, playerController);
	DOREPLIFETIME(ADNG_RTSPawn, currentSupply);
	DOREPLIFETIME(ADNG_RTSPawn, maxSupply);
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

	if (!bIsInitialized) return;

	if (selectedUnits.Num() == 0)
	{
		userUI->ResetDisplay();
	}
	else if (selectedUnits.Num() == 1)
	{
		userUI->Display(selectedUnits[0]);
	}
	else if (selectedUnits.Num() > 1)
	{
		userUI->DisplayUnitEntity(selectedUnits);
	}

	for (int i = 0; i < selectedUnits.Num(); ++i)
	{
		if (!selectedUnits[i]->bIsAlive)
		{
			selectedUnits.RemoveAt(i);
		}
	}

	viewPort = GEngine->GameViewport;
	check(viewPort);
	viewPort->GetViewportSize(viewportSize);

	if (viewPort->IsFocused(viewPort->Viewport))
	{
		viewPort->GetMousePosition(mousePos);
		
		MoveCam(DeltaTime);

		if (bPressedLeftMouse && !bIsClickedPanel)
			DrawSelectBox();
	}
	CheckKeysAndExecute();
	FindMostUnit();
}

// Called to bind functionality to input
void ADNG_RTSPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("LMousePress", IE_Pressed, this, &ADNG_RTSPawn::LMousePress);
	PlayerInputComponent->BindAction("LMousePress", IE_Released, this, &ADNG_RTSPawn::LMouseRelease);

	PlayerInputComponent->BindAction("RMousePress", IE_Pressed, this, &ADNG_RTSPawn::RMousePress);
	PlayerInputComponent->BindAction("RMousePress", IE_Released, this, &ADNG_RTSPawn::RMouseRelease);

	PlayerInputComponent->BindAction("Shift", IE_Pressed, this, &ADNG_RTSPawn::PressShiftKey);
	PlayerInputComponent->BindAction("Shift", IE_Released, this, &ADNG_RTSPawn::ReleasedShiftKey);

	PlayerInputComponent->BindAction("Ctrl", IE_Pressed, this, &ADNG_RTSPawn::PressCtrlKey);
	PlayerInputComponent->BindAction("Ctrl", IE_Released, this, &ADNG_RTSPawn::ReleasedCtrlKey);
	
	PlayerInputComponent->BindAction("LMousePress", IE_DoubleClick, this, &ADNG_RTSPawn::SelectAllSameType);
	PlayerInputComponent->BindAction("CtrlLMouse", IE_Pressed, this, &ADNG_RTSPawn::SelectAllSameType);
}

void ADNG_RTSPawn::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, "Init");

	Client_Init();
} 

void ADNG_RTSPawn::BasicInit()
{
	playerController = Cast<APlayerController>(Controller);

	playerController->bShowMouseCursor = true;
	bIsInitialized = true;
	//playerController->CurrentMouseCursor = EMouseCursor::Crosshairs;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, playerController->GetName());

	userUI = CreateWidget<URTS_UI>(GetWorld(), UI_Class);

	viewPort = GEngine->GameViewport;

	if (userUI)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "UI Created");
		userUI->AddToViewport();
	}
	
	rtsCamera->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));

}

void ADNG_RTSPawn::Client_Init_Implementation()
{
	BasicInit();
}

bool ADNG_RTSPawn::Client_Init_Validate()
{
	return true;
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
	FHitResult outHit;

	playerController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, outHit);

	targetPos = outHit.Location;
	
	if (Cast<ADNG_RTSBaseObject>(outHit.GetActor()) || Cast<AFPSCharacter>(outHit.GetActor()))
		targetActor = outHit.GetActor();
	else
		targetActor = nullptr;

	bPressedLeftMouse = true;

	mouseStartPos.X = mousePos.X;
	mouseStartPos.Y = mousePos.Y;

	selectionStartPos = outHit.Location;
	baseUnit = Cast<ADNG_RTSBaseObject>(outHit.GetActor());

	if (bPressedCtrlKey)
	{
		SelectAllSameType();
		baseUnit = nullptr;
	}
	else
	{

	}
	// 이후 해당 유닛의 정보를 UI 화면 중앙 밑에 띄우게 할 것.
	// 또한 유닛들을 2개 이상 선택했을 때, 정보창에 그 개체수들을 띄우도록 할 것.
}

void ADNG_RTSPawn::LMouseRelease()
{
	bPressedLeftMouse = false;
	userUI->GetSelectionBoxImage()->SetVisibility(ESlateVisibility::Collapsed);
	
	if (bIsDoubleClicked)
	{
		bIsDoubleClicked = false;
		return;
	}
	else if (bPressedCtrlKey)
	{
		return;
	}

	if (bIsClickedPanel)
		bIsClickedPanel = false;

	FHitResult outHit;
	playerController->GetHitResultUnderCursor(ECC_Visibility, false, outHit);
	selectionEndPos = outHit.Location;

	if(!bIsCommanding)
		SelectionUnitsInBox();
	bIsCommanding = false;
}

// 더블클릭 or Ctrl + 좌클릭
void ADNG_RTSPawn::SelectAllSameType()
{
	// 더블클릭 시 뭔가 좀 멈추는 듯
	if (!baseUnit || bIsCommanding) return;

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

		if (!unit || !unit->bIsAlive) continue;

		if (baseUnitName == unit->GetUnitName())
		{
			unit->SetSelectedStatus(true);
			unit->SetPawn(this);
			SetObjectOwner(unit, Controller);
			selectedUnits.Add(unit);
		}
	}

	for (int i = 0; i < selectedUnits.Num(); ++i)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, selectedUnits[i]->GetName());
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

	for (auto actor : selectedActors)
	{
		ADNG_RTSBaseObject *unit = Cast<ADNG_RTSBaseObject>(actor);

		if (!unit || !unit->bIsAlive)
			selectedActors.Remove(actor);
	}

	if (!bPressedShiftKey)
	{
		for (int i = 0; i < selectedUnits.Num(); ++i)
		{
			selectedUnits[i]->SetSelectedStatus(false);
		}

		selectedUnits.Empty();
		
		for (auto actor : selectedActors)
		{
			ADNG_RTSBaseObject *unit = Cast<ADNG_RTSBaseObject>(actor);

			// Controller 인수로 전달하여 
			if (unit)
			{
				SetObjectOwner(unit, Controller);
				unit->SetSelectedStatus(true);
				selectedUnits.Add(unit);
				unit->SetPawn(this);
			}
		}
	}
	// 서로 다른 종류의 유닛을
	// Shift + Left MB 후
	// 명령 내릴 시 팅김.

	else if (bPressedShiftKey)
	{
		if (selectedActors.Num() == 1)
		{
			ADNG_RTSBaseObject *unit = Cast<ADNG_RTSBaseObject>(selectedActors[0]);

			bool status = !unit->GetSelectedStatus();
			unit->SetSelectedStatus(status);
			if (status)
			{
				SetObjectOwner(unit, Controller);
				selectedUnits.Add(unit);
				unit->SetPawn(this);
			}
			else
				selectedUnits.Remove(unit);
		}
		else if (selectedActors.Num() > 1)
		{
			for (auto actor : selectedActors)
			{
				ADNG_RTSBaseObject *unit = Cast<ADNG_RTSBaseObject>(actor);
				unit->SetSelectedStatus(true);
				unit->SetPawn(this);
				SetObjectOwner(unit, Controller);
				selectedUnits.Add(unit);
			}
		}
	}
}

void ADNG_RTSPawn::SetObjectOwner(ADNG_RTSBaseObject *obj, AController *ownController)
{
	if (Role == ROLE_Authority)
	{
		// 액터 자체에만 하는게 아니라
		// 액터에 있는 Controller에도 오너를 설정하여야 함.
		obj->SetOwner(ownController);
		obj->GetController()->SetOwner(ownController);
		obj->SetPawn(Cast<ADNG_RTSPawn>(ownController->GetPawn())/*this*/);
	}
	else
	{
		Server_SetObjectOwner(obj, ownController);
	}
}

void ADNG_RTSPawn::Server_SetObjectOwner_Implementation(ADNG_RTSBaseObject *obj, AController *ownController)
{
	SetObjectOwner(obj, ownController);
}

void ADNG_RTSPawn::RMousePress()
{
	bPressedRightMouse = true;

	FHitResult outHit;
	Cast<APlayerController>(Controller)->GetHitResultUnderCursor(ECC_Visibility, false, outHit);
	FVector dest = outHit.Location;
	targetPos = outHit.Location;

	AActor *actor = outHit.GetActor();
	// 적 클릭 시 공격

	// 아군 클릭 시 아군 따라가기

	// 땅 바닥 클릭 시 이동
	MoveUnits(dest);
}

void ADNG_RTSPawn::RMouseRelease()
{
	bPressedRightMouse = false;
}

void ADNG_RTSPawn::MoveUnits(FVector dest)
{
	
	for (int i = 0; i < selectedUnits.Num(); ++i)
	{
		ADNG_RTSUnit *unit = Cast<ADNG_RTSUnit>(selectedUnits[i]);
		if (unit)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, "Casted");
			unit->Move();
		}
			//unit->Move();
	}

	return;

	//int size = selectedUnits.Num();
	//float root = sqrt(size);
	//float side = (int)root;

	//if (size == 0) return;

	//if (root != side)
	//{
	//	side += 1;
	//}

	//int offsetX = (size / side + (size % (int)side == 0 ? 0 : 1)) - 1;
	//int offsetY = side - 1;

	//// X -> 위 아래, Y -> 왼 오른

	//float startPointX = unitsPlacementOffset * (offsetX * 0.5f);
	//float startPointY = unitsPlacementOffset * (-offsetY * 0.5f);

	//float x = dest.X + startPointX, y = dest.Y + startPointY;

	//for (int i = 0; i < (int)side; ++i)
	//{
	//	float nX = x - (unitsPlacementOffset * i);
	//	for (int j = 0; ((j < (int)side) && (i * (int)side + j < size)); ++j)
	//	{

	//		float nY = y + (unitsPlacementOffset * j);

	//		FVector pos(nX, nY, dest.Z);
	//		int idx = i * (int)side + j;
	//		DrawDebugSphere(GetWorld(), pos, 32.0f, 16, FColor::Yellow, 5.0f);
	//		//selectedUnits[idx]->Move(pos);
	//	} 
	//}
}

void ADNG_RTSPawn::DrawSelectBox()
{
	userUI->DrawBox(mouseStartPos, mousePos);
}

void ADNG_RTSPawn::FindMostUnit()
{
	mostUnit = nullptr;
	userUI->ResetCommandOnPanel();

	int len = selectedUnits.Num();

	if (len == 0)
	{
		userUI->ResetCommandOnPanel();
		return;
	}

	TMap<FString, int> unitCount;

	for (auto unit : selectedUnits)
	{
		FString name = unit->GetUnitName();
		
		if (!unitCount.Contains(name))
		{
			unitCount.Add(name, 1);
		}
		else
			++unitCount[name];

		if (!mostUnit)
		{
			mostUnit = unit;
		}
		else
		{
			FString mostUnitName = mostUnit->GetUnitName();

			if (!mostUnit || !unitCount.Contains(mostUnitName))
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, mostUnitName);
				return;
			}
			// 유닛의 종류가 달라졌을 때 
			// mostUnit 또한 달라지는 데,
			// 이때 CommandMap의 상태를 초기화 하고 잘 바꿔야한다.
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
	TMap<FKey, const FCommandInfo> &cmdInfo = mostUnit->GetCmdInfoMap();

	for (auto cmd = cmdInfo.CreateConstIterator(); cmd; ++cmd)
	{
		userUI->SetCommandOnPanel(cmd.Value());
		commandMap.Add(cmd.Key(), cmd.Value());
	}
}

void ADNG_RTSPawn::CheckKeysAndExecute()
{
	for (auto cmd : commandMap)
	{
		if (playerController->IsInputKeyDown(cmd.Key))
		{
			ExecuteCommand(cmd.Key);
		}
	}

	//for (int i = 0; i < keys.Num(); ++i)
	//{
	//	if (playerController->IsInputKeyDown(keys[i]))
	//	{
	//		if (commandMap.Contains(keys[i]))
	//		{
	//			ExecuteCommand(keys[i]);
	//		}
	//	}
	//}
}

void ADNG_RTSPawn::ExecuteCommand(FKey key)
{
	// 키를 꾹 누르고 있을 때 처음 키를 누르고 있는 시간에 일정 시간을 둔 후,
	// 일정 시간이 지나면 프레임마다 명령을 수행하도록 구현할 것.
	for (int j = 0; j < selectedUnits.Num(); ++j)
	{
		if (selectedUnits[j]->GetCmdInfoMap().Contains(key))
		{
			FString unitCmdName = selectedUnits[j]->GetCmdInfoMap().Find(key)->name;
			FString mostUnitCmdName = mostUnit->GetCmdInfoMap().Find(key)->name;
			
			if (unitCmdName == mostUnitCmdName)
			{
				selectedUnits[j]->GetCmdInfoMap().Find(key)->commandDele.ExecuteIfBound();
			}
		}
	}
}

void ADNG_RTSPawn::ReceiveCmdPanel(FKey key)
{
	bIsClickedPanel = true;
	if (commandMap.Contains(key))
	{
		ExecuteCommand(key);
	}
}

