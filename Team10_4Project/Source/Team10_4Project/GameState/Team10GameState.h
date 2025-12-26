// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameMode/Team10GameMode.h"
#include "GameMode/GameTypes/GameTypes.h"
#include "Team10GameState.generated.h"

class ACivilianPlayerController;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseTimeChanged, int32, ChangedTime);

UCLASS()
class TEAM10_4PROJECT_API ATeam10GameState : public AGameStateBase
{
	GENERATED_BODY()
	
#pragma region GameState 
public:
	ATeam10GameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 현재 게임 구역
	UPROPERTY(ReplicatedUsing = OnRep_CurrentArea, BlueprintReadOnly, Category = "GameState")
	EGameArea CurrentArea;
	
	UFUNCTION()
	void OnRep_CurrentArea();

	// 플레이어를 투표로 죽이는데 필요한 최소 투표 수    
	UPROPERTY(ReplicatedUsing = OnRep_KillPlayerVotesCount, BlueprintReadOnly, Category = "GameState")
	int32 KillPlayerVotesCount;

	UFUNCTION()
	void OnRep_KillPlayerVotesCount();

	// 현재 투표 수    
	UPROPERTY(ReplicatedUsing = OnRep_CurrentVotesCount, BlueprintReadOnly, Category = "GameState")
	int32 CurrentVotesCount;

	UFUNCTION()
	void OnRep_CurrentVotesCount();
	
	// 현재 게임 페이즈
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase, BlueprintReadOnly, Category = "GameState")
	EGamePhase CurrentPhase;

	UFUNCTION()
	void OnRep_CurrentPhase();

	// 현재 페이즈 남은 시간
	UPROPERTY(ReplicatedUsing = OnRep_PhaseTimeRemaining, BlueprintReadOnly, Category = "GameState")
	int32 PhaseTimeRemaining;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPhaseTimeChanged OnPhaseTimeChanged;

	UFUNCTION()
	void OnRep_PhaseTimeRemaining();

	// 다음 구역으로 이동하기 위해 활성화가 필요한 퓨즈 박스 개수
	UPROPERTY(ReplicatedUsing = OnRep_RemainingFuseBoxCount, BlueprintReadOnly, Category = "GameState")
	int32 RemainingFuseBoxCount;

	UFUNCTION()
	void OnRep_RemainingFuseBoxCount();
	
	// 게임 결과
	UPROPERTY(ReplicatedUsing = OnRep_GameResult, BlueprintReadOnly, Category = "GameState")
	EGameResult GameResult;

	UFUNCTION()
	void OnRep_GameResult();
	
	// 모든 플레이어
	UPROPERTY(BlueprintReadOnly, Category = "GameState")
	TArray<TObjectPtr<ACivilianPlayerController>> AllPlayers; 
	
	// 생존 플레이어 수
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
	int32 AliveCitizenCount;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
	int32 AliveInfectedCount;

	
	// Setter 함수들
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void SetCurrentPhase(EGamePhase NewPhase);

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void SetPhaseTimeRemaining(float Time);

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void SetGameResult(EGameResult Result);

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void DecreaseRemainTimer();

#pragma endregion	
};
