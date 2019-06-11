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

	void AttachSquad(int num);
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_AttachSquad(int num);
	void Server_AttachSquad_Implementation(int num);
	bool Server_AttachSquad_Validate(int num) { return true; }

	UFUNCTION(Reliable, Client, WithValidation)
		virtual void Client_AfterInit();
	void Client_AfterInit_Implementation();
	bool Client_AfterInit_Validate() { return true; }

	void SetSelectedTimer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual void OnDied() = 0;

	virtual void Die();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_Die();
	void Server_Die_Implementation();
	bool Server_Die_Validate() { return true; }

	void RemoveMeFromSquad();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_RemoveMeFromSquad();
	void Server_RemoveMeFromSquad_Implementation();
	bool Server_RemoveMeFromSquad_Validate() { return true; }

	UFUNCTION(Reliable, Client, WithValidation)
		virtual void Client_Init();
	void Client_Init_Implementation();
	bool Client_Init_Validate() { return true; }



	//UFUNCTION(NetMulticast, Reliable, WithValidation)
	//	virtual void Multicast_RemoveMeFromSquad();
	//void Multicast_RemoveMeFromSquad_Implementation();
	//bool Multicast_RemoveMeFromSquad_Validate() { return true; };

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;

public:
	UDNGProperty *objProperty;

	UPROPERTY(Replicated)
		bool bIsAlive;

	bool bIsCurrentSelected;

	FString initial;

private:
	FTimerDelegate selectTimerDele;
	FTimerHandle selectTimerHandle;

	UTexture2D *minimapPointerTexture;
	UMaterialBillboardComponent *minimapPointer;

	UMaterial *friendlyPointMaterial;
	UMaterial *enemyPointMaterial;


protected:
	UPROPERTY(Replicated)
		TArray<int> attachSquadsArray;

	UDecalComponent *ringDecal;
	//UBillboardComponent *minimapPointer;

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