// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/DecalComponent.h"
#include "Materials/Material.h"
#include "DNG_RTSUnitAIController.h"
#include "DNGProperty.h"
#include "DNGStruct.h"
#include "DNG_RTSBaseObject.generated.h"

UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSBaseObject : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADNG_RTSBaseObject();

	// Setter
	void SetPawn(class ADNG_RTSPawn *pawn) { this->pawn = pawn; };
	void SetSelectedStatus(bool status);

	// Getter
	bool GetIsMovable() { return bIsMovable; };
	bool GetSelectedStatus() { return bIsSelected; };
	TMap<FKey, const FCommandInfo>& GetCmdInfoMap() { return commandInfoMap; };
	FString GetUnitName() { return unitName; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual void OnDied() = 0;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;

public:
	UDNGProperty *objProperty;

private:

protected:
	UDecalComponent *ringDecal;

	UPROPERTY(Replicated)
		class ADNG_RTSPawn *pawn;

	TMap<FKey, const FCommandInfo> commandInfoMap;

	FTimerDelegate commandCheckDele;
	FTimerHandle commandCheckHandle;

	UPROPERTY(Replicated)
		ADNG_RTSUnitAIController *aiController;

	FString unitName;

	bool bIsSelected;
	bool bIsMovable;

public:
};