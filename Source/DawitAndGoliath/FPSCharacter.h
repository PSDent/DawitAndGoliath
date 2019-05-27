// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AreaWeapon.h"
#include "Gun.h"
#include "FireParam.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

UCLASS()
class DAWITANDGOLIATH_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Fire(FFireParam params);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire(FFireParam params);

	virtual void ServerFire_Implementation(FFireParam params);

	virtual bool ServerFire_Validate(FFireParam params);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastFire(FFireParam params);

	virtual void MulticastFire_Implementation(FFireParam params);

	virtual bool MulticastFire_Validate(FFireParam params);

	virtual bool CheckFlameHit(FVector socLoc, AActor* target);

	//virtual void DealDamage(AActor* dealer, AActor* target, float dmg);

	virtual void OnMousePressed();

	virtual void OnMouseReleased();

	virtual void MoveForward(float amount);

	virtual void MoveRight(float amount);

	virtual void RotateYaw(float amount);

	virtual void RotatePitch(float amount);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	virtual void Jump() override;

	virtual void Heal();

	virtual void EnableHeal();

	virtual void GiveDamage(AActor* target, float dmg, FVector, FRotator);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerGiveDamage(AActor* target, float dmg, FVector loc, FRotator rot);

	virtual void ServerGiveDamage_Implementation(AActor* target, float dmg, FVector, FRotator);

	virtual bool ServerGiveDamage_Validate(AActor* target, float dmg, FVector, FRotator);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastGiveDamage(AActor* target, float dmg, FVector loc, FRotator rot);

	virtual void MulticastGiveDamage_Implementation(AActor* target, float dmg, FVector, FRotator);

	virtual bool MulticastGiveDamage_Validate(AActor* target, float dmg, FVector, FRotator);

	virtual void Boost();

	virtual void SetBoost();

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void ServerSetBoost(bool value);

	virtual void ServerSetBoost_Implementation(bool value);
	
	virtual bool ServerSetBoost_Validate(bool value);

	UFUNCTION(Client, Reliable, WithValidation)
		virtual void ClientSetBoost(bool value);

	virtual void ClientSetBoost_Implementation(bool value);

	virtual bool ClientSetBoost_Validate(bool value);

	virtual void EnableFire() { IsFireable = true; }
	
	USpringArmComponent* SpringArm3rd;

	UPROPERTY(Replicated)
	UCameraComponent* Cam3rd;

	UCharacterMovementComponent* MovementComponent;

	USoundBase* GunFireSound;

	UAnimMontage* FireMontage;

	UAnimMontage* BoostMontage;

	FTimerHandle GunFireTimerHandle;

	FTimerHandle HealTimerHandle;

	FTimerHandle ReloadTimerHandle;

	UParticleSystem* FireParticle;

	UParticleSystem* FlameParticle;

	UParticleSystem* MuzzleFlame;

	UParticleSystem* BoosterParticle;

	UDNGProperty* Prop;

	FTimerDelegate FireDele;

	//UPROPERTY(Transient, Replicated)
	TArray<UWeapon*> Weapons;

	UPROPERTY(Transient, Replicated)
	UWeapon* CurrentWeapon;

	EWeaponType CurrentWeaponType;

	float SplitRange;

	bool IsLeftMousePressed;

	bool IsReloading;

	UPROPERTY(BlueprintReadOnly, category = "FPS_State", Replicated)
		bool IsBoosting;

	bool IsFireable = true;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsHealable;


	template <EWeaponType T>
	void ChangeWeapon();

	virtual void ChangeWeapon(EWeaponType type);

	UFUNCTION(BlueprintCallable)
		float GetHp();

	UFUNCTION(BlueprintCallable)
		float GetAmmoPer();

	UFUNCTION(BlueprintCallable)
		float GetReloadTimePer();

	//UFUNCTION(Server, Reliable, WithValidation)
	//virtual void ServerSetWeapon(EWeaponType type);

	//virtual void ServerSetWeapon_Implementation(EWeaponType type);

	//virtual bool ServerSetWeapon_Validate(EWeaponType type);
};
