
// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Manager/GameFlowManager.h"
//#include "GameTypes/GameTypes.h"
#include "Team10GameMode.generated.h"


class ACivilianPlayerState;
class ACivilian;
class ACivilianPlayerController;
struct FGameplayTag;
class APlayerSpawn;
class UGameFlowManager;
class UPlayerSpawnManager;
class ATeam10GameState;

UCLASS()
class TEAM10_4PROJECT_API ATeam10GameMode : public AGameModeBase
{
	GENERATED_BODY()
#pragma region GameFlow 
	
public:
	ATeam10GameMode();
	
	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaSeconds) override;
	//virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	//virtual void HandleSeamlessTravelPlayer(AController*& C) override; // 맵 로딩 중에 호출 
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override; // 맵 진입 완료 후 호출
	
	// 승리 조건 체크
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void CheckWinCondition();
	
	// 플레이어 사망 처리
	UFUNCTION(BlueprintCallable, Category = "Dead")
	void HandlePlayerDeath(APlayerController* DeadPlayer, APlayerController* AttackPlayer);

	// 부활이 가능한 사망
	UFUNCTION(BlueprintCallable, Category = "Dead")
	void RespawnDeath(APlayerController* DeadPlayer);
	
	// 부활이 불가능한 사망
	UFUNCTION(BlueprintCallable, Category = "Dead")
	void EternalDeath(APlayerController* DeadPlayer);
	
	// 감염자 결정
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void AssignInfectedPlayers();

	// 퓨즈 활성화 하면 호출
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void OnFuseBoxActivated();
	
	// 감염자가 변신이 가능한 상태인지 체크
	bool CanInfectedTransform(APlayerState* PlayerState);
protected:
	
	// 감염자 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	int32 InfectedCount = 2;
	
	// 다음 Area로 이동하기 위해 필요한 퓨즈박스 개수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	int32 TotalFuseBoxCount = 4;
	
	// 현재 맵 로딩을 완료한 플레이어 수
	// 지금은 GameMode에 선언 했지만 UI가 필요할 경우 GameState에서 Replicate 예정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameFlow")
	int32 LoadedPlayerCount  = 0;

	// 감염자가 변신에 필요한 최소 혈액량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	int32 MinBloodForTransformation = 100; // 임시로 설정
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameFlow")
	bool CanRespawn;

	// GameState 레퍼런스
	UPROPERTY()
	TObjectPtr<ATeam10GameState> Team10GameState;

private:
	// 생존자 수 카운트
	int32 GetAliveCitizenCount();
	int32 GetAliveInfectedCount();

	void ReSpawnPlayer(APlayerController* Controller);

#pragma endregion


#pragma region Vote


public:
	// 플레이어가 투표 상태가 되면 호출
	UFUNCTION(BlueprintCallable, Category = "Vote")
	void UpdateKillPlayerVotesCount();

	UFUNCTION(BlueprintCallable, Category = "Vote")
	void StartVote(ACivilianPlayerState* VoteTarget, ACivilianPlayerState* VotePlayer);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vote")
	int32 Area1VoteCount = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vote")
	int32 Area2VoteCount = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vote")
	int32 Area3VoteCount = 2;
	
#pragma endregion

#pragma region Chat

public:
	void ProcessChatMessage(APlayerController* InPlayerController, const struct FChatMessage& ChatMessage);
	
#pragma endregion

	
#pragma region Manager

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGameFlowManager> GameFlowManager;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerSpawnManager> PlayerSpawnManager;
	
#pragma endregion
};