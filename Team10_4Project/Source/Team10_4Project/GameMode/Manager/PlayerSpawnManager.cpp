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

void UPlayerSpawnManager::FoundPlayerSpawner(FGameplayTag AreaTag)
{
	if (!Team10GameState)
	{
		return;
	}
	
	for (TActorIterator<APlayerSpawn> It(GetWorld()); It; ++It)
	{
		if(It->AreaTag.MatchesTagExact(AreaTag))
		{
			PlayerSpawners.Add(*It);
		}
	}

	for (int32 i = PlayerSpawners.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		PlayerSpawners.Swap(i, j);
	}
}

void UPlayerSpawnManager::SpawnAllPlayer()
{
	for (int i = 0; i < Team10GameState->PlayerArray.Num() && i < PlayerSpawners.Num(); i++)
	{
		APawn* Pawn = Team10GameState->PlayerArray[i]->GetPawn();

		Pawn->SetActorLocationAndRotation(PlayerSpawners[i]->GetActorLocation(), PlayerSpawners[i]->GetActorRotation());
	}
	
}

APlayerSpawn* UPlayerSpawnManager::GetPlayerSpawner()
{
	if (BeforeSpawnIndex >= PlayerSpawners.Num())
	{
		BeforeSpawnIndex = 0;
	}

	return PlayerSpawners[BeforeSpawnIndex++];
}



