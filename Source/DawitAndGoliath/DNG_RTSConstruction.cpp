// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSConstruction.h"
#include "Particles/ParticleSystemComponent.h"

ADNG_RTSConstruction::ADNG_RTSConstruction() : Super()
{
	bReplicates = true;
	bIsMovable = false;
}

void ADNG_RTSConstruction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADNG_RTSConstruction::BeginPlay()
{
	Super::BeginPlay();

}

void ADNG_RTSConstruction::Die()
{
	if (Role == ROLE_Authority)
	{
		Super::Die();

		Multicast_DestructionEffect();

		bIsSelected = false;
		bIsAlive = false;

		destructionDele.BindLambda(
			[&]() {
			Destroy();
		});
		GetWorld()->GetTimerManager().SetTimer(destructionTimer, destructionDele, 0.1f, false, destructionTime);
	}
	else
	{
		Server_ConstructionDie();
	}
}

void ADNG_RTSConstruction::Server_ConstructionDie_Implementation()
{
	Die();
}

void ADNG_RTSConstruction::Multicast_DestructionEffect_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), destructionFX, GetActorLocation(), FRotator::ZeroRotator);
}