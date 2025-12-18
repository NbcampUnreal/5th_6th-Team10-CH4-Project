
#include "GameMode/Manager/GameFlowManager.h"
#include "GameState/Team10GameState.h"

void UGameFlowManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void UGameFlowManager::StartGame()
{
	Team10GameState = GetWorld()->GetGameState<ATeam10GameState>();
	
	if (Team10GameState->CurrentPhase != EGamePhase::Lobby)
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Game Starting!"));
	
	ChangePhase(EGamePhase::DayPhase);
}

void UGameFlowManager::ChangePhase(EGamePhase NewPhase)
{
	Team10GameState->SetCurrentPhase(NewPhase);
	
	switch (NewPhase)
	{
	case EGamePhase::DayPhase:
		StartPhaseTimer(DayNightPhaseDuration);
		UE_LOG(LogTemp, Warning, TEXT("ChangePhase: DayPhase"));
		break;
	case EGamePhase::NightPhase:
		StartPhaseTimer(DayNightPhaseDuration);
		UE_LOG(LogTemp, Warning, TEXT("ChangePhase: Night"));
		break;
	case EGamePhase::TrapIn:
		StartPhaseTimer(TrapInPhaseDuration);
		UE_LOG(LogTemp, Warning, TEXT("ChangePhase: TrapIn"));
		break;
	case EGamePhase::GameEnd:
		UE_LOG(LogTemp, Warning, TEXT("ChangePhase: Game End"));
		break;
	default:
		break;
	}
}
void UGameFlowManager::EndGame(EGameResult Result)
{
	if (!Team10GameState)
	{
		return;
	}
	ChangePhase(EGamePhase::GameEnd);
	Team10GameState->SetGameResult(Result);

	UE_LOG(LogTemp, Warning, TEXT("Game Ended! Result: %d"), (int32)Result);
	// 나중에 수정
	
	// FTimerHandle EndGameTimer;
	// GetWorldTimerManager().SetTimer(EndGameTimer, [this]()
	// {
	// 	for (APlayerController* PlayerController : Team10GameState->AllPlayers)
	// 	{
	// 		if (PlayerController)
	// 		{
	// 			//결과 화면 UI 표시
	// 		}
	// 	}
	// }, 5.0f, false);
}
void UGameFlowManager::StartPhaseTimer(float Duration)
{
	GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(PhaseTimerHandle, this, &UGameFlowManager::UpdatePhaseTimer, 1.f, true, 1.f);
	
	//GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
	//GetWorldTimerManager().SetTimer(PhaseTimerHandle, this, &ATeam10GameMode::UpdatePhaseTimer, 1.f, true, 1.f);

	if (Team10GameState)
	{
		Team10GameState->SetPhaseTimeRemaining(Duration);
	}
}

void UGameFlowManager::UpdatePhaseTimer()
{
	if (Team10GameState)
	{
		Team10GameState->DecreaseRemainTimer();
		
		if (Team10GameState->PhaseTimeRemaining <= 0)
		{
			AdvanceToNextPhase();
		}

		UE_LOG(LogTemp, Log, TEXT("Current Phase Timer: %d"), Team10GameState->PhaseTimeRemaining);
	}
}

void UGameFlowManager::AdvanceToNextPhase()
{
	switch (Team10GameState->CurrentPhase)
	{
	case EGamePhase::DayPhase:
		ChangePhase(EGamePhase::NightPhase);
		break;
	case EGamePhase::NightPhase:
		ChangePhase(EGamePhase::TrapIn);
		break;
	case EGamePhase::TrapIn:
		// Trap In Phase에서 타이머가 끝나면 강제로 감염자 팀 승리
		EndGame(EGameResult::InfectedWin);
		break;
	default:
		break;
	}
}
