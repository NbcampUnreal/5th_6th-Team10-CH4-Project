// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameMode/Team10GameMode.h"
#include "Team10GameState.generated.h"

class APlayerController;

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
	
	// 현재 게임 페이즈
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase, BlueprintReadOnly, Category = "GameState")
	EGamePhase CurrentPhase;

	UFUNCTION()
	void OnRep_CurrentPhase();

	// 현재 페이즈 남은 시간
	UPROPERTY(ReplicatedUsing = OnRep_PhaseTimeRemaining, BlueprintReadOnly, Category = "GameState")
	int32 PhaseTimeRemaining;

	UFUNCTION()
	void OnRep_PhaseTimeRemaining();
	
	// 게임 결과
	UPROPERTY(ReplicatedUsing = OnRep_GameResult, BlueprintReadOnly, Category = "GameState")
	EGameResult GameResult;

	UFUNCTION()
	void OnRep_GameResult();
	
	// 모든 플레이어
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
	TArray<TObjectPtr<APlayerController>> AllPlayers; // 현재는 타입이 기본 APlayerController 이지만 추후 APlayerController의 자식 클래스를 생성하면 자식 클래스로 타입을 변경할 예정
	
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
