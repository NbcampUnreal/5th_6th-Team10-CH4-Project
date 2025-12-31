# 게임모드 & State Tree 예시 코드 (서원준)

## 1. SurvivalGameMode.h - 게임 모드

```cpp
// SurvivalGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SurvivalGameMode.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    Lobby UMETA(DisplayName = "로비"),
    Phase1 UMETA(DisplayName = "1단계"),
    Phase2 UMETA(DisplayName = "2단계"),
    Phase3 UMETA(DisplayName = "3단계"),
    Vote UMETA(DisplayName = "투표"),
    GameEnd UMETA(DisplayName = "종료")
};

UENUM(BlueprintType)
enum class EGameResult : uint8
{
    None,
    CitizenWin UMETA(DisplayName = "시민 승리"),
    InfectedWin UMETA(DisplayName = "감염자 승리")
};

UCLASS()
class SURVIVALGAME_API ASurvivalGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASurvivalGameMode();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;

    // State Tree 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateTree")
    class UStateTreeComponent* GameFlowStateTree;

    // 게임 시작
    UFUNCTION(BlueprintCallable, Category = "GameFlow")
    void StartGame();

    // 페이즈 전환
    UFUNCTION(BlueprintCallable, Category = "GameFlow")
    void ChangePhase(EGamePhase NewPhase);

    // 투표 시스템
    UFUNCTION(BlueprintCallable, Category = "Vote")
    void StartVoting();

    UFUNCTION(BlueprintCallable, Category = "Vote")
    void CastVote(APlayerController* Voter, APlayerController* Target);

    UFUNCTION(BlueprintCallable, Category = "Vote")
    void EndVoting();

    // 승리 조건 체크
    UFUNCTION(BlueprintCallable, Category = "GameFlow")
    void CheckWinCondition();

    UFUNCTION(BlueprintCallable, Category = "GameFlow")
    void EndGame(EGameResult Result);

    // 플레이어 사망 처리
    UFUNCTION(BlueprintCallable, Category = "Player")
    void HandlePlayerDeath(APlayerController* DeadPlayer);

    // 감염자 배정
    UFUNCTION(BlueprintCallable, Category = "GameFlow")
    void AssignInfectedPlayers();

protected:
    // 현재 페이즈
    UPROPERTY(BlueprintReadOnly, Category = "GameFlow")
    EGamePhase CurrentPhase;

    // 최소 플레이어 수
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
    int32 MinPlayersToStart = 4;

    // 감염자 비율 (0.0 ~ 1.0)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
    float InfectedRatio = 0.25f;

    // 페이즈별 시간 (초)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
    float Phase1Duration = 180.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
    float Phase2Duration = 180.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
    float Phase3Duration = 180.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
    float VoteDuration = 60.0f;

    // 투표 결과 저장
    UPROPERTY(BlueprintReadOnly, Category = "Vote")
    TMap<APlayerController*, int32> VoteResults;

    // GameState 레퍼런스
    UPROPERTY()
    class ASurvivalGameState* SurvivalGameState;

    // 타이머 핸들
    FTimerHandle PhaseTimerHandle;
    FTimerHandle VoteTimerHandle;

private:
    // 페이즈 타이머 시작
    void StartPhaseTimer(float Duration);

    // 다음 페이즈로 이동
    void AdvanceToNextPhase();

    // 생존자 수 카운트
    int32 GetAliveCitizenCount();
    int32 GetAliveInfectedCount();
};
```

## 2. SurvivalGameMode.cpp

