// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "RTS_UI.h"
#include "DNG_RTSPawn.generated.h"

UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADNG_RTSPawn();

	void Init();

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

	// Unit Command
	void SelectBox();


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

private:
	// TArray<ADNG_Unit> selectedUnits;

	URTS_UI *userUI;
	UGameViewportClient *viewPort;

	FVector2D mousePos;
	FVector2D mouseStartPos;
	FVector2D viewportSize;

	float camScrollBoundary;
	float scrollSpeed;
	float height;

	bool bPressedLeftMouse;
};
