// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "DNGProperty.h"
#include "GameFramework/Pawn.h"
#include "TestEnemyPawn.generated.h"

UCLASS()
class DAWITANDGOLIATH_API ATestEnemyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATestEnemyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UDNGProperty* Prop;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void DealDmg(float dmg);

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};
