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

	// DPI ������ ������ ���� ���콺 Ŀ�� ��ġ��
	// ����Ʈ �ڽ� ��ġ�� ��ġ���� �ʴ´�. 
	// �̸� �� �����Ͽ� �� ��. 
	// ��Ź�̴�.

private:
	
protected:

public:
	UPROPERTY(BlueprintReadWrite, category = "RTS_UI")
		UImage *selectionBoxImage;

};