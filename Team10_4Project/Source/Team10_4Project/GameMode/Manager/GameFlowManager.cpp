
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

	// 현재 구역에 해당하는 태그 결정
	FGameplayTag TargetTag;
	switch (Team10GameState->CurrentArea)
	{
	case EGameArea::Area1: TargetTag = GamePlayTags::AreaTag::Area_Area1; break;
	case EGameArea::Area2: TargetTag = GamePlayTags::AreaTag::Area_Area2; break;
	case EGameArea::Area3: TargetTag = GamePlayTags::AreaTag::Area_Area3; break;
	default: return;
	}

	int32 Count = 0;
	for (TActorIterator<AFuseBoxActor> It(GetWorld()); It; ++It)
	{
		// Enum이 아닌 Tag 비교로 개수를 셉니다.
		if (It->GetBelongingArea().MatchesTagExact(TargetTag))
		{
			Count++;
		}
	}

	Team10GameState->RemainingFuseBoxCount = Count;
	UE_LOG(LogTemp, Warning, TEXT("Area Tag %s Initialized: %d Boxes"), *TargetTag.ToString(), Count);
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

