// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/Team10GameMode.h"

#include "PlayerSpawn.h"
#include "Character/CivilianPlayerController.h"
#include "Character/CivilianPlayerState.h"
#include "GameState/Team10GameState.h"
#include "Manager/GameFlowManager.h"
#include "GamePlayTag/GamePlayTags.h"
#include "InGameUI/JKH/ChatSubsystem.h"
#include "Manager/PlayerSpawnManager.h"

ATeam10GameMode::ATeam10GameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GameStateClass = ATeam10GameState::StaticClass();
	PlayerControllerClass = ACivilianPlayerController::StaticClass();
	PlayerStateClass = ACivilianPlayerState::StaticClass();
	
	GameFlowManager = CreateDefaultSubobject<UGameFlowManager>(TEXT("GameFlowManager"));
	PlayerSpawnManager = CreateDefaultSubobject<UPlayerSpawnManager>(TEXT("PlayerSpawnManager"));
}

void ATeam10GameMode::BeginPlay()
{
	Super::BeginPlay();

	GameFlowManager->OnCurrentAreaChangedDelegate.AddUObject(this, &ThisClass::OnCurrentAreaChanged);
	
	Team10GameState = Cast<ATeam10GameState>(GameState);
	
	if (Team10GameState)
	{
		for (int i = 0; i < Team10GameState->PlayerArray.Num(); i++)
		{
			ACivilianPlayerController* CivilianPlayerController = Cast<ACivilianPlayerController>(Team10GameState->PlayerArray[i]->GetPlayerController());
			Team10GameState->AllPlayers.Add(CivilianPlayerController);
		}
	}
	
	Team10GameState->CurrentArea = EGameArea::Area1;
	
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

	if (!GameFlowManager || !PlayerSpawnManager)
	{
		return;
	}
	
	LoadedPlayerCount++;
	if (LoadedPlayerCount == Team10GameState->PlayerArray.Num())
	{
		// 모든 플레이어의 로딩이 끝나면 로딩 UI를 해제한다.
		UE_LOG(LogTemp, Log, TEXT("All Player Loaded"));
		
		InitializeRemainingFuseBoxes();
		AssignInfectedPlayers();
		PlayerSpawnManager->FoundPlayerSpawner(EGameArea::Area1);
		PlayerSpawnManager->SpawnAllPlayer();
		GameFlowManager->StartGame();
	}
}

void ATeam10GameMode::ReSpawnPlayer(APlayerController* Controller)
{
	// 사망한 플레이어 랜덤 스폰 지점으로 리스폰
	if (PlayerSpawnManager)
	{
		RestartPlayerAtPlayerStart(Controller, PlayerSpawnManager->GetPlayerSpawner());
	}
}

void ATeam10GameMode::CheckWinCondition()
{
	if (!Team10GameState)
	{
		return;
	}

	if (!GameFlowManager)
	{
		return;
	}
	
	if (GetAliveCitizenCount()== 0)
	{
		GameFlowManager->EndGame(EGameResult::InfectedWin);
	}
	else if (GetAliveInfectedCount() == 0)
	{
		GameFlowManager->EndGame(EGameResult::CitizenWin);
	}
}

void ATeam10GameMode::HandlePlayerDeath(APlayerController* DeadPlayer)
{
	if (!DeadPlayer)
	{
		return;
	}

	if(!Team10GameState)
	{
		return;
	}
	
	APawn* Pawn = DeadPlayer->GetPawn();

	if (Pawn)
	{
		Pawn->Destroy();
	}

	// // 플레이어가 다시 스폰이 가능한 사망인지 영구적 사망인지 체크
	//
	// // 영구 사망 DeadPlayer의 역할에 맞춰 AliveCount감소
	//
	// ACivilianPlayerState* CivilianPlayerState = Pawn->GetController()->GetPlayerState<ACivilianPlayerState>();
	//
	// if (CivilianPlayerState)
	// {
	// 	if (CivilianPlayerState->GetPlayerRole() == EPlayerRole::Civilian)
	// 	{
	// 		Team10GameState->AliveCitizenCount--;
	// 	}
	// 	else if(CivilianPlayerState->GetPlayerRole() == EPlayerRole::Infected)
	// 	{
	// 		Team10GameState->AliveInfectedCount--;
	// 	}
	//
	// 	CheckWinCondition();
	// }
	//
	// // 사망한 플레이어 관전상태로 변경
	//
	// for (ACivilianPlayerController* CivilianPlayerController : Team10GameState->AllPlayers)
	// {
	// 	APawn* TargetPawn = CivilianPlayerController->GetPawn();
	// 	if (TargetPawn)
	// 	{
	// 		DeadPlayer->SetViewTargetWithBlend(TargetPawn);
	// 		return;
	// 	}
	// 	
	// }
	//
	// // 리스폰 가능한 사망
	//
	// ReSpawnPlayer(DeadPlayer);
	//
}

