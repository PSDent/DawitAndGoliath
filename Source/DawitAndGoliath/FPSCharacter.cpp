// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "TestEnemyPawn.h"
#include "UnrealNetwork.h"
#include "FireParam.h"
#include "CollisionQueryParams.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	IsHealable = true;
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

	Prop = CreateDefaultSubobject<UDNGProperty>(TEXT("Property"));

	ConstructorHelpers::FObjectFinder<UAnimMontage> fireMontage(
		TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Fire_A_Fast_V1_Montage")
	);

	ConstructorHelpers::FObjectFinder<USoundBase> gunFireSound(
		TEXT("/Game/Sounds/WR_fire")
	);

	ConstructorHelpers::FObjectFinder<UParticleSystem> fireParticle(
		TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitCharacter")
	);

	ConstructorHelpers::FObjectFinder<UParticleSystem> flameParticle(
		TEXT("/Game/ParagonMinions/FX/Particles/Minions/Dragon/FX/P_Dragon_Fireball_MuzzleFlash")
	);	

	ConstructorHelpers::FObjectFinder<UParticleSystem> muzzleFlame(
		TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash")
	);

	ConstructorHelpers::FObjectFinder<UParticleSystem> boosterParticle(
		TEXT("/Game/ParagonWraith/FX/Particles/Abilities/ScopedShot/FX/P_Wraith_Sniper_Stabilizer.P_Wraith_Sniper_Stabilizer")
	);

	ConstructorHelpers::FObjectFinder<UAnimMontage> boostMontage(
		TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/TravelMode_Transition_InMotion_Montage")
	);

	FireParticle = fireParticle.Object;
	FlameParticle = flameParticle.Object;
	MuzzleFlame = muzzleFlame.Object;
	BoosterParticle = boosterParticle.Object;
	GunFireSound = gunFireSound.Object;
	FireMontage = fireMontage.Object;
	BoostMontage = boostMontage.Object;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;
	bUseControllerRotationPitch = false;

	MovementComponent = GetCharacterMovement();

	if (Prop)
	{
		Prop->SetMaxHp(100);
		Prop->SetHp(100);
	}
	if (Prop)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Hp : %f"), Prop->GetHp()));

	TArray<USceneComponent*> comps;
	SpringArm3rd->GetChildrenComponents(true, comps);
	for (USceneComponent* c : comps)
		if (c->GetFName() == FName(TEXT("DefaultStatePos")))
			Cam3rd->RelativeLocation = c->RelativeLocation;
	
	UGun* gun = NewObject<UGun>();
	gun->GunInit(TEXT("Rifle"), 20, 0.1f, 10000, 1.0f, 2.f, 30, GunFireSound);
	gun->SetParticle(FireParticle, MuzzleFlame);
	Weapons.Add(gun);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::FromInt(Weapons.Num()));
	gun = NewObject<UGun>();
	gun->GunInit(TEXT("MachineGun"), 10, 0.06f, 10000, 3.0f, 6.f, 80, GunFireSound);
	gun->SetParticle(FireParticle, MuzzleFlame);
	Weapons.Add(gun);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::FromInt(Weapons.Num()));

	//CurrentWeapon = Weapons[0];
	UAreaWeapon* flame = NewObject<UAreaWeapon>();
	flame->SetParticle(FlameParticle, MuzzleFlame);
	UStaticMeshComponent* comp = nullptr;

	comps.Empty();
	RootComponent->GetChildrenComponents(true, comps);
	for (USceneComponent* c : comps)
		if (c->GetFName() == FName(TEXT("Cone")))
			comp = Cast<UStaticMeshComponent>(c);	

	flame->AreaInit(TEXT("Rifle"), 2.f, 0.04f, 3.5f, 100, GunFireSound, comp, FireParticle, MuzzleFlame);
	flame->GetAreaMesh()->OnComponentBeginOverlap.AddDynamic(flame, &UAreaWeapon::OnOverlapBegin);
	flame->GetAreaMesh()->OnComponentEndOverlap.AddDynamic(flame, &UAreaWeapon::OnOverlapEnd);
	Weapons.Add(flame);

	//GetWorld()->GetGameViewport()->SetMouseLockMode(EMouseLockMode::LockAlways);


	ChangeWeapon<EWeaponType::Rifle>();
	FireDele.BindLambda([&] {
		if (CurrentWeapon->UseBullet() < 0)
		{
			if (!GetWorldTimerManager().IsTimerActive(ReloadTimerHandle))
				GetWorldTimerManager().SetTimer(ReloadTimerHandle, CurrentWeapon, &UWeapon::Reload, CurrentWeapon->GetReloadTime(), false);

			return;
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::FromInt(CurrentWeapon->GetBulletCount()));

		FVector socLoc = GetMesh()->GetSocketLocation(TEXT("gun_barrel"));

		if (CurrentWeapon->IsA(UGun::StaticClass()))
			Fire(FFireParam());
		else
		{
			if (CurrentWeapon != nullptr)
			{
				UAreaWeapon* wep = Cast<UAreaWeapon>(CurrentWeapon);
				if (wep)
				{
					TArray<AActor*> targets = wep->GetTargets();

					if (targets.Num() == 0) GiveDamage(nullptr, 0, socLoc, GetActorRotation() + FRotator(-90.0f, 0, -90.0f));
					else
						for (AActor* target : targets)
						{
							GiveDamage(target, CurrentWeapon->GetDamage(), socLoc, GetActorRotation() + FRotator(-90.0f, 0, -90.0f));

						}
				}
			}
		}
	});

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "FPSCHARACTER");
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsBoosting)
		Boost();
}

