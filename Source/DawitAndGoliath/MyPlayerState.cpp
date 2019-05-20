// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerState.h"
#include "Engine.h"
#include "FPSCharacter.h"
#include "DNG_RTSPawn.h"

AMyPlayerState::AMyPlayerState()
{
	bHost = false;
}

void AMyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	gameState = GetWorld()->GetGameState<AMyGameStateBase>();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "MYPLAYERSTATE");

}

void AMyPlayerState::ChoosePlayerRoleInterface_Implementation(const FString &roleName, const FName &playerNameParam)
{
	ChoosePlayerRole(roleName, playerNameParam);
}

void AMyPlayerState::ChoosePlayerRole(const FString &roleName, const FName &playerNameParam)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Called ChoosePlayerRole");

	if (Role < ROLE_Authority)
	{
		ServerChoosePlayerRole(roleName, playerNameParam);
	}
	else
	{
		MulticastChoosePlayerRole(roleName, playerNameParam);
	}

	
}

 void AMyPlayerState::ServerChoosePlayerRole_Implementation(const FString &roleName, const FName &playerNameParam)
{
	 GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Called ServerChoosePlayerRole");
	 ChoosePlayerRole(roleName, playerNameParam);
}

 bool AMyPlayerState::ServerChoosePlayerRole_Validate(const FString &roleName, const FName &playerNameParam)
 {
	 return true;
 }

 void AMyPlayerState::MulticastChoosePlayerRole_Implementation(const FString &roleName, const FName &playerNameParam)
 {
	 gameState->ChoosePlayerRole(roleName, playerNameParam);
 }
 
 bool AMyPlayerState::MulticastChoosePlayerRole_Validate(const FString &roleName, const FName &playerNameParam)
 {
	 return true;
 }

 void AMyPlayerState::SetHost()
 {
	 bHost = true;
 }

 bool AMyPlayerState::GetHost()
 {
	 return bHost;
 }

 void AMyPlayerState::TestPlz()
 {
	 ServerTestPlz();
 }

 void AMyPlayerState::ServerTestPlz_Implementation()
 {
	 if (GetNetOwningPlayer()->GetPlayerController(GetWorld()))
	 {
		 GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "Success");

		 UClass *character = LoadObject<UClass>(this, *FString("Class'/Game/CharacterBP/FPSCharacter.FPSCharacter_C'"));
		 APawn * pa = GetWorld()->SpawnActor<AFPSCharacter>(character);
		 GetNetOwningPlayer()->GetPlayerController(GetWorld())->Possess(pa);
		 if (pa)
			 GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, "PAWANAWASDAWD");

		 //GetNetOwningPlayer()->GetPlayerController(GetWorld())->Possess(pawn);
	 }
 }

 bool AMyPlayerState::ServerTestPlz_Validate()
 {
	 return true;
 }

 void AMyPlayerState::SetPossessPawn(APawn *pawn)
 {
	 GetNetOwningPlayer()->GetPlayerController(GetWorld())->Possess(pawn);
	 //myPossessPawn = pawn;
 }

 APawn* AMyPlayerState::GetPossessPawn()
 {
	 return myPossessPawn;
 }

 void AMyPlayerState::SetPlayRole(FString roleName)
 {
	 playRoleName = roleName;
 }

 FString AMyPlayerState::GetPlayRole()
 {
	 return playRoleName;
 }

 void AMyPlayerState::CopyProperties(APlayerState * PlayerState)
 {
	 Super::CopyProperties(PlayerState);

	 if (PlayerState != nullptr)
	 {
		 AMyPlayerState *myState = Cast<AMyPlayerState>(PlayerState);
		 if (myState)
		 {
			 myState->playRoleName = playRoleName;
		 }
	 }
 }