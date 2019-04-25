// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameStateBase.h"

AMyGameStateBase::AMyGameStateBase()
{

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
