// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "TestEnemyPawn.h"
#include "UnrealNetwork.h"
#include "FireParam.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	IsFireable = true;
	IsLeftMousePressed = false;

	SpringArm3rd = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm3rd"));
	SpringArm3rd->SetupAttachment(RootComponent);
	SpringArm3rd->RelativeRotation = FRotator(20.f, 0.f, 0.f);
	SpringArm3rd->TargetArmLength = 320.f;
	SpringArm3rd->bEnableCameraLag = true;
	SpringArm3rd->CameraLagSpeed = 100.f;
	SpringArm3rd->bUsePawnControlRotation = true;

	Cam3rd = CreateDefaultSubobject<UCameraComponent>(FName(TEXT("Camera3rd")));
	Cam3rd->SetupAttachment(SpringArm3rd, USpringArmComponent::SocketName);
	Cam3rd->SetIsReplicated(true);
	SpringArm3rd->SetNetAddressable();
	SpringArm3rd->SetIsReplicated(true);


	ConstructorHelpers::FObjectFinder<USoundBase> gunFireSound(
		TEXT("/Game/Sounds/WR_fire")
	);

	ConstructorHelpers::FObjectFinder<UParticleSystem> fireParticle(
		TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitCharacter")
	);

	ConstructorHelpers::FObjectFinder<UParticleSystem> muzzleFlame(
		TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash")
	);

	FireParticle = fireParticle.Object;
	MuzzleFlame = muzzleFlame.Object;
	GunFireSound = gunFireSound.Object;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;
	bUseControllerRotationPitch = false;

	TArray<USceneComponent*> comps;
	SpringArm3rd->GetChildrenComponents(true, comps);
	for (USceneComponent* c : comps)
		if (c->GetFName() == FName(TEXT("DefaultStatePos")))
		{
			Cam3rd->RelativeLocation = c->RelativeLocation;
		}


	UGun* gun = NewObject<UGun>();
	gun->GunInit(TEXT("Rifle"), 20, 0.1f, 10000, 1.0f, GunFireSound);
	gun->SetParticle(FireParticle, MuzzleFlame);
	Weapons.Add(gun);
	gun = NewObject<UGun>();
	gun->GunInit(TEXT("MachineGun"), 10, 0.03f, 10000, 3.0f, GunFireSound);
	gun->SetParticle(FireParticle, MuzzleFlame);
	Weapons.Add(gun);
	CurrentWeapon = Weapons[0];


	FireDele.BindLambda([&] {
		Fire(FFireParam());
	});

}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (IsLeftMousePressed && IsFireable)
	//{
	//	FFireParam params;

	//	Fire(params);
	//}
}

void AFPSCharacter::Fire(FFireParam params)
{

	if (Role == ROLE_Authority)
	{
		MulticastFire(params);
	}
	else
	{
		params.Location = Cam3rd->GetComponentLocation();
		params.Rotation = Cam3rd->GetComponentRotation();
		params.World = GetWorld();
		params.Damage = CurrentWeapon->GetDamage();
		params.AttackRate = CurrentWeapon->GetAttackRate();
		params.SocketLocation = GetMesh()->GetSocketLocation(TEXT("gun_barrel"));

		FRotator& rot = params.Rotation;
		rot.Yaw = FMath::RandRange(rot.Yaw - SplitRange, rot.Yaw + SplitRange);
		rot.Pitch = FMath::RandRange(rot.Pitch - SplitRange, rot.Pitch + SplitRange);
		ServerFire(params);
	}
}

void AFPSCharacter::ServerFire_Implementation(FFireParam params)
{
	Fire(params);
}

void AFPSCharacter::MulticastFire_Implementation(FFireParam params)
{
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		GunFireSound,
		RootComponent->GetComponentLocation()
	);

	//CurrentWeapon 줄에서 계속 크래시 (무기 바꾸면서 총 쏘다보면 가끔 크래시)
	AActor* target = CurrentWeapon->GetTarget(params.Location, params.SocketLocation, params.Rotation, GetWorld(), this);
	if (target)
	{
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticle, target->GetActorLocation(), FRotator::ZeroRotator);

	}
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::SanitizeFloat(params.AttackRate));
	IsFireable = false;
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCharacter, Cam3rd);
	DOREPLIFETIME(AFPSCharacter, CurrentWeapon);

}

bool AFPSCharacter::ServerFire_Validate(FFireParam params)
{
	return true;
}

bool AFPSCharacter::MulticastFire_Validate(FFireParam params)
{
	return true;
}

void AFPSCharacter::ChangeWeapon(EWeaponType type)
{
	switch (type)
	{
	case EWeaponType::Rifle:
		CurrentWeapon = Weapons[0];
		break;
	case EWeaponType::MachineGun:
		CurrentWeapon = Weapons[1];
		break;
	default:
		CurrentWeapon = Weapons[0];
	}

	if (IsLeftMousePressed)
	{
		GetWorldTimerManager().ClearTimer(GunFireTimerHandle);
		SplitRange = Cast<UGun>(CurrentWeapon)->GetSplitRange();
		GetWorldTimerManager().SetTimer(GunFireTimerHandle, FireDele, CurrentWeapon->GetAttackRate(), true, 0.0f);
	}
}

template <AFPSCharacter::EWeaponType T>
void AFPSCharacter::ChangeWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Change weapon"));
	ChangeWeapon(T);
}

void AFPSCharacter::EnableFire()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("GunFire Ready"));
	IsFireable = true;
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::RotateYaw);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::RotatePitch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::Jump);
	PlayerInputComponent->BindAction("LeftFire", IE_Pressed, this, &AFPSCharacter::OnMousePressed);
	PlayerInputComponent->BindAction("LeftFire", IE_Released, this, &AFPSCharacter::OnMouseReleased);
	PlayerInputComponent->BindAction("Rifle", IE_Pressed, this, &AFPSCharacter::ChangeWeapon<EWeaponType::Rifle>);
	PlayerInputComponent->BindAction("MachineGun", IE_Pressed, this, &AFPSCharacter::ChangeWeapon<EWeaponType::MachineGun>);

	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AFPSCharacter::OnMousePressed()
{
	IsLeftMousePressed = true;
	SplitRange = Cast<UGun>(CurrentWeapon)->GetSplitRange();
	GetWorldTimerManager().SetTimer(GunFireTimerHandle, FireDele, CurrentWeapon->GetAttackRate(), true, 0.0f);
}

void AFPSCharacter::OnMouseReleased()
{
	IsLeftMousePressed = false;
	GetWorldTimerManager().ClearTimer(GunFireTimerHandle);
}

void AFPSCharacter::MoveForward(float amount)
{
	if (amount)
	{
		FVector forwardVec = GetActorForwardVector();
		AddMovementInput(forwardVec, amount);
	}
}

void AFPSCharacter::MoveRight(float amount)
{
	if (amount)
	{
		FVector rightVec = GetActorRightVector();
		AddMovementInput(rightVec, amount);
	}
}

void AFPSCharacter::RotateYaw(float amount)
{
	if (amount)
	{
		AddControllerYawInput(amount);
	}
}

void AFPSCharacter::RotatePitch(float amount)
{
	if (amount)
	{
		AddControllerPitchInput(amount);
	}
}

void AFPSCharacter::Jump()
{
	Super::Jump();
}