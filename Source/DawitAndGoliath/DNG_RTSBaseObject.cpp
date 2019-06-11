// Fill out your copyright notice in the Description page of Project Settings.

#include "DNG_RTSBaseObject.h"
#include "DNGProperty.h"
#include "DNGDelegates.h"
#include "DNG_RTSPawn.h"
#include "RTS_UI.h"
#include "MyPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Engine.h"

// Sets default values
ADNG_RTSBaseObject::ADNG_RTSBaseObject() : Super()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	ringDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionRing"));
	ringDecal->SetupAttachment(RootComponent);


	static ConstructorHelpers::FObjectFinder<UMaterial> decalMaterial(TEXT("Material'/Game/Blueprints/FriendlyRing.FriendlyRing'"));

	if (decalMaterial.Succeeded())
	{
		ringDecal->SetDecalMaterial(decalMaterial.Object);
		ringDecal->RelativeLocation = FVector(0.0f, 0.0f, -80.0f);
		ringDecal->DecalSize = FVector(32.f, 64.f, 64.f);
		ringDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
		ringDecal->SetVisibility(false);
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> minimapPointTextureObj(TEXT("Texture2D'/Engine/EditorResources/S_Actor.S_Actor'"));

	if (minimapPointTextureObj.Succeeded())
		minimapPointerTexture = minimapPointTextureObj.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> enemyPointMaterialObj(TEXT("Material'/Game/Sprite/EnemyPoint.EnemyPoint'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> friendlyPointMaterialObj(TEXT("Material'/Game/Sprite/FriendlyPoint.FriendlyPoint'"));

	if (friendlyPointMaterialObj.Succeeded())
		friendlyPointMaterial = friendlyPointMaterialObj.Object;

	if (enemyPointMaterialObj.Succeeded())
		enemyPointMaterial = enemyPointMaterialObj.Object;

	/*if (Cast<AMyPlayerState>(GetPlayerState()))
	{
		FString roleName = Cast<AMyPlayerState>(GetPlayerState())->playRoleName;
		if (roleName == "RTS")
			Client_Init();
	}*/
	Client_Init();

	objProperty = CreateDefaultSubobject<UDNGProperty>(TEXT("DNGProperty"));

	Tags.Add("Object");
	
	unitName = "Object";

	bIsSelected = false;
	bIsAlive = true;
	bIsCurrentSelected = false;
}

void ADNG_RTSBaseObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADNG_RTSBaseObject, aiController);
	DOREPLIFETIME(ADNG_RTSBaseObject, pawn);
	DOREPLIFETIME(ADNG_RTSBaseObject, bIsAlive);
	DOREPLIFETIME(ADNG_RTSBaseObject, attachSquadsArray);
	
}

// Called when the game starts or when spawned
void ADNG_RTSBaseObject::BeginPlay()
{
	Super::BeginPlay();

	aiController = Cast<ADNG_RTSUnitAIController>(Controller);
}

void ADNG_RTSBaseObject::SetSelectedTimer()
{
	bIsCurrentSelected = true;
	selectTimerDele.BindLambda([&]
	{
		bIsCurrentSelected = false;
	});
	GetWorld()->GetTimerManager().SetTimer(selectTimerHandle, selectTimerDele, 0.1f, false, 0.2f);
}

void ADNG_RTSBaseObject::Client_Init_Implementation()
{
	minimapPointer = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("MinimapPointer"));
	minimapPointer->SetupAttachment(RootComponent);
	minimapPointer->SetRelativeLocation(FVector(0, 0, 3000.0f));
	minimapPointer->SetRelativeScale3D(FVector(100.0f, 100.0f, 1.0f));
	minimapPointer->bHiddenInGame = false;
	minimapPointer->bVisible = true;
	minimapPointer->AddElement(friendlyPointMaterial, nullptr, false, 128.0f, 128.0f, nullptr);
}

void ADNG_RTSBaseObject::Client_AfterInit_Implementation()
{
	if (Cast<AMyPlayerState>(GetPlayerState()))
	{
		FString roleName = Cast<AMyPlayerState>(GetPlayerState())->playRoleName;
		if (roleName != "RTS")
			return;
	}

	minimapPointer = NewObject<UMaterialBillboardComponent>(this, TEXT("MinimapPointer"));
	minimapPointer->RegisterComponent();
	minimapPointer->AttachTo(GetRootComponent());
	minimapPointer->SetRelativeLocation(FVector(0, 0, 3000.0f));
	minimapPointer->SetRelativeScale3D(FVector(10.0f, 10.0f, 1.0f));
	minimapPointer->bHiddenInGame = false;
	minimapPointer->bVisible = true;
	minimapPointer->SetMaterial(0, friendlyPointMaterial);
	minimapPointer->AddElement(friendlyPointMaterial, nullptr, false, 128.0f, 128.0f, nullptr);
}

// Called every frame
void ADNG_RTSBaseObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADNG_RTSBaseObject::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ADNG_RTSBaseObject::SetSelectedStatus(bool status)
{
	if (!ringDecal) return;
	bIsSelected = status;

	if (bIsSelected)
	{
		ringDecal->SetVisibility(true);
	}
	else
		ringDecal->SetVisibility(false);
}

void ADNG_RTSBaseObject::AttachSquad(int num)
{
	if (Role == ROLE_Authority)
	{
		if (!attachSquadsArray.Contains(num))
		{
			attachSquadsArray.Add(num);
		}
	}
	else
	{
		Server_AttachSquad(num);
	}
}
void ADNG_RTSBaseObject::Server_AttachSquad_Implementation(int num)
{
	AttachSquad(num);
}

void ADNG_RTSBaseObject::RemoveMeFromSquad()
{
	if (Role == ROLE_Authority)
	{
		if (!pawn) return;

		for (auto squadNum : attachSquadsArray)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "REMOVE");

			pawn->RemoveFromSquad(this, squadNum);
		}
		pawn->RemoveFromSquad(this, -1);

		//Multicast_RemoveMeFromSquad();
	}
	else
	{
		Server_RemoveMeFromSquad();
	}
}

void ADNG_RTSBaseObject::Server_RemoveMeFromSquad_Implementation()
{
	RemoveMeFromSquad();
}

//void ADNG_RTSBaseObject::Multicast_RemoveMeFromSquad_Implementation()
//{
//	for (auto squadNum : attachSquadsArray)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "REMOVE");
//		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("%d"), squadNum));
//		pawn->RemoveFromSquad(this, squadNum);
//	}
//}

void ADNG_RTSBaseObject::Die()
{
	if (Role == ROLE_Authority)
	{
		RemoveMeFromSquad();

	}
	else
	{
		Server_Die();
	}
}

void ADNG_RTSBaseObject::Server_Die_Implementation()
{
	Die();
}