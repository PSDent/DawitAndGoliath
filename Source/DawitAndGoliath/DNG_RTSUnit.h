// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNG_RTSBaseObject.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "DNG_RTSUnit.generated.h"

/**
 * 
 */
UCLASS()
class DAWITANDGOLIATH_API ADNG_RTSUnit : public ADNG_RTSBaseObject
{
	GENERATED_BODY()
	
public:
	ADNG_RTSUnit();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Command
	UFUNCTION(BlueprintCallable, category = "RTSMelee")
		void Move();

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_Move();
	void Server_Move_Implementation();
	bool Server_Move_Validate() { return true; }

	UFUNCTION(BlueprintCallable, category = "RTSMelee")
		void Stop();
	UFUNCTION(BlueprintCallable, category = "RTSMelee")
		void Hold();
	UFUNCTION(BlueprintCallable, category = "RTSMelee")
		void Patrol();
	UFUNCTION()
		virtual void Attack();
	UFUNCTION(BlueprintCallable, category = "RTSMelee")
		virtual void Deal(AActor *targetActor);
	UFUNCTION(BlueprintCallable, category = "RTSUnit")
		virtual void Check();

private:
	void CompareDistance();

protected:
	


public:



private:
	FTimerDelegate attackDelayDele;
	FTimerHandle attackDelayTimerHandle;

protected:
	TArray<class AFPSCharacter*> enemyPlayers;
	AActor *target;

	UPROPERTY(BlueprintReadWrite, category = "RTSUnit")
		UBlackboardComponent *blackBoard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
		UBlackboardData * useBB;
		//TSubclassOf<UBlackboardData> useBlackBoard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
		UBehaviorTree *useBT;
		//TSubclassOf<UBehaviorTree> behaviorTree;

	//UPROPERTY(BlueprintReadOnly, category = "RTSUnit")
	//	TSubclassOf<UBlackboardData> blackBoardAsset;

	//UPROPERTY(BlueprintReadOnly, category = "RTSUnit")
	//	TSubclassOf<UBehaviorTree> behaviorTreeAsset;

	float fireRange;
	float fireRate;
	float damage;

	UPROPERTY(BlueprintReadOnly, category = "RTSUnit")
		bool bIsWalk;

	bool bIsHold;

	// BlackBoard Key
	FName key_IsCanDeal = "IsCanDeal";


};
