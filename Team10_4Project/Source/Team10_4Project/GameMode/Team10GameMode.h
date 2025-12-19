
// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Team10GameMode.generated.h"

#pragma region ENUM

UENUM(BlueprintType)
enum class EGameArea : uint8
{
	None,
	Area1 UMETA(DisplayName = "Area1"),
	Area2 UMETA(DisplayName = "Area2"),
	Area3 UMETA(DisplayName = "Area3"),
	
};

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	None,
	Lobby UMETA(DisplayName = "Lobby"),
	//AssignInfectedPhase UMETA(DisplayName = "TeamSelection"),
	DayPhase UMETA(DisplayName = "DayPhase"),
	NightPhase UMETA(DisplayName = "NightPhase"),
	TrapIn UMETA(DisplayName = "TrapIn"),
	GameEnd UMETA(DisplayName = "GameEnd"),
};
UENUM(BlueprintType)
enum class EGameResult : uint8
{	
	None,
	CitizenWin UMETA(DisplayName = "CitizenWin"),
	InfectedWin  UMETA(DisplayName = "InfectedWin"),
};

#pragma endregion

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

	// 게임 시작
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void StartGame();
	
	// 페이즈 전환
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void ChangePhase(EGamePhase NewPhase);
	
	// 승리 조건 체크
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void CheckWinCondition();
	
	// 게임 종료
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void EndGame(EGameResult Result);
	
	// 플레이어 사망 처리
	UFUNCTION(BlueprintCallable, Category = "Player")
	void HandlePlayerDeath(APlayerController* DeadPlayer);
	
	// 감염자 결정
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void AssignInfectedPlayers();
	
protected:

	// 현재 구역
	UPROPERTY(BlueprintReadOnly, Category = "GameFlow")
	EGameArea CurrentArea;
	
	// 현재 페이즈
	UPROPERTY(BlueprintReadOnly, Category = "GameFlow")
	EGamePhase CurrentPhase;

	// 최소 플레이어 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	int32 MinPlayersToStart = 6;
	
	// 감염자 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	int32 InfectedCount = 2;
	
	// 낮/밤 페이즈 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	float DayNightPhaseDuration = 90.0f;
	
	// 트랩 인 페이즈 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	float TrapInPhaseDuration = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	int32 TotalFixFuseBox = 4;

	// GameState 레퍼런스
	UPROPERTY()
	TObjectPtr<ATeam10GameState> Team10GameState;
	
	// 타이머 핸들
	FTimerHandle PhaseTimerHandle;
	
private:
	// 페이즈 타이머 시작
	void StartPhaseTimer(float Duration);
	
	// 페이즈 타이머 업데이트
	void UpdatePhaseTimer();
	
	// 다음 페이즈로 이동
	void AdvanceToNextPhase();

	// 다음 구역 활성화
	void OpenNextArea();

	// 생존자 수 카운트
	int32 GetAliveCitizenCount();
	int32 GetAliveInfectedCount();

#pragma endregion
};