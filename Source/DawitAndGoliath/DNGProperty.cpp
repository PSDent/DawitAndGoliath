// Fill out your copyright notice in the Description page of Project Settings.

#include "DNGProperty.h"

// Sets default values for this component's properties
UDNGProperty::UDNGProperty()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDNGProperty::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDNGProperty::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UDNGProperty::GetMaxHp() { return MaxHp; }

float UDNGProperty::GetHp() { return Hp; }

void UDNGProperty::SetMaxHp(float hp) { MaxHp = hp; }

void UDNGProperty::SetHp(float hp) { Hp = hp; }

void UDNGProperty::DealDamage(float dmg)
{
	if (Hp - dmg > MaxHp)
	{
		Hp = MaxHp;
		if (GetOwner()->Role == ROLE_Authority)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Stop! It's already a corpse..."));
	}
	else if (Hp <= dmg)	
	{
		Hp = 0;
		//��� ó��
		if (GetOwner()->Role == ROLE_Authority)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Dead"));
	}
	else
	{
		Hp -= dmg;
		if (GetOwner()->Role == ROLE_Authority)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Dealt %.0f, Left : %.0f"), dmg, Hp));
	}
}

