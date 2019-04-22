// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	virtual void Fire(FVector loc, FRotator rot);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire(FVector loc, FRotator rot);

	virtual void ServerFire_Implementation(FVector loc, FRotator rot);

	virtual bool ServerFire_Validate(FVector loc, FRotator rot);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastFire(FVector loc, FRotator rot);

	virtual void MulticastFire_Implementation(FVector loc, FRotator rot);

	virtual bool MulticastFire_Validate(FVector loc, FRotator rot);

	virtual AActor* GetRaycastTarget(FVector loc, FRotator rot, float length);

	virtual void DealDamage(AActor* dealer, AActor* target, float dmg);

	virtual void OnMousePressed();

	virtual void OnMouseReleased();

	virtual void MoveForward(float amount);

	virtual void MoveRight(float amount);

	virtual void RotateYaw(float amount);

	virtual void RotatePitch(float amount);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	virtual void Jump() override;

	virtual void EnableFire();

	USpringArmComponent* SpringArm3rd;

	UCameraComponent* Cam3rd;

	USoundBase* GunFireSound;

	UAnimMontage* FireMontage;	//기본 연사 0.4초간격

	FTimerHandle GunFireTimerHandle;

	UParticleSystem* FireParticle;

	UParticleSystem* MuzzleFlame;

	bool IsLeftMousePressed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsFireable;
};
