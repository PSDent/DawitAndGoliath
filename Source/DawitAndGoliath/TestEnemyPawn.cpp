// Fill out your copyright notice in the Description page of Project Settings.

#define LLOCTEXT_NAMESPACE "TestEnemyPawnActions"
#include "TestEnemyPawn.h"
#include "DNGProperty.h"
#include "UnrealNetwork.h"

// Sets default values
ATestEnemyPawn::ATestEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	Prop = CreateDefaultSubobject<UDNGProperty>(TEXT("Fuck you"));
}

// Called when the game starts or when spawned
void ATestEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	if (Prop)
	{
		Prop->SetMaxHp(100);
		Prop->SetHp(100);
	}
}

// Called every frame
void ATestEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATestEnemyPawn::DealDmg(float dmg)
{
	

}

void ATestEnemyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}
