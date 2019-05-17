// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNG_RTSBaseObject.h"

#include "BehaviorTree/BlackboardComponent.h"

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

	float fireRange;
	float fireRate;
	float damage;

	UPROPERTY(BlueprintReadOnly, category = "RTSUnit")
		bool bIsWalk;

	bool bIsHold;

	// BlackBoard Key
	FName bIsCanDeal = "IsCanDeal";


};
