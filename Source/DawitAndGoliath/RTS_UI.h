// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Components/PanelSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "DNGStruct.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetSwitcher.h"
//#include "Engine/UserInterfaceSetting.h"
#include "RTS_UI.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API URTS_UI : public UUserWidget
{
	GENERATED_BODY()

public:
	URTS_UI(const FObjectInitializer &objInitializer);
	void DrawBox(FVector2D start, FVector2D end);
	
	void Display(TArray<class ADNG_RTSBaseObject*>&);
	void ResetDisplay();

	void DisplayUnitInform(class ADNG_RTSBaseObject*);
	void ResetUnitInform();

	void DisplayProductionInform(class ADNG_RTSBaseObject *construction);
	void ResetProductionInform();

	void SetCommandOnPanel(FCommandInfo cmdInfo);
	void ResetCommandOnPanel();
	void SetMouseCursor();

	void DisplayUnitEntity(TArray<class ADNG_RTSBaseObject*>&);
	void ResetUnitEntity();
	void ResetUnitEntityGrid();

	void SetObjectsArray(TArray<class ADNG_RTSBaseObject*> *objArray) { objectsArray = objArray; }

	// Getter
	UImage* GetSelectionBoxImage() { return selectionBoxImage; };
	UGridPanel* GetCommandPanel() { return commandPanel; };
	
	UFUNCTION(BlueprintCallable)
		void SendToPawnPanelInfo(FString key);

	UFUNCTION(BlueprintCallable)
		void RemoveQueueElement(int index);
private:

	UFUNCTION(BlueprintCallable)
		void SelectEntity(int row, int column);

	UFUNCTION(BlueprintCallable)
		void ExceptEntity(int row, int column);

	UFUNCTION(BlueprintCallable)
		void SelectPage(int pageNum);

protected:

public:
	UPROPERTY(BlueprintReadOnly)
		float currentProgress;
	UPROPERTY(BlueprintReadOnly)
		float totalProgress;
	UPROPERTY(BlueprintReadWrite)
		int entityPage;
	UPROPERTY(BlueprintReadWrite)
		int currentEntityPage;

private:
	enum { SLOT_NUMBER, PRODUCTION_UNIT};
	const int MAX_QUEUE_SIZE = 5;

	class ADNG_RTSBarrack *focusingBarrack;
	TArray<class ADNG_RTSBaseObject*> *objectsArray;
private:

protected:
	UPROPERTY(BlueprintReadOnly)
		int maxRow;

	UPROPERTY(BlueprintReadOnly)
		int maxColumn;

	UPROPERTY(BlueprintReadOnly)
		int maxPages;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UImage *selectionBoxImage;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UGridPanel *commandPanel;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UCanvasPanel *entityInformCanvas;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UCanvasPanel *entityCanvas;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UGridPanel *entityGrid;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UGridPanel *entityPageGrid;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UCanvasPanel *productionInformCanvas;

	UPROPERTY(BlueprintReadWrite, Category = "RTS_UI")
		UProgressBar *productionProgress;

	UPROPERTY(BlueprintReadWrite, Category = "RTS_UI")
		TArray<UWidgetSwitcher*> queueSlots;

public:
};