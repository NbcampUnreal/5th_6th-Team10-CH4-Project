
// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Team10GameMode.generated.h"

class APlayerSpawn;
class UGameFlowManager;
class ATeam10GameState;

UCLASS()
class TEAM10_4PROJECT_API ATeam10GameMode : public AGameModeBase
{
	GENERATED_BODY()
#pragma region GameFlow 
	
public:
	ATeam10GameMode();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	//virtual void HandleSeamlessTravelPlayer(AController*& C) override; // 맵 로딩 중에 호출 
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override; // 맵 진입 완료 후 호출
	
	// 승리 조건 체크
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void CheckWinCondition();
	
	// 플레이어 사망 처리
	UFUNCTION(BlueprintCallable, Category = "Player")
	void HandlePlayerDeath(APlayerController* DeadPlayer);
	
	// 감염자 결정
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void AssignInfectedPlayers();

protected:
	
	// 감염자 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	int32 InfectedCount = 2;
	
	// 다음 Area로 이동하기 위해 필요한 퓨즈박스 개수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	int32 TotalFixFuseBox = 4;
	
	// 현재 맵 로딩을 완료한 플레이어 수
	// 지금은 GameMode에 선언 했지만 UI가 필요할 경우 GameState에서 Replicate 예정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameFlow")
	int32 LoadedPlayerCount  = 0; 

	// GameState 레퍼런스
	UPROPERTY()
	TObjectPtr<ATeam10GameState> Team10GameState;

private:
	
	// 다음 구역 활성화
	void OpenNextArea();

	// 생존자 수 카운트
	int32 GetAliveCitizenCount();
	int32 GetAliveInfectedCount();

#pragma endregion

#pragma region Spawn
	
public:
	// 플레이어 스폰
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnAllPlayer(APlayerController* NewPlayer);

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void ReSpawnPlayer(APlayerController* NewPlayer);

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void FoundPlayerSpawner();
protected:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<APlayerSpawn>> PlayerSpawners;
	
#pragma endregion
#pragma region Vote


public:
	// 플레이어가 투표 상태가 되면 호출
	UFUNCTION(BlueprintCallable, Category = "Vote")
	void PlayerVote();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vote")
	int32 Area1VoteCount = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vote")
	int32 Area2VoteCount = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vote")
	int32 Area3VoteCount = 2;
	
#pragma endregion


#pragma region Manager

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGameFlowManager> GameFlowManager;
	
#pragma endregion
};