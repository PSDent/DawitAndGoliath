// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyGameStateBase.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMyPlayerState();

	void BeginPlay() override;

	void TestPlz();
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerTestPlz();
	virtual void ServerTestPlz_Implementation();
	virtual bool ServerTestPlz_Validate();

	UFUNCTION(BlueprintCallable)
		void SetHost();

	UFUNCTION(BlueprintCallable)
		bool GetHost();

	void ChoosePlayerRole(const FString &roleName, const FName &playerNameParam);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Network", meta = (DisplayName = "ChoosePlayerRoleInterface"))
		void ChoosePlayerRoleInterface(const FString &roleName, const FName &playerNameParam);
	virtual void ChoosePlayerRoleInterface_Implementation(const FString &roleName, const FName &playerNameParam);

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerChoosePlayerRole(const FString &roleName, const FName &playerNameParam);
	virtual void ServerChoosePlayerRole_Implementation(const FString &roleName, const FName &playerNameParam);
	virtual bool ServerChoosePlayerRole_Validate(const FString &roleName, const FName &playerNameParam);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void MulticastChoosePlayerRole(const FString &roleName, const FName &playerNameParam);
	virtual void MulticastChoosePlayerRole_Implementation(const FString &roleName, const FName &playerNameParam);
	virtual bool MulticastChoosePlayerRole_Validate(const FString &roleName, const FName &playerNameParam);

	void CopyProperties(APlayerState * PlayerState) override;


	void SetPossessPawn(APawn *pawn);
	APawn* GetPossessPawn();

	void SetPlayRole(FString roleName);
	FString GetPlayRole();

public:
	// 옮길 변수들은 무조건 BluePrintReadonly 프로퍼티를 붙일 것.
	UPROPERTY(BlueprintReadOnly)
		FString playRoleName;

private:
	bool bHost;
	AMyGameStateBase *gameState;
	APawn *myPossessPawn;
};