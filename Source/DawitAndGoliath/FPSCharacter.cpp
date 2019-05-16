	// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "TestEnemyPawn.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	SpringArm3rd = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm3rd"));
	SpringArm3rd->SetupAttachment(RootComponent);
	SpringArm3rd->RelativeRotation = FRotator(20.f, 0.f, 0.f);
	SpringArm3rd->TargetArmLength = 320.f;
	SpringArm3rd->bEnableCameraLag = true;
	SpringArm3rd->CameraLagSpeed = 100.f;
	SpringArm3rd->bUsePawnControlRotation = true;

	Cam3rd = CreateDefaultSubobject<UCameraComponent>(FName(TEXT("Camera3rd")));
	Cam3rd->SetupAttachment(SpringArm3rd, USpringArmComponent::SocketName);
	SpringArm3rd->SetNetAddressable();
	SpringArm3rd->SetNetAddressable();
	SpringArm3rd->SetIsReplicated(true);
	SpringArm3rd->SetIsReplicated(true);

	ConstructorHelpers::FObjectFinder<USoundBase> gunFireSound(
		TEXT("/Game/Sounds/WR_fire")
	);

	ConstructorHelpers::FObjectFinder<UAnimMontage> fireMontage(
		TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Fire_A_Fast_V1_Montage")
	);

	ConstructorHelpers::FObjectFinder<UParticleSystem> fireParticle(
		TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitCharacter")
	);

	ConstructorHelpers::FObjectFinder<UParticleSystem> muzzleFlame(
		TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash")
	);

	GunFireSound = gunFireSound.Object;
	FireMontage = fireMontage.Object;
	FireParticle = fireParticle.Object;
	MuzzleFlame = muzzleFlame.Object;

	IsFireable = true;
	IsLeftMousePressed = false;
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
			Cam3rd->RelativeLocation = c->RelativeLocation;

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "FPSCHARACTER");
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLeftMousePressed && IsFireable)
	{
		FRotator rot = Cam3rd->GetComponentRotation();
		rot.Yaw = FMath::RandRange(rot.Yaw - 1.f, rot.Yaw + 1.f);
		rot.Pitch = FMath::RandRange(rot.Pitch - 1.f, rot.Pitch + 1.f);
		Fire(Cam3rd->GetComponentLocation(), rot);
	}
}

void AFPSCharacter::Fire(FVector loc, FRotator rot)
{
	if (Role == ROLE_Authority)
	{
		MulticastFire(loc, rot);
	}
	else
	{
		ServerFire(loc, rot);
	}
}

void AFPSCharacter::ServerFire_Implementation(FVector loc, FRotator rot)
{
	Fire(loc, rot);
}

bool AFPSCharacter::ServerFire_Validate(FVector loc, FRotator rot)
{
	return true;
}

void AFPSCharacter::MulticastFire_Implementation(FVector loc, FRotator rot)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, "Fire");

	AActor* target = GetRaycastTarget(loc, rot, 10000.0f);
	if (target && target->IsA(APawn::StaticClass()))
	{
		DealDamage(this, target, 2);
	}

	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		GunFireSound,
		RootComponent->GetComponentLocation() + FVector(100.f, 0.f, 0.f)
	);
	IsFireable = false;
	PlayAnimMontage(FireMontage, 1.f);
	GetWorldTimerManager().SetTimer(GunFireTimerHandle, this, &AFPSCharacter::EnableFire, 0.1f);
}

bool AFPSCharacter::MulticastFire_Validate(FVector loc, FRotator rot)
{
	return true;
}

AActor* AFPSCharacter::GetRaycastTarget(FVector loc, FRotator rot, float length)
{
	FCollisionQueryParams params(FName(TEXT("PlayerAimCheck")), true);
	params.bTraceAsyncScene = true;
	params.bReturnPhysicalMaterial = true;

	FHitResult hit(ForceInit);

	FVector start = loc;
	FVector end = loc + (rot.Vector() * length);

	FTransform soc = GetMesh()->GetSocketTransform(TEXT("gun_barrel"));
	if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params))
	{
		GetWorld()->LineTraceSingleByChannel(
			hit,
			soc.GetLocation(),
			end,
			ECC_Visibility,
			params
		);

		if (!hit.GetActor()) return nullptr;

		if (!hit.GetActor()->IsA(APawn::StaticClass()))
			DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 3.0f);
		else
			DrawDebugLine(GetWorld(), soc.GetLocation(), hit.Location, FColor::Yellow, false, 3.0f);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticle, hit.Location, FRotator::ZeroRotator);
	}

	//FRotator socRot = rot.Add(0, 90, 0);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlame,
		soc.GetLocation(), rot);

	return hit.GetActor();
}

void AFPSCharacter::DealDamage(AActor* dealer, AActor* target, float dmg)
{
	TArray<UActorComponent*> arr;
	target->GetComponents(arr);

	for (UActorComponent* c : arr)
		if (c->IsA(UDNGProperty::StaticClass()))
		{
			UDNGProperty* prop = Cast<UDNGProperty>(c);
			if (prop)
				prop->DealDamage(dmg);
		}
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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

	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AFPSCharacter::OnMousePressed()
{
	IsLeftMousePressed = true;
}

void AFPSCharacter::OnMouseReleased()
{
	IsLeftMousePressed = false;
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