void AFPSCharacter::Fire(FFireParam params)
{

	if (Role == ROLE_Authority)
	{
		MulticastFire(params);
	}
	else
	{
		params.WeaponType = CurrentWeaponType;

		params.Location = Cam3rd->GetComponentLocation();
		params.Rotation = Cam3rd->GetComponentRotation();
		params.World = GetWorld();
		params.Damage = CurrentWeapon->GetDamage();
		params.AttackRate = CurrentWeapon->GetAttackRate();
		params.Range = CurrentWeapon->GetRange();
		params.SocketLocation = GetMesh()->GetSocketLocation(TEXT("gun_barrel"));
		params.IsGun = CurrentWeapon->IsA(UGun::StaticClass());

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
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
		MuzzleFlame,
		params.SocketLocation, params.Rotation);
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		GunFireSound,
		RootComponent->GetComponentLocation()
	);
	PlayAnimMontage(FireMontage, 1.f);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::FromInt((int)CurrentWeaponType));


	if (params.IsGun)
	{
		//CurrentWeapon �ٿ��� ��� ũ���� (���� �ٲٸ鼭 �� ��ٺ��� ���� ũ����)
		FHitResult hit = CurrentWeapon->GetTarget(params.Location, params.SocketLocation, params.Rotation, GetWorld(), this, params.Range);
		AActor* target = hit.GetActor();
		if (target)
		{
			if (target->IsA(ATestEnemyPawn::StaticClass()))
			{
				TArray<UActorComponent*> arr;
				target->GetComponents(arr);

				for (UActorComponent* c : arr)
					if (c->IsA(UDNGProperty::StaticClass()))
					{
						//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("WTF"));
						UDNGProperty* prop = Cast<UDNGProperty>(c);
						if (prop)
						{
							prop->DealDamage(params.Damage);

						}
					}
			}
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticle, hit.Location, FRotator::ZeroRotator);

		}
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::SanitizeFloat(params.AttackRate));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Fire"));
	}
}

void AFPSCharacter::GiveDamage(AActor* target, float dmg, FVector loc, FRotator rot)
{
	if (Role == ROLE_Authority)
	{
		MulticastGiveDamage(target, dmg, loc, rot);
	}
	else
	{
		ServerGiveDamage(target, dmg, loc, rot);
	}
}

void AFPSCharacter::ServerGiveDamage_Implementation(AActor* target, float dmg, FVector loc, FRotator rot)
{
	GiveDamage(target, dmg, loc, rot);
}

bool AFPSCharacter::ServerGiveDamage_Validate(AActor* target, float dmg, FVector loc, FRotator rot)
{
	return true;
}

void AFPSCharacter::MulticastGiveDamage_Implementation(AActor* target, float dmg, FVector loc, FRotator rot)
{
	PlayAnimMontage(FireMontage, 1.f);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
		FlameParticle,
		loc, rot)->SetWorldScale3D(FVector(2, 3.5f, 2));

	if (!target || !CheckFlameHit(loc, target)) return;

	TArray<UActorComponent*> arr;
	target->GetComponents(arr);

	for (UActorComponent* c : arr)
		if (c->IsA(UDNGProperty::StaticClass()))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("WTF"));
			UDNGProperty* prop = Cast<UDNGProperty>(c);
			if (prop)
			{
				prop->DealDamage(dmg);
			}
		}
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticle, target->GetActorLocation(), FRotator::ZeroRotator);
}

bool AFPSCharacter::MulticastGiveDamage_Validate(AActor* target, float dmg, FVector loc, FRotator rot)
{
	return true;
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCharacter, Cam3rd);
	DOREPLIFETIME(AFPSCharacter, CurrentWeapon);
	DOREPLIFETIME(AFPSCharacter, IsBoosting);
	//DOREPLIFETIME(AFPSCharacter, Weapons);

}

