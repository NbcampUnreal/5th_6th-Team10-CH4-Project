
#include "GameMode/Manager/GameFlowManager.h"

#include "PlayerSpawnManager.h"
#include "GameState/Team10GameState.h"
#include "EngineUtils.h"
#include "Gimmick/Actors/FuseBoxActor.h"

void UGameFlowManager::BeginPlay()
{
	Super::BeginPlay();

	Team10GameState = GetWorld()->GetGameState<ATeam10GameState>();

	Team10GameState->OnAreaChanged.AddDynamic(this, &UGameFlowManager::AreaChanged);

	if (ATeam10GameMode* GameMode = Cast<ATeam10GameMode>(GetOwner()))
	{
		PlayerSpawnManager = GameMode->FindComponentByClass<UPlayerSpawnManager>();
	}
}

void UGameFlowManager::InitializeRemainingFuseBoxes()
{
	// 기존 코드
	/*if (Team10GameState)
	{
		Team10GameState->RemainingFuseBoxCount = TotalFuseBoxCount;
	}*/

	// 금성 - 새로 추가한 코드
	if (!Team10GameState) return;

	int32 Count = 0;
	EGameArea TargetArea = Team10GameState->CurrentArea;

	// 현재 월드에 배치된 모든 AFuseBoxActor를 순회
	for (TActorIterator<AFuseBoxActor> It(GetWorld()); It; ++It)
	{
		// 액터의 BelongingArea가 현재 게임 구역과 일치하는지 확인
		// (FuseBoxActor.h에 BelongingArea가 public 또는 getter로 선언되어 있어야 함)
		if (It->GetBelongingArea() == TargetArea)
		{
			Count++;
		}
	}

	Team10GameState->RemainingFuseBoxCount = Count;
	UE_LOG(LogTemp, Warning, TEXT("Area %d initialized with %d Fuses"), (int32)TargetArea, Count);
}

void UGameFlowManager::StartGame()
{
	// if (Team10GameState->CurrentPhase != EGamePhase::Lobby)
	// {
	// 	return;
	// }
	
	UE_LOG(LogTemp, Warning, TEXT("Game Starting!"));
	SetCurrentArea(EGameArea::Area1);
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
void UGameFlowManager::OpenNextArea()
{
	switch (Team10GameState->CurrentArea)
	{
	case EGameArea::Area1:
		SetCurrentArea(EGameArea::Area2);
		break;
	case EGameArea::Area2:
		SetCurrentArea(EGameArea::Area3);
		break;
	case EGameArea::Area3:
		ActivateExit();
		break;
	default:
		break;
	}
}

void UGameFlowManager::ActivateExit()
{
	// 탈출구 활성화 
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

void UGameFlowManager::AreaChanged(FGameplayTag AreaTag)
{
	if (PlayerSpawnManager)
	{
		PlayerSpawnManager->FoundPlayerSpawner(AreaTag);
	}
	
	InitializeRemainingFuseBoxes();
	ChangePhase(EGamePhase::DayPhase);

	UE_LOG(LogTemp, Error, TEXT("Area Changed"));
}

void UGameFlowManager::StartPhaseTimer(float Duration)
{
	GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(PhaseTimerHandle, this, &UGameFlowManager::UpdatePhaseTimer, 1.f, true, 1.f);
	
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

void UGameFlowManager::SetCurrentArea(EGameArea NewArea)
{
	if (Team10GameState)
	{
		Team10GameState->SetCurrentArea(NewArea);
		
	}
}

