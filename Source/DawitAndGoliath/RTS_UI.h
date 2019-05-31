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
	
	void Display(class ADNG_RTSBaseObject*);
	void ResetDisplay();

	void DisplayUnitInform(class ADNG_RTSBaseObject*);
	void ResetUnitInform();

	void DisplayProductionInform(class ADNG_RTSBaseObject *construction);
	void ResetProductionInform();

	void SetCommandOnPanel(FCommandInfo cmdInfo);
	void ResetCommandOnPanel();
	void SetMouseCursor();

	// Getter
	UImage* GetSelectionBoxImage() { return selectionBoxImage; };
	UGridPanel* GetCommandPanel() { return commandPanel; };
	
	UFUNCTION(BlueprintCallable)
		void SendToPawnPanelInfo(FString key);

	UFUNCTION(BlueprintCallable)
		void RemoveQueueElement(int index);
private:

protected:

public:
	UPROPERTY(BlueprintReadOnly)
		float currentProgress;
	UPROPERTY(BlueprintReadOnly)
		float totalProgress;

private:
	enum { SLOT_NUMBER, PRODUCTION_UNIT};
	const int MAX_QUEUE_SIZE = 5;

	class ADNG_RTSBarrack *focusingBarrack;

private:

protected:
	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UImage *selectionBoxImage;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UGridPanel *commandPanel;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UCanvasPanel *entityInformCanvas;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UGridPanel *entityCanvas;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UCanvasPanel *productionInformCanvas;

	UPROPERTY(BlueprintReadWrite, Category = "RTS_UI")
		UProgressBar *productionProgress;

	UPROPERTY(BlueprintReadWrite, Category = "RTS_UI")
		TArray<UWidgetSwitcher*> queueSlots;

public:
};