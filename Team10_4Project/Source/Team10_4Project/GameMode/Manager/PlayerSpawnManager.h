// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameMode/GameTypes/GameTypes.h"
#include "PlayerSpawnManager.generated.h"


class ATeam10GameState;
class APlayerSpawn;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM10_4PROJECT_API UPlayerSpawnManager : public UActorComponent
{
	GENERATED_BODY()
public:
	UPlayerSpawnManager();

	virtual void BeginPlay() override;
	
	void FoundPlayerSpawner(EGameArea GameArea);
	void SpawnAllPlayer();
	void ReSpawnPlayer(APlayerController* NewPlayer);

protected:
	UPROPERTY()
	TObjectPtr<ATeam10GameState> Team10GameState;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<APlayerSpawn>> PlayerSpawners;
};
