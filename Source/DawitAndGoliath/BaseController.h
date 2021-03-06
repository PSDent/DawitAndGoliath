// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseController.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API ABaseController : public APlayerController
{
	GENERATED_BODY()

public:
	ABaseController();
	void Tick(float DeltaTime) override;
	
	void BeginPlay() override;
	void Possess(APawn *pawn) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void ShowGameSetUI(bool isRts);

	void ShowGameSetUI_Implementation(bool isRts);

private:

};