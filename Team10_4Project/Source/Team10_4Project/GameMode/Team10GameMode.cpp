// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/Team10GameMode.h"

#include "Character/CivilianPlayerController.h"
#include "Character/CivilianPlayerState.h"
#include "GameState/Team10GameState.h"

ATeam10GameMode::ATeam10GameMode()
{
	GameStateClass = ATeam10GameState::StaticClass();
	PlayerControllerClass = ACivilianPlayerController::StaticClass();
	PlayerStateClass = ACivilianPlayerState::StaticClass();

	//CurrentArea = EGameArea::None;
	//CurrentPhase = EGamePhase::None;
}

void ATeam10GameMode::BeginPlay()
{
	Super::BeginPlay();

	Team10GameState = Cast<ATeam10GameState>(GameState);
	if (Team10GameState)
	{
		//CurrentPhase = EGamePhase::Lobby;
		Team10GameState->SetCurrentPhase(EGamePhase::DayPhase);
	}
	//UE_LOG(LogTemp, Log, TEXT("Ingame %d"), Team10GameState->PlayerArray.Num());
	AssignInfectedPlayers();
}

void ATeam10GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATeam10GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// UE_LOG(LogTemp, Log, TEXT("Player Logged In InGame: %s"), *NewPlayer->GetName());
	//
	// if (!Team10GameState)
	// {
	// 	return;
	// }
	
	// ACivilianPlayerController* CivilianPlayerController = Cast<ACivilianPlayerController>(NewPlayer);
	// if (CivilianPlayerController)
	// {
	// 	Team10GameState->AllPlayers.Add(CivilianPlayerController); 
	// 	
	// 	if (Team10GameState->CurrentPhase == EGamePhase::Lobby)
	// 	{
	// 		if (Team10GameState->AllPlayers.Num() >= Team10GameState->PlayerArray.Num())
	// 		{
	// 			FTimerHandle TestTimerHandle;
	// 			GetWorldTimerManager().SetTimer(TestTimerHandle, this, &ThisClass::StartGame, 5.f);
	// 			
	// 			// 추후 모든 플레이어가 준비되면 게임을 시작하게 변경할 수 있음.
	// 			StartGame();
	// 		}
	// 	}
	// }
}

void ATeam10GameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	UE_LOG(LogTemp, Log, TEXT("Player Logged Out: %s"), *Exiting->GetName());
	
	if (Team10GameState)
	{
		ACivilianPlayerController* CivilianPlayerController = Cast<ACivilianPlayerController>(Exiting);
		if (CivilianPlayerController)
		{
			Team10GameState->AllPlayers.Remove(CivilianPlayerController);
			
			 if (Team10GameState->CurrentPhase != EGamePhase::Lobby && Team10GameState->CurrentPhase != EGamePhase::GameEnd)
			 {
			 	ACivilianPlayerState* CivilianPlayerState = Cast<ACivilianPlayerState>(CivilianPlayerController->PlayerState);
			
			 	if (!CivilianPlayerState)
			 	{
			 		return;
			 	}
			 	
			 	// if (CivilianPlayerState->role == Citizen && CivilianPlayerState->IsAlive)
			 	// {
			 	// 	Team10GameState->AliveCitizenCount--;
			 	// }
			 	// else if (CivilianPlayerState->role == Infecter && CivilianPlayerState->IsAlive)
			 	// {
			 	// 	Team10GameState->AliveInfectedCount--;
			 	// }
			 	CheckWinCondition();
			 }
		}
	}
}


void ATeam10GameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	UE_LOG(LogTemp, Log, TEXT("Ingame HandleStartingNewPlayer_Implementation"));
	
	LoadedPlayerCount++;
	
	if (LoadedPlayerCount == Team10GameState->PlayerArray.Num())
	{
		// 모든 플레이어의 로딩이 끝나면 로딩 UI를 해제한다.
		UE_LOG(LogTemp, Log, TEXT("All Player Loaded"));

		StartGame();
	}
	
}

void ATeam10GameMode::StartGame()
{
	if (Team10GameState->CurrentPhase != EGamePhase::Lobby)
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Game Starting!"));

	
	
	ChangePhase(EGamePhase::DayPhase);
	
}

void ATeam10GameMode::ChangePhase(EGamePhase NewPhase)
{
	//Team10GameState->CurrentPhase = NewPhase;
	Team10GameState->SetCurrentPhase(NewPhase);
	
	switch (Team10GameState->CurrentPhase)
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
void ATeam10GameMode::StartPhaseTimer(float Duration)
{
	GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
	GetWorldTimerManager().SetTimer(PhaseTimerHandle, this, &ATeam10GameMode::UpdatePhaseTimer, 1.f, true, 1.f);

	if (Team10GameState)
	{
		Team10GameState->SetPhaseTimeRemaining(Duration);
	}
}

void ATeam10GameMode::UpdatePhaseTimer()
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

void ATeam10GameMode::AdvanceToNextPhase()
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

void ATeam10GameMode::OpenNextArea()
{
	// 퓨즈 박스를 일정개수 활성화 하면 다음 구역이 열리고 이전 구역은 자기장 
}

void ATeam10GameMode::CheckWinCondition()
{
	if (!Team10GameState)
	{
		return;
	}
	
	if (GetAliveCitizenCount()== 0)
	{
		Team10GameState->SetGameResult(EGameResult::InfectedWin);
	}
	else if (GetAliveInfectedCount() == 0)
	{
		Team10GameState->SetGameResult(EGameResult::CitizenWin);
	}
}

void ATeam10GameMode::EndGame(EGameResult Result)
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

void ATeam10GameMode::HandlePlayerDeath(APlayerController* DeadPlayer)
{
	// DeadPlayer의 역할에 맞춰 AliveCount감소

	// if (!DeadPlayer)
	// {
	// 	return;
	// }
	//
	// if(!Team10GameState)
	// {
	// 	return;
	// }
	// Team10GameState->AliveCitizenCount--;
	// Team10GameState->AliveInfectedCount--;
	//
	// APawn* Pawn = DeadPlayer->GetPawn();
	// if (Pawn)
	// {
	// 	Pawn->Destroy();
	// }
	// CheckWinCondition();
}

void ATeam10GameMode::AssignInfectedPlayers()
{
	if (!Team10GameState)
	{
		return;
	}
	
	int32 PlayerCount = Team10GameState->AllPlayers.Num();
	
	TArray<int32> RandomInfectedArray;
	
	for (int i = 0; i < PlayerCount ; i++)
	{
		RandomInfectedArray.Add(i);
	}

	for (int32 i = RandomInfectedArray.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		//UE_LOG(LogTemp, Warning, TEXT("RandomInfectedIndex: %d"), j);
		RandomInfectedArray.Swap(i, j);
	}

	// 인덱스를 랜덤으로 섞고 랜덤한 인덱스 값에 해당하는 플레이어를 찾아 감염자로 설정한다.
	
	// for (int i = 0; i < InfectedCount; i++)
	// {
	// 	Team10GameState->AllPlayers[RandomInfectedArray[i]];
	// }
	
	// for (int i = InfectedCount; i < PlayerCount; i++)
	// {
	// 	Team10GameState->AllPlayers[RandomInfectedArray[i]];
	// }

	Team10GameState->AliveInfectedCount = InfectedCount;
	Team10GameState->AliveCitizenCount = PlayerCount - InfectedCount;
}

int32 ATeam10GameMode::GetAliveCitizenCount()
{
	return Team10GameState->AliveCitizenCount;
}

int32 ATeam10GameMode::GetAliveInfectedCount()
{
	return Team10GameState->AliveInfectedCount;
}
