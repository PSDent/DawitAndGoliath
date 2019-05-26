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
	void DisplayUnitInform(class ADNG_RTSBaseObject*);

	void SetCommandOnPanel(FCommandInfo cmdInfo);
	void ResetCommandOnPanel();
	void SetMouseCursor();

	// Getter
	UImage* GetSelectionBoxImage() { return selectionBoxImage; };
	UGridPanel* GetCommandPanel() { return commandPanel; };
	
	UFUNCTION(BlueprintCallable)
		void SendToPawnPanelInfo(FString key);
private:


protected:
	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UImage *selectionBoxImage;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UGridPanel *commandPanel;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UCanvas *entityInformCanvas;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UCanvas *entityCanvas;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UCanvas *productionInformCanvas;

public:
};