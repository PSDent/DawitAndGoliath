// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS_UI.h"
#include "Engine.h"

URTS_UI::URTS_UI(const FObjectInitializer &objInitializer) : Super(objInitializer)
{
	//selectionBoxImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	//selectionBoxImage->SetColorAndOpacity(FLinearColor(0.15076f, 0.489583f, 0.09085f, 0.5f));
}


void URTS_UI::DrawBox(float lx, float ly, float rx, float ry)
{
	selectionBoxImage->SetVisibility(ESlateVisibility::Visible);
	UCanvasPanelSlot *test = Cast<UCanvasPanelSlot>(selectionBoxImage->Slot);

	test->SetPosition(FVector2D(lx, ly));

	float sizeX = rx - lx;
	float sizeY = ry - ly;
	FVector2D size(sizeX, sizeY);
	test->SetSize(size);
}

