// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSHUD.h"
#include "Engine.h"

ADNG_RTSHUD::ADNG_RTSHUD() : Super()
{
	minimapPointSize = 5.0f;
}

void ADNG_RTSHUD::BeginPlay()
{
	Super::BeginPlay();

	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, "HUD On");
}

void ADNG_RTSHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADNG_RTSHUD::DrawHUD()
{
	Super::DrawHUD();
/*
	FVector2D viewSize;
	rtsViewPort->GetViewportSize(viewSize);

	for (auto actor : *minimapPointArray)
	{
		FVector pos = actor->GetActorLocation();
		float xPos = (pos.Y - leftBot_Point.Y) / mapRatio;
		float yPos = viewSize.Y - (pos.X - leftBot_Point.X) / mapRatio;
		DrawRect(FLinearColor::Green, xPos, yPos, minimapPointSize, minimapPointSize);
	}*/
}

//void ADNG_RTSHUD::OnPaint()
//{
//	FVector2D viewSize;
//	rtsViewPort->GetViewportSize(viewSize);
//
//	for (auto actor : *minimapPointArray)
//	{
//		FVector pos = actor->GetActorLocation();
//		float xPos = (pos.Y - leftBot_Point.Y) / mapRatio;
//		float yPos = viewSize.Y - (pos.X - leftBot_Point.X) / mapRatio;
//		DrawRect(FLinearColor::Green, xPos, yPos, minimapPointSize, minimapPointSize);
//	}
//}