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
	
	Team10GameState = Cast<ATeam10GameState>(GameState);
	
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
			 	
			 	// if (CivilianPlayerState->IsPlayerRole(GamePlayTags::PlayerRole::Civilian) && CivilianPlayerState->IsAlive)
			 	// {
			 	// 	Team10GameState->AliveCitizenCount--;
			 	// }
			 	// else if (CivilianPlayerState->IsPlayerRole(GamePlayTags::PlayerRole::Infected) && CivilianPlayerState->IsAlive)
			 	// {
			 	// 	Team10GameState->AliveInfectedCount--;
			 	// }
			 	// CheckWinCondition();
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

	if (!Team10GameState)
	{
		return;
	}

	ACivilianPlayerController* CivilianPlayerController = Cast<ACivilianPlayerController>(NewPlayer);

	if (CivilianPlayerController)
	{
		Team10GameState->AllPlayers.Add(CivilianPlayerController);
	}
	
	LoadedPlayerCount++;
	const int32 ExpectedPlayers = 2; // 크래시 나길래 추가한 크래시 방지용 임시 추가코드. - 금성
	if (LoadedPlayerCount >= ExpectedPlayers)	// 크래시 나길래 추가한 크래시 방지용 임시 추가 코드. - 금성
	//if (LoadedPlayerCount == Team10GameState->PlayerArray.Num())
	{
		// 모든 플레이어의 로딩이 끝나면 로딩 UI를 해제한다.
		UE_LOG(LogTemp, Log, TEXT("All Player Loaded"));
		
		AssignInfectedPlayers();
		PlayerSpawnManager->FoundPlayerSpawner(GamePlayTags::AreaTag::Area_Area1);
		PlayerSpawnManager->SpawnAllPlayer();
		GameFlowManager->StartGame();


		// 사망 시 관전 및 리스폰 테스트
		// FTimerHandle TimerHandle;
		//
		// GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, CivilianPlayerController]()
		// {
		// 	HandlePlayerDeath(CivilianPlayerController);
		// }), 10.f, false);
	
	}
}
void ATeam10GameMode::AssignInfectedPlayers()
{
	if (!Team10GameState)
	{
		return;
	}

	int32 PlayerCount = Team10GameState->PlayerArray.Num();
	int32 ActualInfectedCount = FMath::Clamp(InfectedCount, 0, PlayerCount);	// 크래시 나길래 추가한 크래시 방지용 임시 추가 코드. - 금성	

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
	
	for (int i = 0; i < ActualInfectedCount; i++)	// 크래시 나길래 추가한 크래시 방지용 임시 추가 코드. - 금성
		//for (int i = 0; i < InfectedCount; i++)
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


void ATeam10GameMode::ReSpawnPlayer(APlayerController* Controller)
{
	// 사망한 플레이어 랜덤 스폰 지점으로 리스폰
	if (PlayerSpawnManager)
	{
		RestartPlayerAtPlayerStart(Controller, PlayerSpawnManager->GetPlayerSpawner());
	}

	//player state idle로 변경
}

void ATeam10GameMode::HandlePlayerDeath(APlayerController* DeadPlayer, APlayerController* AttackPlayer)
{
	// 플레이어가 기본 상태에서 기본 공격(총)에 의해 사망 할 경우 투표 상태로 변경
	// 투표 상태에서 일정 시간이 지나면 리스폰
	// 플레이어가 감염자 변신 상태에서 사망할 경우 리스폰 (변신이 풀리는지는 모름)
	// 플레이어가 감염자 변신 공격에 사망할 경우 영구 사망
	// 투표 상태에서 투표 수 초과 할 경우 영구 사망
	
	if (!DeadPlayer)
	{
		return;
	}

	if (!AttackPlayer)
	{
		return;
	}
	
	if(!Team10GameState)
	{
		return;
	}

	ACivilianPlayerState* AttackPlayerState = AttackPlayer->GetPlayerState<ACivilianPlayerState>();
	ACivilianPlayerState* DeadPlayerState = DeadPlayer->GetPlayerState<ACivilianPlayerState>();
	
	if (!AttackPlayerState)
	{
		return;
	}

	if (!DeadPlayerState)
	{
		return;
	}
	
	// // 감염자가 변신 상태에서 사망 시 리스폰 
	// if (DeadPlayerState->IsPlayerRole(GamePlayTags::PlayerRole::Infected))
	// {
	// 	if (DeadPlayerState->IsInfectedState(GamePlayTags::InfectedState::Transformed))
	// 	{
	// 		RespawnDeath(DeadPlayer);
	// 		return;
	// 	}
	// }
	// else
	// {
	// 	//시민이 감연자 변신에 사망할 경우 영구 사망
	// 	if (AttackPlayerState->IsPlayerRole(GamePlayTags::PlayerRole::Infected) && DeadPlayerState->IsInfectedState(GamePlayTags::InfectedState::Transformed))
	// 	{
	// 		EternalDeath(DeadPlayer);
	// 		return;
	// 	}
	// }
	//
	// // 위의 두가지 경우를 제외하고 모두 투표 상태로 전환
	// StartVote(DeadPlayerState, AttackPlayerState);
	//
	// UE_LOG(LogTemp, Error, TEXT("Player Dead"));
	
}

void ATeam10GameMode::RespawnDeath(APlayerController* DeadPlayer)
{
	APawn* Pawn = DeadPlayer->GetPawn();

	if (Pawn)
	{
		//UE_LOG(LogTemp, Error, TEXT("Pawn Valid"));
		DeadPlayer->UnPossess();
		Pawn->Destroy();
	}

	ReSpawnPlayer(DeadPlayer);
}

void ATeam10GameMode::EternalDeath(APlayerController* DeadPlayer)
{
	ACivilianPlayerState* CivilianPlayerState = DeadPlayer->GetPlayerState<ACivilianPlayerState>();

	if (!Team10GameState)
	{
		return;
	}
	
	// if (CivilianPlayerState)
	// {
	// 	if (CivilianPlayerState->IsPlayerRole(GamePlayTags::PlayerRole::Civilian))
	// 	{
	// 		CivilianPlayerState->SetCivilianState(GamePlayTags::CivilianState::Dead);
	// 		Team10GameState->AliveCitizenCount--;
	// 		
	// 	}
	// 	else if(CivilianPlayerState->IsPlayerRole(GamePlayTags::PlayerRole::Infected))
	// 	{
	// 		CivilianPlayerState->SetInfectedState(GamePlayTags::InfectedState::Dead);
	// 		Team10GameState->AliveInfectedCount--;
	// 	}
	//
	// 	CheckWinCondition();
	// }

	APawn* Pawn = DeadPlayer->GetPawn();

	if (Pawn)
	{
		//UE_LOG(LogTemp, Error, TEXT("Pawn Valid"));
		DeadPlayer->UnPossess();
		Pawn->Destroy();
	}
	
	for (ACivilianPlayerController* CivilianPlayerController : Team10GameState->AllPlayers)
	{
		APawn* TargetPawn = CivilianPlayerController->GetPawn();
		if (TargetPawn)
		{
			// 클라에서 실행
			DeadPlayer->SetViewTargetWithBlend(TargetPawn);
			return;
		}
	}
}


void ATeam10GameMode::StartVote(ACivilianPlayerState* VoteTarget, ACivilianPlayerState* VotePlayer)
{
	if (!VoteTarget)
	{
		return;
	}

	if (!VotePlayer)
	{
		return;
	}
	
	// if (VoteTarget->IsPlayerRole(GamePlayTags::PlayerRole::Infected))
	// {
	// 	VoteTarget->SetCivilianStateTag(GamePlayTags::InfectedState::Stun);
	// }
	// else
	// {
	// 	VoteTarget->SetInfectedStateTag(GamePlayTags::CivilianState::Stun);
	// }

	// 투표한 플레이어 저장해서 중복 투표 불가능하게 설정
	//VoteTarget->
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

bool ATeam10GameMode::CanInfectedTransform(APlayerState* PlayerState)
{
	if (!Team10GameState)
	{
		return false;
	}
	
	if (!PlayerState)
	{
		return false;
	}

	ACivilianPlayerState* CivilianPlayerState = Cast<ACivilianPlayerState>(PlayerState);

	if (!CivilianPlayerState)
	{
		return false;
	}
	
	if (Team10GameState->CurrentPhase != EGamePhase::NightPhase && Team10GameState->CurrentPhase != EGamePhase::TrapIn)
	{
		return false;
	}

	if (CivilianPlayerState->IsPlayerRole(GamePlayTags::PlayerRole::Civilian))
	{
		return false;
	}

	// 감염자가 이미 변신 중이거나 변신에 필요한 혈액량이 부족할 경우 return false
		
	return true;
	
}
// 그 외에 밤 페이즈가 되면 화면 어둡게 설정 델리게이트로 nightphase가 되면 맵에 있는 lightobject의 light를 비활성화 하는 방식 생각 중
// 밤 페이즈가 끝나면 되면 변신이 풀려야 함

int32 ATeam10GameMode::GetAliveCitizenCount()
{
	return Team10GameState->AliveCitizenCount;
}

int32 ATeam10GameMode::GetAliveInfectedCount()
{
	return Team10GameState->AliveInfectedCount;
}

void ATeam10GameMode::ProcessChatMessage(APlayerController* InPlayerController, const FChatMessage& ChatMessage)
{
	if (GameState->PlayerArray.Num() <= 0) return;
	
	TArray<TObjectPtr<APlayerState>> PlayerList = GameState->PlayerArray;
	FChatMessage RefinedChatMessage = ChatMessage;
	APlayerState* InPlayerState = InPlayerController->PlayerState;
	if (!IsValid(InPlayerState)) return;

	int32 SenderIndex = GameState->PlayerArray.Find(InPlayerState);
	if (SenderIndex == -1) return;

	RefinedChatMessage.PlayerName = GameState->PlayerArray[SenderIndex]->GetPlayerName();
	
	TArray<TObjectPtr<APlayerController>> ValidPlayerList;
	for (TObjectPtr<APlayerState> ClientPlayer : PlayerList)	// 필터. 팀채팅, 일반채팅을 필터하기 위함
	{
		if (!IsValid(ClientPlayer)) continue;	// 받을사람이 도중에 게임을 나간 경우 ?

		APlayerController* ClientController =  ClientPlayer->GetPlayerController();
		if (!IsValid(ClientController)) continue;

		ValidPlayerList.Push(ClientController);
	}

	if (ValidPlayerList.Num() <= 0) return;	// 채팅을 수신할 대상이 없는 경우.

	for (TObjectPtr<APlayerController> ValidPlayer : ValidPlayerList)
	{
		if (!IsValid(ValidPlayer)) continue;

		ACivilianPlayerController* CastPlayerController = Cast<ACivilianPlayerController>(ValidPlayer);
		if (!IsValid(CastPlayerController)) continue;
		CastPlayerController->ClientRPC_ReceiveMessage(RefinedChatMessage);
	}
}
