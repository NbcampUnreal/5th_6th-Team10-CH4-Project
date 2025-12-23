// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/Team10GameState.h"

#include "Character/CivilianPlayerController.h"
#include "Net/UnrealNetwork.h"

ATeam10GameState::ATeam10GameState()
{
	bReplicates = true;

	AliveCitizenCount = 0;
	AliveInfectedCount = 0;
}

void ATeam10GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentPhase);
	DOREPLIFETIME(ThisClass, PhaseTimeRemaining);
	DOREPLIFETIME(ThisClass, CurrentArea);
	DOREPLIFETIME(ThisClass, GameResult);
	DOREPLIFETIME(ThisClass, AliveCitizenCount);
	DOREPLIFETIME(ThisClass, AliveInfectedCount);
}

void ATeam10GameState::OnRep_CurrentArea()
{
	
}

void ATeam10GameState::OnRep_KillPlayerVotesCount()
{
	// UI 설정
}

void ATeam10GameState::OnRep_CurrentVotesCount()
{
	
}

void ATeam10GameState::OnRep_CurrentPhase()
{
	// UI 설정

	// for (ACivilianPlayerController* CivilianPlayerController : AllPlayers)
	// {
	// 	CivilianPlayerController->
	// }
}

void ATeam10GameState::OnRep_PhaseTimeRemaining()
{
	// UI 설정 

	//UE_LOG(LogTemp, Log, TEXT("Current Phase Timer: %d"), PhaseTimeRemaining);
}

void ATeam10GameState::OnRep_RemainingFuseBoxCount()
{
	
}

void ATeam10GameState::OnRep_GameResult()
{
	// UI 설정
}

void ATeam10GameState::SetCurrentPhase(EGamePhase NewPhase)
{
	if (HasAuthority())
	{
		CurrentPhase = NewPhase;
	}
}

void ATeam10GameState::SetPhaseTimeRemaining(float Time)
{
	if (HasAuthority())
	{
		PhaseTimeRemaining = Time;
	}
}

void ATeam10GameState::SetGameResult(EGameResult Result)
{
	if (HasAuthority())
	{
		GameResult = Result;
	}
}

void ATeam10GameState::DecreaseRemainTimer()
{
	if (HasAuthority())
	{
		PhaseTimeRemaining--;
	}
}
