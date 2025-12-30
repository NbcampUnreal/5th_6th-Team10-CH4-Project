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

void ATeam10GameState::OnRep_ChangedArea()
{
	if (OnAreaChanged.IsBound())
	{
		switch(CurrentArea)
		{
		case EGameArea::Area1:
			OnAreaChanged.Broadcast(GamePlayTags::AreaTag::Area_Area1);
			break;
		case EGameArea::Area2:
			OnAreaChanged.Broadcast(GamePlayTags::AreaTag::Area_Area2);
			break;
		case EGameArea::Area3:
			OnAreaChanged.Broadcast(GamePlayTags::AreaTag::Area_Area3);
			break;
		case EGameArea::Exit:
			OnAreaChanged.Broadcast(GamePlayTags::AreaTag::Area_Exit);
			default:
			break;
		}
	}
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
	if (OnGamePhaseChanged.IsBound())
	{
		switch(CurrentPhase)
		{
		case EGamePhase::Lobby:
			OnGamePhaseChanged.Broadcast(EGamePhase::Lobby);
			break;
		case EGamePhase::DayPhase:
			OnGamePhaseChanged.Broadcast(EGamePhase::DayPhase);
			break;
		case EGamePhase::NightPhase:
			OnGamePhaseChanged.Broadcast(EGamePhase::NightPhase);
			break;
		case EGamePhase::TrapIn:
			OnGamePhaseChanged.Broadcast(EGamePhase::TrapIn);
		case EGamePhase::GameEnd:
			OnGamePhaseChanged.Broadcast(EGamePhase::GameEnd);
			break;
			default:
			break;
		}
	}
	
}

void ATeam10GameState::OnRep_PhaseTimeRemaining()
{
	// Widget에 델리게이트 신호 보내기
	if (OnPhaseTimeChanged.IsBound())
	{
		OnPhaseTimeChanged.Broadcast(PhaseTimeRemaining);
	}
	//UE_LOG(LogTemp, Log, TEXT("Current Phase Timer: %d"), PhaseTimeRemaining);
}

void ATeam10GameState::OnRep_RemainingFuseBoxCount()
{
	
}

void ATeam10GameState::OnRep_GameResult()
{
	// UI 설정
}

void ATeam10GameState::SetCurrentArea(EGameArea NewArea)
{
	if (HasAuthority())
	{
		CurrentArea = NewArea;
		OnRep_ChangedArea();
	}
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
