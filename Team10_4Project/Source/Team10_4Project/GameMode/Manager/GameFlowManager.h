// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameMode/GameTypes/GameTypes.h"
#include "GameFlowManager.generated.h"


class UPlayerSpawnManager;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentAreaChangedDelegate, EGameArea)

class ATeam10GameState;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM10_4PROJECT_API UGameFlowManager : public UActorComponent
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	
	// 게임 시작
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void StartGame();

	// 페이즈 전환
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void ChangePhase(EGamePhase NewPhase);

	// 다음 구역 오픈
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void OpenNextArea();
	
	// 게임 종료
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void EndGame(EGameResult Result);

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void AreaChanged(FGameplayTag AreaTag);
protected:
	// 낮/밤 페이즈 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	float DayNightPhaseDuration = 90.0f;
	
	// 트랩 인 페이즈 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	float TrapInPhaseDuration = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	int32 TotalFuseBoxCount = 4;
	
	// GameState 레퍼런스
	UPROPERTY()
	TObjectPtr<ATeam10GameState> Team10GameState;

	// 타이머 핸들
	FTimerHandle PhaseTimerHandle;

private:
	// 남은 퓨즈박스 개수 초기화
	void InitializeRemainingFuseBoxes();
	
	// 페이즈 타이머 시작
	void StartPhaseTimer(float Duration);

	// 페이즈 타이머 업데이트
	void UpdatePhaseTimer();

	// 다음 페이즈로 이동
	void AdvanceToNextPhase();
	
	// 탈출구 활성화
	void ActivateExit();
	
	// 현재 구역 설정
	void SetCurrentArea(EGameArea NewArea);

	UPROPERTY()
	TObjectPtr<UPlayerSpawnManager> PlayerSpawnManager;
	
public:
	FOnCurrentAreaChangedDelegate OnCurrentAreaChangedDelegate;
};
