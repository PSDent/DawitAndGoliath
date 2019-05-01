// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameStateBase.h"
#include "UnrealNetwork.h"
#include "Engine.h"

AMyGameStateBase::AMyGameStateBase()
{
	SetReplicates(true);
	lobbyKey.Add("Button_Shooter1");
	lobbyKey.Add("Button_Shooter2");
	lobbyKey.Add("Button_Shooter3");
	lobbyKey.Add("Button_Shooter4");
	lobbyKey.Add("Button_RTS");

	lobbyValue.Add("None");
	lobbyValue.Add("None");
	lobbyValue.Add("None");
	lobbyValue.Add("None");
	lobbyValue.Add("None");
}

void AMyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyGameStateBase, lobbyKey);
	DOREPLIFETIME(AMyGameStateBase, lobbyValue);

}

void AMyGameStateBase::SetSessionPlayersInfo(FSessionPlayersInfo &sessionPlayersInfo)
{
	this->sessionPlayersInfo.playersRole_TMap.Reset();
	for (auto iter = sessionPlayersInfo.playersRole_TMap.CreateConstIterator(); iter; ++iter)
	{
		this->sessionPlayersInfo.playersRole_TMap.Add(iter.Key(), iter.Value());
	}
}

FSessionPlayersInfo AMyGameStateBase::GetSessionPlayersInfo()
{
	return this->sessionPlayersInfo;
}


void AMyGameStateBase::ChoosePlayerRole(const FString &roleName, const FName &playerName)
{
	int32 index;
	lobbyKey.Find(roleName, index);

	if (lobbyValue[index] == "None")
	{
		int32 playerExist;

		// 이미 한 번 선택했던 경우
		if (lobbyValue.Find(playerName, playerExist))
		{
			lobbyValue[playerExist] = "None";
		}

		int32 deployPos;
		lobbyKey.Find(roleName, deployPos);
		lobbyValue[deployPos] = playerName;
	}
}

TArray<FString> AMyGameStateBase::GetLobbyKey()
{
	return lobbyKey;
}

TArray<FName> AMyGameStateBase::GetLobbyValue()
{
	return lobbyValue;
}