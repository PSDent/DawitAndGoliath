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

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;

	void Init();
	void ReceiveCmdPanel(FKey key);

	// MiniMap
	void CamMoveTo(FVector2D pos);
	void GetMinimapToWorldPos(FVector2D pos);

	// setter
	void SetCommandingFlag(bool flag) { bIsCommanding = flag; }

	///### getter ###
	bool GetLeftMouseStatus() { return bPressedLeftMouse; }
	bool GetRightMouseStatus() { return bPressedRightMouse; }
	bool GetCommandingStatus() { return bIsCommanding; }

	APlayerController* GetPlayerController() { return playerController; }
	URTS_UI* GetUI() { return userUI; };
	UGameViewportClient* GetViewportClient() { return viewPort; }

	///####################

	UFUNCTION(Reliable, Client, WithValidation)
		virtual void Client_Init();
	void Client_Init_Implementation();
	bool Client_Init_Validate();

	void SetObjectOwner(class ADNG_RTSBaseObject *obj, AController *ownController);
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void Server_SetObjectOwner(class ADNG_RTSBaseObject *obj, AController *ownController);
	void Server_SetObjectOwner_Implementation(class ADNG_RTSBaseObject *obj, AController *ownController);
	bool Server_SetObjectOwner_Validate(class ADNG_RTSBaseObject *obj, AController *ownController) { return true; }

	void RemoveFromSquad(class ADNG_RTSBaseObject *obj, int squadNum);
	UFUNCTION(Reliable, Server, WithValidation)
		void Server_RemoveFromSquad(class ADNG_RTSBaseObject *obj, int squadNum);
	void Server_RemoveFromSquad_Implementation(class ADNG_RTSBaseObject *obj, int squadNum);
	bool Server_RemoveFromSquad_Validate(class ADNG_RTSBaseObject *obj, int squadNum) { return true; }

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_RemoveFromSquad(class ADNG_RTSBaseObject *obj, int squadNum);
	void Multicast_RemoveFromSquad_Implementation(class ADNG_RTSBaseObject *obj, int squadNum);
	bool Multicast_RemoveFromSquad_Validate(class ADNG_RTSBaseObject *obj, int squadNum) { return true; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	// Key
	void PressShiftKey() { bPressedShiftKey = true; }
	void ReleasedShiftKey();

	void PressCtrlKey() { bPressedCtrlKey = true; }
	void ReleasedCtrlKey() { bPressedCtrlKey = false; }

	// Initialize
	void BasicInit();

	// Cam
	void MoveCam(float DeltaTime);
	void MoveRightCam(float direction);
	void MoveUpCam(float direction);

	// Mouse 
	void LMousePress();
	void LMouseRelease();
	void RMousePress();
	void RMouseRelease();

	void ShiftLMBPress() { bPressedShiftLMB = true; }
	void ShiftLMBRelease() { bPressedShiftLMB = false; }
	void CtrlLMBPress() { bPressedCtrlLMB = true; }
	void CtrlLMBRelease() { bPressedCtrlLMB = false; }

	void SelectAllSameType();
	void DoubleClick();

	// Unit Command
	void FindMostUnit();
	void MappingCmdPanel();
	
	void ExecuteCommand(FKey key);
	//UFUNCTION(Server, Reliable, WithValidation)


	void ResetSelectedUnits();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_ResetSelectedUnits();
	void Server_ResetSelectedUnits_Implementation();
	bool Server_ResetSelectedUnits_Validate() { return true; }

	void SetSelectedUnits();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_SetSelectedUnits();
	void Server_SetSelectedUnits_Implementation();
	bool Server_SetSelectedUnits_Validate() { return true; }

	template<int num>
	void SetSquad();

	template<int num>
	void GetSquad();

	template<int num>
	void AddToSquad();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
		TSubclassOf<class ADNG_RTSHUD> HUD_Class;

	UPROPERTY(Replicated, BlueprintReadOnly)
		UCameraComponent *rtsCamera;

	// Unit
	UPROPERTY(Replicated, BlueprintReadOnly)
		int currentSupply;
	UPROPERTY(Replicated, BlueprintReadOnly)
		int maxSupply;

	UPROPERTY(Replicated)
		FVector targetPos;
	FVector minimapTargetPos;

	AActor *targetActor;

	UPROPERTY(BlueprintReadOnly)
		bool bPressedShiftKey;
	UPROPERTY(BlueprintReadWrite)
		bool bPressedShiftLMB;
	UPROPERTY(BlueprintReadWrite)
		bool bPressedCtrlLMB;
	UPROPERTY(BlueprintReadWrite)
		bool bPressedCtrlKey;

private:
	FTimerDelegate testDele;
	FTimerHandle testHandle;

	FTimerDelegate doubleClickDele;
	FTimerHandle doubleClickHandle;

	UPROPERTY(Replicated)
		APlayerController *playerController;

	UPROPERTY(Replicated)
		TArray<AActor*> minimapPointArray;
	TArray<ADNG_RTSBaseObject*> selectedUnits;
	UPROPERTY(Replicated)
	TArray<FBaseObjectArray> squads;

	// UI 
	URTS_UI *userUI;
	UGameViewportClient *viewPort;

	// Mouse
	FVector2D mousePos;
	FVector2D mouseStartPos;
	FVector2D viewportSize;

	float selectionAllRadius;
	float unitsPlacementOffset;
	float doubleClickDelay;

	bool bIsCanDoubleClick;
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
	
	AActor *prevSelectUnit;
	AActor *nowSelectUnit;

	// Cam
	float camScrollBoundary;
	float scrollSpeed;
	float height;

	bool bPressedRightMouse;
	bool bPressedLeftMouse;

private:
	const int SQUAD_SIZE = 10;
	const float UPPER_HEIGHT = 10000.0f;
	const float LOWER_HEIGHT = -1000.0f;
};