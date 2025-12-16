// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/Team10GameMode.h"

#include "EngineUtils.h"
#include "PlayerSpawn.h"
#include "Character/CivilianPlayerController.h"
#include "Character/CivilianPlayerState.h"
#include "GameState/Team10GameState.h"
#include "Manager/GameFlowManager.h"
#include "GameMode/GameTypes/AreaTag.h"

ATeam10GameMode::ATeam10GameMode()
{
	GameStateClass = ATeam10GameState::StaticClass();
	PlayerControllerClass = ACivilianPlayerController::StaticClass();
	PlayerStateClass = ACivilianPlayerState::StaticClass();

	//CurrentArea = EGameArea::None;
	//CurrentPhase = EGamePhase::None;

	GameFlowManager = CreateDefaultSubobject<UGameFlowManager>(TEXT("GameFlowManager"));
	
}

void ATeam10GameMode::BeginPlay()
{
	Super::BeginPlay();

	Team10GameState = Cast<ATeam10GameState>(GameState);
	// if (Team10GameState)
	// {
	// 	//CurrentPhase = EGamePhase::Lobby;
	// 	Team10GameState->SetCurrentPhase(EGamePhase::None);
	// }
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
		FoundPlayerSpawner();
		SpawnAllPlayer(NewPlayer);
		GameFlowManager->StartGame();
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

void ATeam10GameMode::FoundPlayerSpawner()
{
	for (TActorIterator<APlayerSpawn> It(GetWorld()); It; ++It)
	{
		if(It->AreaTag == Area_Area1)
		{
			PlayerSpawners.Add(*It);
		}
		
	}
}

void ATeam10GameMode::SpawnAllPlayer(APlayerController* NewPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerSpawners Count Area1: %d"), PlayerSpawners.Num());
}

void ATeam10GameMode::ReSpawnPlayer(APlayerController* NewPlayer)
{
	//RestartPlayerAtPlayerStart();
}



void ATeam10GameMode::PlayerVote()
{
	if (!Team10GameState)
	{
		return;
	}

	int32 AreaVoteCount = 0;

	switch (Team10GameState->CurrentArea)
	{
	case EGameArea::Area1:
		AreaVoteCount = Area1VoteCount;
		break;
	case EGameArea::Area2:
		AreaVoteCount = Area2VoteCount;
		break;
	case EGameArea::Area3:
		AreaVoteCount = Area3VoteCount;
		break;
	default:
		break;
	}


	// 생존자 과반수, 구역별 투표수 둘 중 적은 쪽을 플레이어 사망에 필요한 최소 투표수로 설정한다.
	int32 AlivePlayerCount = (Team10GameState->AliveInfectedCount + Team10GameState->AliveCitizenCount) / 2 + 1;
	
	Team10GameState->KillPlayerVotesCount = AlivePlayerCount > AreaVoteCount ? AreaVoteCount : AlivePlayerCount;
}

int32 ATeam10GameMode::GetAliveCitizenCount()
{
	return Team10GameState->AliveCitizenCount;
}

int32 ATeam10GameMode::GetAliveInfectedCount()
{
	return Team10GameState->AliveInfectedCount;
}

// 플레이어 스폰 설정
