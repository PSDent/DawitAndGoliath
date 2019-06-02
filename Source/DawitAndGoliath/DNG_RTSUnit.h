// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DNG_RTSBaseObject.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

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
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_BeginPlay();
	void Server_BeginPlay_Implementation();
	bool Server_BeginPlay_Validate() { return true; }

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

public:
	// Command
	UFUNCTION(BlueprintCallable, category = "RTSMelee")
		void Move();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_Move(FVector dest);
	void Server_Move_Implementation(FVector dest);
	bool Server_Move_Validate(FVector dest) { return true; }

	UFUNCTION(BlueprintCallable, category = "RTSMelee")
		void Stop();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_Stop();
	void Server_Stop_Implementation();
	bool Server_Stop_Validate() { return true; }

	UFUNCTION(BlueprintCallable, category = "RTSMelee")
		void Hold();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_Hold();
	void Server_Hold_Implementation();
	bool Server_Hold_Validate() { return true; }

	UFUNCTION(BlueprintCallable, category = "RTSMelee")
		void Patrol();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_Patrol(const FVector &posOne, const FVector &posTwo);
	void Server_Patrol_Implementation(const FVector &posOne, const FVector &posTwo);
	bool Server_Patrol_Validate(const FVector &posOne, const FVector &posTwo) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
		virtual void Server_CheckPatrol();
	void Server_CheckPatrol_Implementation();
	bool Server_CheckPatrol_Validate() { return true; }

	UFUNCTION()
		void Attack();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_Attack(AActor *targetActor);
	void Server_Attack_Implementation(AActor *targetActor);
	bool Server_Attack_Validate(AActor *targetActor) { return true; }

	UFUNCTION(BlueprintCallable, category = "RTSMelee")
		void Deal();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_Deal();
	void Server_Deal_Implementation();
	bool Server_Deal_Validate() { return true; }

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		virtual void Multicast_FireEffect(FVector pos);
	void Multicast_FireEffect_Implementation(FVector pos);
	bool Multicast_FireEffect_Validate(FVector pos) { return true; };


	UFUNCTION(BlueprintCallable, category = "RTSUnit")
		virtual void Check();

private:
	void CompareDistance();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_CompareDistance();
	void Server_CompareDistance_Implementation();
	bool Server_CompareDistance_Validate() { return true; }

	void TurnToTarget();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_TurnToTarget();
	void Server_TurnToTarget_Implementation();
	bool Server_TurnToTarget_Validate() { return true; }

	UFUNCTION(BlueprintCallable)
		void Chase();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_Chase();
	void Server_Chase_Implementation();
	bool Server_Chase_Validate() { return true; }

protected:
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_Die();
	void Server_Die_Implementation();
	bool Server_Die_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
		virtual void Server_AfterDie();
	void Server_AfterDie_Implementation();
	bool Server_AfterDie_Validate() { return true; }

public:
	int spawnTime = 0;
	int supply;


private:
	FTimerDelegate afterDieDele;
	FTimerHandle afterDieTimerHandle;

	FTimerDelegate attackDelayDele;
	FTimerHandle attackDelayTimerHandle;

	class USphereComponent *patrolPointTriggerOne;
	class USphereComponent *patrolPointTriggerTwo;
	class USphereComponent *nextPatrolPointTrigger;

	FVector patrolPointOne;
	FVector patrolPointTwo;
	FVector nextPatrolPoint;

protected:
	TArray<class AFPSCharacter*> enemyPlayers;

	UPROPERTY(Replicated)
		AActor *target;

	UPROPERTY(BlueprintReadWrite, category = "RTSUnit")
		UBlackboardComponent *blackBoard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
		UBlackboardData *useBB;
		//TSubclassOf<UBlackboardData> useBlackBoard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
		UBehaviorTree *useBT;

	// Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS Unit")
		UParticleSystem *fireParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS Unit")
		UParticleSystem *hitParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS Unit")
		USoundCue *fireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS Unit")
		UAnimMontage *fireAnim;

	UAudioComponent *fireAudioComponent;

	FVector fireEffectPos;

	float deadDelay;
	float fireRange;
	float traceRange;
	float fireRate;
	float damage;


	UPROPERTY(Replicated, BlueprintReadOnly, category = "RTSUnit")
		bool bIsWalk;
	UPROPERTY(Replicated, BlueprintReadOnly, category = "RTSUnit")
		bool bIsDie;

	bool bIsHold;
	bool bIsMarkTarget;

	// BlackBoard Key
	FName key_IsCanDeal = "IsCanDeal";
	FName key_Target = "target";
	FName key_IsWantToDeal = "IsWantToDeal";
	FName key_IsPatrolling = "IsPatrolling";
	FName key_IsChasing = "IsChasing";
};
