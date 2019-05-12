// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS_UI.h"
#include "Engine.h"

URTS_UI::URTS_UI(const FObjectInitializer &objInitializer) : Super(objInitializer)
{
	//selectionBoxImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	//selectionBoxImage->SetColorAndOpacity(FLinearColor(0.15076f, 0.489583f, 0.09085f, 0.5f));
}


void URTS_UI::DrawBox(FVector2D start, FVector2D end)
{
	selectionBoxImage->SetVisibility(ESlateVisibility::Visible);
	UCanvasPanelSlot *test = Cast<UCanvasPanelSlot>(selectionBoxImage->Slot);

	test->SetPosition(FVector2D(start.X, start.Y));

	
	float sizeX = end.X - start.X;
	float sizeY = end.Y - start.Y;
	FVector2D size(sizeX, sizeY);
	test->SetSize(size);
}

void URTS_UI::SetCommandOnPanel(FCommandInfo cmdInfo)
{
	int index = cmdInfo.row * 4 + cmdInfo.column;

	UUserWidget *cmdBtn = Cast<UUserWidget>(commandPanel->GetChildAt(index));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, cmdBtn->GetName());

	UTextBlock *shortCutText = Cast<UTextBlock>(cmdBtn->WidgetTree->FindWidget("ShortCut"));
	UTextBlock *cmdNameText = Cast<UTextBlock>(cmdBtn->WidgetTree->FindWidget("Name"));
	shortCutText->SetText(FText::FromString(cmdInfo.shortCut.ToString()));
	cmdNameText->SetText(FText::FromString(cmdInfo.name));
}