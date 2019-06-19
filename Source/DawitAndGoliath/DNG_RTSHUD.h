// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DNG_RTSHUD.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	ADNG_RTSHUD();

	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	void SetLeftBotPoint(FVector point) { leftBot_Point = point; }
	void SetMapRatio(float ratio) { mapRatio = ratio; }
	void SetRtsViewPort(UGameViewportClient *viewPort) { rtsViewPort = viewPort; }
	void SetMinimapPointArray(TArray<AActor*> *pointArray) { minimapPointArray = pointArray; }

private:
	void DrawHUD() override;

protected:

public:

private:
	FVector leftBot_Point;
	float mapRatio;
	UGameViewportClient *rtsViewPort;
	TArray<AActor*> *minimapPointArray;
	
	float minimapPointSize;

protected:

};
