// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//#include "GameMode/GameTypes/GameTypes.h"
#include "GameFlowManager.generated.h"

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

	// 게임 종료
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void EndGame(EGameResult Result);
	
protected:
	// 낮/밤 페이즈 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	float DayNightPhaseDuration = 90.0f;
	
	// 트랩 인 페이즈 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	float TrapInPhaseDuration = 60.0f;
	
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
};
