// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/Team10LobbyGameMode.h"

#include "Character/CivilianPlayerController.h"
#include "Character/CivilianPlayerState.h"
#include "GameInstance/Team10GameInstance.h"
#include "GameState/Team10GameState.h"
#include "GameTypes/GameTypes.h"
#include "Kismet/GameplayStatics.h"


ATeam10LobbyGameMode::ATeam10LobbyGameMode()
{
	bUseSeamlessTravel = true;
}

void ATeam10LobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	Team10GameState = Cast<ATeam10GameState>(GameState);
	if (Team10GameState)
	{
		//CurrentPhase = EGamePhase::Lobby;
		Team10GameState->SetCurrentPhase(EGamePhase::Lobby);
	}
}

void ATeam10LobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Log, TEXT("Player Logged In Lobby: %s"), *NewPlayer->GetName());
	ACivilianPlayerController* CivilianPlayerController = Cast<ACivilianPlayerController>(NewPlayer);
	//UTeam10GameInstance* Team10GameInstance = Cast<UTeam10GameInstance>(GetWorld()->GetGameInstance());
	ACivilianPlayerState* CivilianPlayerState = CivilianPlayerController->GetPlayerState<ACivilianPlayerState>();
	
	if (!Team10GameState)
	{
		UE_LOG(LogTemp, Log, TEXT("Team10GameState is null"));
		return;
	}
	if (!CivilianPlayerController)
	{
		UE_LOG(LogTemp, Log, TEXT("CivilianPlayerController is null"));
		return;
	}
	// if (!Team10GameInstance)
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("Team10GameInstance is null"));
	// 	return;
	// }
	if (!CivilianPlayerState)
	{
		UE_LOG(LogTemp, Log, TEXT("CivilianPlayerState is null"));
		return;
	}
	
	// 닉네임 설정
	//CivilianPlayerState->NickName = Team10GameInstance->NickName;
	
	Team10GameState->AllPlayers.Add(CivilianPlayerController);
	
	if (Team10GameState->CurrentPhase == EGamePhase::Lobby)
	{
		if (Team10GameState->AllPlayers.Num() >= MinPlayersToStart)
		{
			// 멀티 서버에서 레벨을 이동할 때는 openlevel이 아닌 Travelling을 사용
			// Travelling은 Seamless와 non-seamless로 나뉜다.
			// 이 둘의 가장 큰 차이점은 non-seamless는 클라이언트가 서버의 연결을 끊은 뒤 동일 서버에 재연결하는 과정을 거친다.
			GetWorld()->ServerTravel(TEXT("/Game/Team10/Level/InGame"));

			UE_LOG(LogTemp, Log, TEXT("ServerTravel"));
			// 모든 플레이어에게 로딩 UI 출력
				
			// 추후 모든 플레이어가 준비되면 게임을 시작하게 변경할 수 있음.
		}
	}
}

void ATeam10LobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (!Team10GameState)
	{
		return;
	}
	
	ACivilianPlayerController* CivilianPlayerController = Cast<ACivilianPlayerController>(Exiting);
	if (CivilianPlayerController)
	{
		if (Team10GameState->CurrentPhase == EGamePhase::Lobby)
		{
			Team10GameState->AllPlayers.Remove(CivilianPlayerController);
		}
	}
}
