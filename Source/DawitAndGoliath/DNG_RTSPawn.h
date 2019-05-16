// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "RTS_UI.h"
#include "DNG_RTSObject.h"
#include "DNG_RTSBaseObject.h"
#include "DNGDelegates.h"
#include "Components/BoxComponent.h"
#include "DNG_RTSPawn.generated.h"

UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADNG_RTSPawn();

	void Init();
	void ReceiveCmdPanel(FKey key);

	// Key
	void PressShiftKey() { bPressedShiftKey = true; }
	void ReleasedShiftKey() { bPressedShiftKey = false; }

	void PressCtrlKey() { bPressedCtrlKey = true; }
	void ReleasedCtrlKey() { bPressedCtrlKey = false; }

	// getter
	bool GetLeftMouseStatus() { return bPressedLeftMouse; }
	bool GetRightMouseStatus() { return bPressedRightMouse; }
	APlayerController* GetPlayerController() { return playerController; }
	URTS_UI* GetUI() { return userUI; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Cam
	void MoveCam(float DeltaTime);
	void MoveRightCam(float direction);
	void MoveUpCam(float direction);

	// Mouse 
	void LMousePress();
	void LMouseRelease();
	void RMousePress();
	void RMouseRelease();

	void SelectAllSameType();
	void DoubleClick();

	// Unit Command
	void FindMostUnit();
	void MappingCmdPanel();
	void ExecuteCommand(FKey key);

	void DrawSelectBox();
	void SelectionUnitsInBox();
	void MoveUnits(FVector dest);

	// key
	void CheckKeysAndExecute();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
		TSubclassOf<class URTS_UI> UI_Class;

	UPROPERTY(BlueprintReadOnly)
		UCameraComponent *rtsCamera;

	FVector targetPos;
	AActor *targetActor;

private:
	APlayerController *playerController;
	TArray<ADNG_RTSBaseObject*> selectedUnits;

	// UI 
	URTS_UI *userUI;
	UGameViewportClient *viewPort;

	// Mouse
	FVector2D mousePos;
	FVector2D mouseStartPos;
	FVector2D viewportSize;

	float selectionAllRadius;
	float unitsPlacementOffset;
	bool bIsDoubleClicked;
	bool bIsCommanding;
	bool bIsTargeted;
	bool bIsClickedPanel;
	
	bool bIsInitialized;

	// Key
	FCommandDelegate dele;
	TArray<FKey> keys;
	TMap<FKey, FCommandInfo> commandMap;

	// SelectionBox
	UBoxComponent *selectionBox;
	UCapsuleComponent *selectionCapsule;
	FVector selectionStartPos;
	FVector selectionEndPos;

	// Selection All
	ADNG_RTSBaseObject *baseUnit;
	ADNG_RTSBaseObject *mostUnit;
	ADNG_RTSBaseObject *prevMostUnit;

	// Cam
	float camScrollBoundary;
	float scrollSpeed;
	float height;

	bool bPressedRightMouse;
	bool bPressedLeftMouse;
	bool bPressedShiftKey;
	bool bPressedCtrlKey;
};