void ATeam10GameMode::AssignInfectedPlayers()
{
	if (!Team10GameState)
	{
		return;
	}
	
	int32 PlayerCount = Team10GameState->PlayerArray.Num();
	
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
	
	for (int i = 0; i < InfectedCount; i++)
	{
		ACivilianPlayerState* CivilianPlayerState = Cast<ACivilianPlayerState>(Team10GameState->PlayerArray[RandomInfectedArray[i]]);
		CivilianPlayerState->SetPlayerRoleTag(GamePlayTags::PlayerRole::Infected);
		UE_LOG(LogTemp, Warning, TEXT("You're Infected"));
	}
	
	for (int i = InfectedCount; i < PlayerCount; i++)
	{
		ACivilianPlayerState* CivilianPlayerState = Cast<ACivilianPlayerState>(Team10GameState->PlayerArray[RandomInfectedArray[i]]);
		CivilianPlayerState->SetPlayerRoleTag(GamePlayTags::PlayerRole::Civilian);
	}

	Team10GameState->AliveInfectedCount = InfectedCount;
	Team10GameState->AliveCitizenCount = PlayerCount - InfectedCount;
}


void ATeam10GameMode::OnCurrentAreaChanged(EGameArea GameArea)
{
	if (PlayerSpawnManager)
	{
		PlayerSpawnManager->FoundPlayerSpawner(GameArea);
	}
}

void ATeam10GameMode::UpdateKillPlayerVotesCount()
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
	
	Team10GameState->KillPlayerVotesCount = AlivePlayerCount >= AreaVoteCount ? AreaVoteCount : AlivePlayerCount;
}

void ATeam10GameMode::OnFuseBoxActivated()
{
	if (Team10GameState)
	{
		Team10GameState->RemainingFuseBoxCount--;

		if (Team10GameState->RemainingFuseBoxCount <= 0)
		{
			GameFlowManager->OpenNextArea();
		}
	}
	
}

void ATeam10GameMode::InitializeRemainingFuseBoxes()
{
	if (Team10GameState)
	{
		Team10GameState->RemainingFuseBoxCount = TotalFuseBoxCount;
	}
}



int32 ATeam10GameMode::GetAliveCitizenCount()
{
	return Team10GameState->AliveCitizenCount;
}

int32 ATeam10GameMode::GetAliveInfectedCount()
{
	return Team10GameState->AliveInfectedCount;
}




// void ATeam10GameMode::ProcessChatMessage(APlayerController* InPlayerController, const FChatMessage& ChatMessage)
// {
// 	//TArray<TObjectPtr<APlayerController>> PlayerList;
// 	TArray<TWeakObjectPtr<APlayerController>> ValidPlayerList;
// 	FChatMessage RefinedChatMessage = ChatMessage;
//
// 	APlayerState* InPlayerState = InPlayerController->PlayerState;
// 	if (IsValid(InPlayerState) == false) return;
// 	FString SenderName = InPlayerState->GetPlayerName();
//
// 	for (APlayerController* ClientPlayer : PlayerList)
// 	{
// 		if (IsValid(ClientPlayer) == false) continue;
// 		if (ClientPlayer == InPlayerController)
// 		{
// 			if (RefinedChatMessage.PlayerName != SenderName)	// hacking ?
// 			{
// 				RefinedChatMessage.PlayerName = SenderName;
// 			}
// 			continue;
// 		}
// 		ValidPlayerList.Push(ClientPlayer);
// 	}
// 	
// 	if (ValidPlayerList.Num() <= 0) return;
//
// 	for (APlayerController* ValidPlayer : ValidPlayerList)
// 	{
// 		ValidPlayer->ClientRPC_ReceiveMessage(RefinedChatMessage);
// 	}
// }