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
	void SetCommandOnPanel(FCommandInfo cmdInfo);

	// DPI 스케일 문제로 인해 마우스 커서 위치와
	// 셀렉트 박스 위치가 일치하지 않는다. 
	// 이를 잘 조정하여 할 것. 
	// 부탁이다.

private:
	
protected:

public:
	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UImage *selectionBoxImage;

	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UGridPanel *commandPanel;
};