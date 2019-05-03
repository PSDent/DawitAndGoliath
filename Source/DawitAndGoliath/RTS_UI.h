// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Components/PanelSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
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
	void DrawBox(float lx, float ly, float rx, float ry);

	// DPI 스케일 문제로 인해 마우스 커서 위치와
	// 셀렉트 박스 위치가 일치하지 않는다. 
	// 이를 잘 조정하여 할 것. 
	// 부탁이다.

private:
	
protected:

public:
	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UImage *selectionBoxImage;

};