```cpp
// SurvivalGameMode.cpp
#include "SurvivalGameMode.h"
#include "SurvivalGameState.h"
#include "SurvivalPlayerState.h"
#include "SurvivalPlayerController.h"
#include "StateTreeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ASurvivalGameMode::ASurvivalGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    // State Tree 컴포넌트 생성
    GameFlowStateTree = CreateDefaultSubobject<UStateTreeComponent>(TEXT("GameFlowStateTree"));

    // 기본 클래스 설정
    GameStateClass = ASurvivalGameState::StaticClass();
    PlayerStateClass = ASurvivalPlayerState::StaticClass();
    PlayerControllerClass = ASurvivalPlayerController::StaticClass();

    CurrentPhase = EGamePhase::Lobby;
}

void ASurvivalGameMode::BeginPlay()
{
    Super::BeginPlay();

    SurvivalGameState = Cast<ASurvivalGameState>(GameState);

    if (SurvivalGameState)
    {
        SurvivalGameState->SetCurrentPhase(CurrentPhase);
    }
}

void ASurvivalGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // State Tree 업데이트는 자동으로 처리됨
}

void ASurvivalGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    UE_LOG(LogTemp, Log, TEXT("Player Logged In: %s"), *NewPlayer->GetName());

    // 충분한 플레이어가 모이면 게임 시작
    if (CurrentPhase == EGamePhase::Lobby)
    {
        int32 PlayerCount = GameState->PlayerArray.Num();
        if (PlayerCount >= MinPlayersToStart)
        {
            // 모든 플레이어가 준비되었는지 확인 (생략 가능)
            StartGame();
        }
    }
}

void ASurvivalGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    UE_LOG(LogTemp, Log, TEXT("Player Logged Out: %s"), *Exiting->GetName());

    // 게임 중 플레이어 나감 처리
    if (CurrentPhase != EGamePhase::Lobby && CurrentPhase != EGamePhase::GameEnd)
    {
        CheckWinCondition();
    }
}

void ASurvivalGameMode::StartGame()
{
    if (CurrentPhase != EGamePhase::Lobby)
        return;

    UE_LOG(LogTemp, Warning, TEXT("Game Starting!"));

    // 감염자 배정
    AssignInfectedPlayers();

    // Phase1으로 전환
    ChangePhase(EGamePhase::Phase1);

    if (SurvivalGameState)
    {
        SurvivalGameState->OnGameStart();
    }
}

void ASurvivalGameMode::ChangePhase(EGamePhase NewPhase)
{
    CurrentPhase = NewPhase;

    if (SurvivalGameState)
    {
        SurvivalGameState->SetCurrentPhase(NewPhase);
    }

    UE_LOG(LogTemp, Warning, TEXT("Phase Changed to: %d"), (int32)NewPhase);

    // 페이즈별 타이머 설정
    switch (NewPhase)
    {
    case EGamePhase::Phase1:
        StartPhaseTimer(Phase1Duration);
        break;
    case EGamePhase::Phase2:
        StartPhaseTimer(Phase2Duration);
        // 자기장 축소 시작
        if (SurvivalGameState)
        {
            SurvivalGameState->StartSafeZoneShrink(1);
        }
        break;
    case EGamePhase::Phase3:
        StartPhaseTimer(Phase3Duration);
        // 자기장 추가 축소
        if (SurvivalGameState)
        {
            SurvivalGameState->StartSafeZoneShrink(2);
        }
        break;
    case EGamePhase::Vote:
        StartVoting();
        break;
    case EGamePhase::GameEnd:
        // 게임 종료 처리는 EndGame에서
        break;
    default:
        break;
    }
}

void ASurvivalGameMode::StartPhaseTimer(float Duration)
{
    GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
    GetWorldTimerManager().SetTimer(PhaseTimerHandle, this,
        &ASurvivalGameMode::AdvanceToNextPhase, Duration, false);

    if (SurvivalGameState)
    {
        SurvivalGameState->SetPhaseTimeRemaining(Duration);
    }
}

void ASurvivalGameMode::AdvanceToNextPhase()
{
    switch (CurrentPhase)
    {
    case EGamePhase::Phase1:
        ChangePhase(EGamePhase::Phase2);
        break;
    case EGamePhase::Phase2:
        ChangePhase(EGamePhase::Phase3);
        break;
    case EGamePhase::Phase3:
        // Phase3 종료 시 승리 조건 체크
        CheckWinCondition();
        break;
    case EGamePhase::Vote:
        EndVoting();
        break;
    default:
        break;
    }
}

void ASurvivalGameMode::AssignInfectedPlayers()
{
    TArray<APlayerState*>& Players = GameState->PlayerArray;

    int32 InfectedCount = FMath::Max(1, FMath::RoundToInt(Players.Num() * InfectedRatio));

    // 랜덤으로 감염자 선택
    TArray<int32> RandomIndices;
    for (int32 i = 0; i < Players.Num(); i++)
    {
        RandomIndices.Add(i);
    }

    // Fisher-Yates Shuffle
    for (int32 i = RandomIndices.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        RandomIndices.Swap(i, j);
    }

    // 감염자 배정
    for (int32 i = 0; i < InfectedCount; i++)
    {
        ASurvivalPlayerState* PS = Cast<ASurvivalPlayerState>(Players[RandomIndices[i]]);
        if (PS)
        {
            PS->SetIsInfected(true);
            UE_LOG(LogTemp, Warning, TEXT("Player %s is now INFECTED"), *PS->GetPlayerName());
        }
    }

    // 나머지는 시민
    for (int32 i = InfectedCount; i < Players.Num(); i++)
    {
        ASurvivalPlayerState* PS = Cast<ASurvivalPlayerState>(Players[RandomIndices[i]]);
        if (PS)
        {
            PS->SetIsInfected(false);
            UE_LOG(LogTemp, Log, TEXT("Player %s is a CITIZEN"), *PS->GetPlayerName());
        }
    }
}

void ASurvivalGameMode::StartVoting()
{
    CurrentPhase = EGamePhase::Vote;

    if (SurvivalGameState)
    {
        SurvivalGameState->SetCurrentPhase(EGamePhase::Vote);
        SurvivalGameState->ClearVoteResults();
    }

    VoteResults.Empty();

    // 투표 타이머 시작
    GetWorldTimerManager().SetTimer(VoteTimerHandle, this,
        &ASurvivalGameMode::EndVoting, VoteDuration, false);

    UE_LOG(LogTemp, Warning, TEXT("Voting Started!"));
}

void ASurvivalGameMode::CastVote(APlayerController* Voter, APlayerController* Target)
{
    if (CurrentPhase != EGamePhase::Vote)
        return;

    if (!Voter || !Target)
        return;

    // 투표 집계
    if (!VoteResults.Contains(Target))
    {
        VoteResults.Add(Target, 0);
    }
    VoteResults[Target]++;

    // GameState에 결과 동기화
    if (SurvivalGameState)
    {
        SurvivalGameState->UpdateVoteResults(VoteResults);
    }

    UE_LOG(LogTemp, Log, TEXT("%s voted for %s"), *Voter->GetName(), *Target->GetName());
}

void ASurvivalGameMode::EndVoting()
{
    GetWorldTimerManager().ClearTimer(VoteTimerHandle);

    // 최다 득표자 찾기
    APlayerController* MostVoted = nullptr;
    int32 MaxVotes = 0;

    for (auto& Pair : VoteResults)
    {
        if (Pair.Value > MaxVotes)
        {
            MaxVotes = Pair.Value;
            MostVoted = Pair.Key;
        }
    }

    // 과반수 이상 득표 시 처형
    int32 TotalPlayers = GameState->PlayerArray.Num();
    if (MostVoted && MaxVotes > TotalPlayers / 2)
    {
        HandlePlayerDeath(MostVoted);
        UE_LOG(LogTemp, Warning, TEXT("%s was eliminated by vote!"), *MostVoted->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No one was eliminated (not enough votes)"));
    }

    // 승리 조건 체크
    CheckWinCondition();

    // 다음 페이즈로 이동 (또는 게임 종료)
    if (CurrentPhase == EGamePhase::Vote)
    {
        ChangePhase(EGamePhase::Phase1);
    }
}

void ASurvivalGameMode::CheckWinCondition()
{
    int32 AliveCitizens = GetAliveCitizenCount();
    int32 AliveInfected = GetAliveInfectedCount();

    UE_LOG(LogTemp, Log, TEXT("Alive - Citizens: %d, Infected: %d"), AliveCitizens, AliveInfected);

    // 감염자 전멸 → 시민 승리
    if (AliveInfected == 0)
    {
        EndGame(EGameResult::CitizenWin);
    }
    // 감염자 수 >= 시민 수 → 감염자 승리
    else if (AliveInfected >= AliveCitizens)
    {
        EndGame(EGameResult::InfectedWin);
    }
}

void ASurvivalGameMode::EndGame(EGameResult Result)
{
    CurrentPhase = EGamePhase::GameEnd;

    if (SurvivalGameState)
    {
        SurvivalGameState->SetCurrentPhase(EGamePhase::GameEnd);
        SurvivalGameState->SetGameResult(Result);
    }

    UE_LOG(LogTemp, Warning, TEXT("Game Ended! Result: %d"), (int32)Result);

    // 5초 후 결과 화면으로 이동
    FTimerHandle EndGameTimer;
    GetWorldTimerManager().SetTimer(EndGameTimer, [this]()
    {
        // 모든 플레이어를 결과 화면으로 이동
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PC = It->Get();
            if (PC)
            {
                // TODO: 결과 화면 UI 표시
            }
        }
    }, 5.0f, false);
}

void ASurvivalGameMode::HandlePlayerDeath(APlayerController* DeadPlayer)
{
    if (!DeadPlayer)
        return;

    ASurvivalPlayerState* PS = DeadPlayer->GetPlayerState<ASurvivalPlayerState>();
    if (PS)
    {
        PS->SetIsAlive(false);
    }

    // 캐릭터 사망 처리
    APawn* Pawn = DeadPlayer->GetPawn();
    if (Pawn)
    {
        Pawn->Destroy();
    }

    UE_LOG(LogTemp, Warning, TEXT("Player %s has died"), *DeadPlayer->GetName());
}

int32 ASurvivalGameMode::GetAliveCitizenCount()
{
    int32 Count = 0;
    for (APlayerState* PS : GameState->PlayerArray)
    {
        ASurvivalPlayerState* SPS = Cast<ASurvivalPlayerState>(PS);
        if (SPS && SPS->IsAlive() && !SPS->IsInfected())
        {
            Count++;
        }
    }
    return Count;
}

int32 ASurvivalGameMode::GetAliveInfectedCount()
{
    int32 Count = 0;
    for (APlayerState* PS : GameState->PlayerArray)
    {
        ASurvivalPlayerState* SPS = Cast<ASurvivalPlayerState>(PS);
        if (SPS && SPS->IsAlive() && SPS->IsInfected())
        {
            Count++;
        }
    }
    return Count;
}
```

