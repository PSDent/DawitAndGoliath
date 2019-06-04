// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Components/ActorComponent.h"
#include "DNGProperty.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DAWITANDGOLIATH_API UDNGProperty : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDNGProperty();	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
		float Hp;
	UPROPERTY(VisibleAnyWhere, Category = "Stat")
		float MaxHp;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual float GetMaxHp();
	virtual float GetHp();
	virtual void SetMaxHp(float hp);
	virtual void SetHp(float hp);

	virtual void DealDamage(float dmg);
};
