
// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS_UI.h"
#include "DNG_RTSPawn.h"
#include "Engine.h"
#include "DNG_RTSBaseObject.h"
#include "DNG_RTSBarrack.h"
#include "DNG_RTSUnit.h"
#include "DNGProperty.h"

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

	UTextBlock *shortCutText = Cast<UTextBlock>(cmdBtn->WidgetTree->FindWidget("ShortCut"));
	UTextBlock *cmdNameText = Cast<UTextBlock>(cmdBtn->WidgetTree->FindWidget("Name"));
	UButton *button = Cast<UButton>(cmdBtn->WidgetTree->FindWidget("Button_0"));
	
	shortCutText->SetText(FText::FromString(cmdInfo.shortCut.ToString()));
	cmdNameText->SetText(FText::FromString(cmdInfo.name));
	button->SetToolTipText(FText::FromString(cmdInfo.description));
}

void URTS_UI::ResetCommandOnPanel()
{
	for (int i = 0; i < 16; ++i)
	{
		UUserWidget *cmdBtn = Cast<UUserWidget>(commandPanel->GetChildAt(i));

		UTextBlock *shortCutText = Cast<UTextBlock>(cmdBtn->WidgetTree->FindWidget("ShortCut"));
		UTextBlock *cmdNameText = Cast<UTextBlock>(cmdBtn->WidgetTree->FindWidget("Name"));
		UButton *button = Cast<UButton>(cmdBtn->WidgetTree->FindWidget("Button_0"));

		shortCutText->SetText(FText::FromString(""));
		cmdNameText->SetText(FText::FromString(""));
		button->SetToolTipText(FText::FromString(""));
	}
}

void URTS_UI::SendToPawnPanelInfo(FString key)
{
	if (key == "")
		return;

	TArray<FKey> keyArray;
	EKeys::GetAllKeys(keyArray);
	for (int i = 0; keyArray.Num(); ++i)
	{
		if (keyArray[i].ToString() == key)
		{
			Cast<ADNG_RTSPawn>(GetOwningPlayer()->GetPawn())->ReceiveCmdPanel(keyArray[i]);
			return;
		}
	}
}

void URTS_UI::DisplayUnitInform(ADNG_RTSBaseObject *unit)
{
	ResetProductionInform();

	entityInformCanvas->SetVisibility(ESlateVisibility::Visible);
	UTextBlock *unitName = Cast<UTextBlock>(entityInformCanvas->GetChildAt(0));
	unitName->SetText(FText::FromString(unit->GetUnitName()));

	UTextBlock *currentHp = Cast<UTextBlock>(Cast<UHorizontalBox>(entityInformCanvas->GetChildAt(1))->GetChildAt(0));
	UTextBlock *maxHp = Cast<UTextBlock>(Cast<UHorizontalBox>(entityInformCanvas->GetChildAt(1))->GetChildAt(2));

	int val = unit->objProperty->GetHp();
	FText hpText = FText::FromString(FString::Printf(TEXT("%d"), val));
	currentHp->SetText(hpText);

	val = unit->objProperty->GetMaxHp();
	FText maxHpText = FText::FromString(FString::Printf(TEXT("%d"), val));
	maxHp->SetText(maxHpText);
}

void URTS_UI::ResetUnitInform()
{
	entityInformCanvas->SetVisibility(ESlateVisibility::Collapsed);
}

void URTS_UI::DisplayProductionInform(ADNG_RTSBaseObject *construction)
{
	//ResetUnitInform();
	DisplayUnitInform(construction);

	ADNG_RTSBarrack *barrack = Cast<ADNG_RTSBarrack>(construction);
	focusingBarrack = barrack;

	productionInformCanvas->SetVisibility(ESlateVisibility::Visible);
	productionProgress->SetVisibility(ESlateVisibility::Visible);

	TArray<TSubclassOf<ADNG_RTSUnit>> &spawnQueue = barrack->GetSpawnQueue();

	if (spawnQueue.Num())
	{
		currentProgress = barrack->spawnTime;
		totalProgress = barrack->spawnTotalTime;
	}
	else
	{
		currentProgress = 0;
		totalProgress = 0;
	}

	for (int i = 0; i < MAX_QUEUE_SIZE; ++i)
	{
		if (spawnQueue.Num() < i + 1)
		{
			queueSlots[i]->SetActiveWidgetIndex(SLOT_NUMBER);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::FromInt(queueSlots.Num()));

			queueSlots[i]->SetActiveWidgetIndex(PRODUCTION_UNIT);
			UUserWidget *productionButton = Cast<UUserWidget>(queueSlots[i]->GetChildAt(PRODUCTION_UNIT));
			UButton *slotButton = Cast<UButton>(productionButton->WidgetTree->FindWidget("SlotButton"));
			UTextBlock *text = Cast<UTextBlock>(slotButton->GetChildAt(0));
			ADNG_RTSUnit *queueUnit = spawnQueue[i].GetDefaultObject();  
			text->SetText(FText::FromString(queueUnit->initial));
		}
	}
}

void URTS_UI::ResetProductionInform()
{
	productionInformCanvas->SetVisibility(ESlateVisibility::Collapsed);
}

void URTS_UI::Display(ADNG_RTSBaseObject *obj)
{
	ADNG_RTSUnit *unit = Cast<ADNG_RTSUnit>(obj);

	if (unit)
	{
		focusingBarrack = nullptr;
		DisplayUnitInform(obj);
		return;
	}

	ADNG_RTSBarrack *barrack = Cast<ADNG_RTSBarrack>(obj);
	if (barrack)
	{
		DisplayProductionInform(obj);
		return;
	}
}

void URTS_UI::ResetDisplay()
{
	ResetProductionInform();
	ResetUnitInform();
}

void URTS_UI::RemoveQueueElement(int index)
{
	if(focusingBarrack)
		focusingBarrack->RemoveQueueElement(index);
}


void URTS_UI::DisplayUnitEntity(TArray<ADNG_RTSBaseObject*> &objects)
{

}

void URTS_UI::ResetUnitEntity()
{

}