## 3. SurvivalGameState.h - 게임 상태 동기화

```cpp
// SurvivalGameState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SurvivalGameMode.h"
#include "SurvivalGameState.generated.h"

UCLASS()
class SURVIVALGAME_API ASurvivalGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ASurvivalGameState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 현재 게임 페이즈
    UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase, BlueprintReadOnly, Category = "GameState")
    EGamePhase CurrentPhase;

    UFUNCTION()
    void OnRep_CurrentPhase();

    // 페이즈 남은 시간
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
    float PhaseTimeRemaining;

    // 게임 결과
    UPROPERTY(ReplicatedUsing = OnRep_GameResult, BlueprintReadOnly, Category = "GameState")
    EGameResult GameResult;

    UFUNCTION()
    void OnRep_GameResult();

    // 투표 결과
    UPROPERTY(ReplicatedUsing = OnRep_VoteResults, BlueprintReadOnly, Category = "Vote")
    TMap<FString, int32> VoteResultsReplicated; // PlayerName -> VoteCount

    UFUNCTION()
    void OnRep_VoteResults();

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
    void UpdateVoteResults(const TMap<APlayerController*, int32>& VoteMap);

    UFUNCTION(BlueprintCallable, Category = "GameState")
    void ClearVoteResults();

    // 자기장 관련
    UFUNCTION(BlueprintCallable, Category = "SafeZone")
    void StartSafeZoneShrink(int32 Level);

    UFUNCTION(BlueprintCallable, Category = "GameState")
    void OnGameStart();
};
```

