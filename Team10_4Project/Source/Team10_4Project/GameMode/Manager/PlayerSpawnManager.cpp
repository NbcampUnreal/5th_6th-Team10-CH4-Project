// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/Manager/PlayerSpawnManager.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/PlayerSpawn.h"
#include "GameState/Team10GameState.h"

// Sets default values for this component's properties
UPlayerSpawnManager::UPlayerSpawnManager()
{
	
	PrimaryComponentTick.bCanEverTick = false;

}

void UPlayerSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	Team10GameState = GetWorld()->GetGameState<ATeam10GameState>();
}

void UPlayerSpawnManager::FoundPlayerSpawner(EGameArea GameArea)
{
	if (!Team10GameState)
	{
		return;
	}
	
	for (TActorIterator<APlayerSpawn> It(GetWorld()); It; ++It)
	{
		if(It->GameArea == Team10GameState->CurrentArea)
		{
			PlayerSpawners.Add(*It);
		}
	}
}

void UPlayerSpawnManager::SpawnAllPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerSpawners Count Area1: %d"), PlayerSpawners.Num());

	for (int32 i = PlayerSpawners.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		PlayerSpawners.Swap(i, j);
	}
	
	for (int i = 0; i < Team10GameState->PlayerArray.Num() && i < PlayerSpawners.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("X:%f , Y:%f Z: %f"),PlayerSpawners[i]->GetActorLocation().X, PlayerSpawners[i]->GetActorLocation().Y, PlayerSpawners[i]->GetActorLocation().Z);
		// 폰 캐릭터의 위치 회전 값 playerspawner의 위치 회전 값으로 설정

		APawn* Pawn = Team10GameState->PlayerArray[i]->GetPawn();

		Pawn->SetActorLocationAndRotation(PlayerSpawners[i]->GetActorLocation(), PlayerSpawners[i]->GetActorRotation());
	}
	
}

void UPlayerSpawnManager::ReSpawnPlayer(APlayerController* NewPlayer)
{
	//RestartPlayerAtPlayerStart();
}