bool AFPSCharacter::CheckFlameHit(FVector socLoc, AActor* target)
{
	FCollisionQueryParams params(FName(TEXT("PlayerAimCheck")), true);
	params.bTraceAsyncScene = true;
	params.bReturnPhysicalMaterial = true;
	params.AddIgnoredActor(this);

	FHitResult hit(ForceInit);
	FVector end = target->GetActorLocation();

	GetWorld()->LineTraceSingleByChannel(
		hit,
		socLoc,
		end,
		ECC_GameTraceChannel1,
		params
	);

	AActor* actor = hit.GetActor();
	return (actor->IsA(ATestEnemyPawn::StaticClass()));
}

void AFPSCharacter::Heal()
{
	if (!IsHealable) return;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Heal"));
	IsHealable = false;
	Prop->DealDamage(-40);
	GetWorldTimerManager().SetTimer(HealTimerHandle, this, &AFPSCharacter::EnableHeal, 1.f, false);

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
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	switch (type)
	{
	case EWeaponType::Rifle:
		CurrentWeapon = Weapons[0];
		CurrentWeaponType = EWeaponType::Rifle;
		break;
	case EWeaponType::MachineGun:
		CurrentWeapon = Weapons[1];
		CurrentWeaponType = EWeaponType::MachineGun;
		break;
	case EWeaponType::FlameThrower:
		CurrentWeapon = Weapons[2];
		CurrentWeaponType = EWeaponType::FlameThrower;
		break;
	default:
		CurrentWeapon = Weapons[0];

	}

	if (CurrentWeapon && CurrentWeapon->IsA(UGun::StaticClass()))
	{
		UGun* gun = Cast<UGun>(CurrentWeapon);
		if (gun)
			SplitRange = gun->GetSplitRange();
	}
	else
	{
		UAreaWeapon* wep = Cast<UAreaWeapon>(CurrentWeapon);
		if (wep)
			wep->GetTargets().Empty();
	}

	if (IsLeftMousePressed)
	{

		GetWorldTimerManager().ClearTimer(GunFireTimerHandle);
		GetWorldTimerManager().SetTimer(GunFireTimerHandle, FireDele, CurrentWeapon->GetAttackRate(), true, 0.0f);
	}
}

template <EWeaponType T>
void AFPSCharacter::ChangeWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Change weapon"));
	ChangeWeapon(T);
}

void AFPSCharacter::EnableHeal()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Heal Ready"));
	IsHealable = true;
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
	PlayerInputComponent->BindAction("Heal", IE_Pressed, this, &AFPSCharacter::Heal);
	PlayerInputComponent->BindAction("LeftFire", IE_Pressed, this, &AFPSCharacter::OnMousePressed);
	PlayerInputComponent->BindAction("LeftFire", IE_Released, this, &AFPSCharacter::OnMouseReleased);
	PlayerInputComponent->BindAction("Rifle", IE_Pressed, this, &AFPSCharacter::ChangeWeapon<EWeaponType::Rifle>);
	PlayerInputComponent->BindAction("MachineGun", IE_Pressed, this, &AFPSCharacter::ChangeWeapon<EWeaponType::MachineGun>);
	PlayerInputComponent->BindAction("Flame", IE_Pressed, this, &AFPSCharacter::ChangeWeapon<EWeaponType::FlameThrower>);
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &AFPSCharacter::SetBoost);
	PlayerInputComponent->BindAction("Boost", IE_Released, this, &AFPSCharacter::SetBoost);

	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AFPSCharacter::Boost()
{

}

void AFPSCharacter::SetBoost()
{
	IsBoosting = !IsBoosting;

	ServerSetBoost(IsBoosting);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::FromInt(MovementComponent->MaxWalkSpeed));
}

void AFPSCharacter::ServerSetBoost_Implementation(bool value)
{
	if (value)
		MovementComponent->MaxWalkSpeed = 1200;
	else
		MovementComponent->MaxWalkSpeed = 600;

	ClientSetBoost(value);
}

bool AFPSCharacter::ServerSetBoost_Validate(bool valye)
{
	return true;
}

void AFPSCharacter::ClientSetBoost_Implementation(bool value)
{
	if (value)
		MovementComponent->MaxWalkSpeed = 1200;
	else
		MovementComponent->MaxWalkSpeed = 600;
}

bool AFPSCharacter::ClientSetBoost_Validate(bool valye)
{
	return true;
}

void AFPSCharacter::OnMousePressed()
{
	IsLeftMousePressed = true;

	//SplitRange = Cast<UGun>(CurrentWeapon)->GetSplitRange();
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

float AFPSCharacter::GetHp()
{
	return Prop->GetHp();
}

float AFPSCharacter::GetAmmoPer()
{
	return (float)CurrentWeapon->GetBulletCount() / CurrentWeapon->GetMaxBulletCount();
}

float AFPSCharacter::GetReloadTimePer()
{
	return GetWorldTimerManager().GetTimerRemaining(ReloadTimerHandle) / CurrentWeapon->GetReloadTime();
}