## 4. State Tree 태스크 예시 - ST_CheckPhaseTimer.h

```cpp
// ST_CheckPhaseTimer.h
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "ST_CheckPhaseTimer.generated.h"

USTRUCT()
struct FST_CheckPhaseTimerInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Input")
    float TimerDuration = 180.0f;

    UPROPERTY()
    float ElapsedTime = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Output")
    bool bTimerExpired = false;
};

USTRUCT()
struct FST_CheckPhaseTimer : public FStateTreeTaskBase
{
    GENERATED_BODY()

    using FInstanceDataType = FST_CheckPhaseTimerInstanceData;

    FST_CheckPhaseTimer() = default;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};
```

## 5. State Tree 태스크 구현 - ST_CheckPhaseTimer.cpp

```cpp
// ST_CheckPhaseTimer.cpp
#include "ST_CheckPhaseTimer.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FST_CheckPhaseTimer::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    // 타이머 초기화
    InstanceData.ElapsedTime = 0.0f;
    InstanceData.bTimerExpired = false;

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FST_CheckPhaseTimer::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    InstanceData.ElapsedTime += DeltaTime;

    // 타이머 만료 체크
    if (InstanceData.ElapsedTime >= InstanceData.TimerDuration)
    {
        InstanceData.bTimerExpired = true;
        return EStateTreeRunStatus::Succeeded;
    }

    return EStateTreeRunStatus::Running;
}
```

---

**State Tree Asset 구성 (Blueprint/Asset 에디터에서 설정):**

```
GameFlowStateTree:
├── Root State: "Lobby"
│   └── Transition: PlayerCount >= MinPlayers → "Phase1"
├── State: "Phase1"
│   ├── Task: ST_CheckPhaseTimer (180초)
│   ├── Task: ST_ActivateGimmicks (Level 1)
│   └── Transitions:
│       ├── TimerExpired → "Phase2"
│       └── WinConditionMet → "GameEnd"
├── State: "Phase2"
│   ├── Task: ST_CheckPhaseTimer (180초)
│   ├── Task: ST_ShrinkSafeZone (Level 1)
│   └── Transitions:
│       ├── TimerExpired → "Phase3"
│       └── WinConditionMet → "GameEnd"
├── State: "Phase3"
│   ├── Task: ST_CheckPhaseTimer (180초)
│   ├── Task: ST_ShrinkSafeZone (Level 2)
│   └── Transitions:
│       ├── TimerExpired → "Vote"
│       └── WinConditionMet → "GameEnd"
├── State: "Vote"
│   ├── Task: ST_CheckPhaseTimer (60초)
│   └── Transition: TimerExpired → "Phase1" or "GameEnd"
└── State: "GameEnd"
    └── Task: ST_ShowResults
```

이 코드는 **서원준(게임모드 담당)** 학생이 작업할 내용입니다!
