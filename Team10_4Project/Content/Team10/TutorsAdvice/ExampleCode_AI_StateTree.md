# AI State Tree 예제 코드 (UE 5.6)

> UE 5.6에서는 **Behavior Tree 대신 State Tree**를 AI에 사용합니다.

## 1. AI Controller 기본 구조

### SurvivalAIController.h

```cpp
// SurvivalAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StateTreeReference.h"
#include "SurvivalAIController.generated.h"

class UStateTreeComponent;
class UAIPerceptionComponent;

UCLASS()
class SURVIVALGAME_API ASurvivalAIController : public AAIController
{
    GENERATED_BODY()

public:
    ASurvivalAIController();

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    // State Tree Component (UE 5.6)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    TObjectPtr<UStateTreeComponent> StateTreeComponent;

    // AI Perception
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

    // State Tree 에셋
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|StateTree")
    FStateTreeReference StateTreeRef;

public:
    // State Tree 시작/정지
    UFUNCTION(BlueprintCallable, Category = "AI")
    void StartStateTree();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void StopStateTree();

    // Perception 콜백
    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    // 타겟 관리
    UPROPERTY(BlueprintReadWrite, Category = "AI")
    TObjectPtr<AActor> CurrentTarget;

    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetTarget(AActor* NewTarget);

    UFUNCTION(BlueprintPure, Category = "AI")
    AActor* GetTarget() const { return CurrentTarget; }
};
```

### SurvivalAIController.cpp

```cpp
// SurvivalAIController.cpp
#include "SurvivalAIController.h"
#include "StateTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

ASurvivalAIController::ASurvivalAIController()
{
    // State Tree Component 생성
    StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));

    // AI Perception Component 생성
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SetPerceptionComponent(*AIPerceptionComponent);

    // 시각 감지 설정
    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1500.f;
    SightConfig->LoseSightRadius = 2000.f;
    SightConfig->PeripheralVisionAngleDegrees = 60.f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    AIPerceptionComponent->ConfigureSense(*SightConfig);

    // 청각 감지 설정
    UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 2000.f;
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    AIPerceptionComponent->ConfigureSense(*HearingConfig);

    AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
}

void ASurvivalAIController::BeginPlay()
{
    Super::BeginPlay();

    // Perception 이벤트 바인딩
    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
            this, &ASurvivalAIController::OnPerceptionUpdated);
    }
}

void ASurvivalAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // State Tree 시작
    StartStateTree();
}

void ASurvivalAIController::OnUnPossess()
{
    StopStateTree();
    Super::OnUnPossess();
}

void ASurvivalAIController::StartStateTree()
{
    if (StateTreeComponent && StateTreeRef.IsValid())
    {
        StateTreeComponent->SetStateTree(StateTreeRef.GetStateTree());
        StateTreeComponent->Start();
    }
}

void ASurvivalAIController::StopStateTree()
{
    if (StateTreeComponent)
    {
        StateTreeComponent->Stop();
    }
}

void ASurvivalAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        // 타겟 발견
        SetTarget(Actor);
        UE_LOG(LogTemp, Log, TEXT("AI: Target detected - %s"), *Actor->GetName());
    }
    else
    {
        // 타겟 상실
        if (CurrentTarget == Actor)
        {
            SetTarget(nullptr);
            UE_LOG(LogTemp, Log, TEXT("AI: Target lost - %s"), *Actor->GetName());
        }
    }
}

void ASurvivalAIController::SetTarget(AActor* NewTarget)
{
    CurrentTarget = NewTarget;

    // State Tree에 타겟 정보 전달 (Schema를 통해)
    // StateTreeComponent의 Context에서 접근 가능
}
```

---

## 2. 감염자 AI Controller

### InfectedAIController.h

```cpp
// InfectedAIController.h
#pragma once

#include "CoreMinimal.h"
#include "SurvivalAIController.h"
#include "InfectedAIController.generated.h"

UENUM(BlueprintType)
enum class EInfectedAIState : uint8
{
    Idle,
    Patrol,
    Chase,
    Attack,
    Search
};

UCLASS()
class SURVIVALGAME_API AInfectedAIController : public ASurvivalAIController
{
    GENERATED_BODY()

public:
    AInfectedAIController();

protected:
    // 현재 AI 상태 (State Tree에서 관리하지만 디버깅용)
    UPROPERTY(BlueprintReadOnly, Category = "AI|State")
    EInfectedAIState CurrentState;

    // 공격 범위
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Combat")
    float AttackRange = 200.f;

    // 공격 쿨다운
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Combat")
    float AttackCooldown = 1.5f;

    // 순찰 반경
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Patrol")
    float PatrolRadius = 1000.f;

public:
    // 타겟이 공격 범위 안에 있는지
    UFUNCTION(BlueprintPure, Category = "AI|Combat")
    bool IsTargetInAttackRange() const;

    // 공격 실행
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void ExecuteAttack();

    // 감염 시도
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void TryInfectTarget();

    // 랜덤 순찰 위치 가져오기
    UFUNCTION(BlueprintCallable, Category = "AI|Patrol")
    FVector GetRandomPatrolLocation() const;
};
```

### InfectedAIController.cpp

```cpp
// InfectedAIController.cpp
#include "InfectedAIController.h"
#include "SurvivalCharacter.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

AInfectedAIController::AInfectedAIController()
{
    CurrentState = EInfectedAIState::Idle;
}

bool AInfectedAIController::IsTargetInAttackRange() const
{
    if (!CurrentTarget || !GetPawn()) return false;

    float Distance = FVector::Dist(GetPawn()->GetActorLocation(), CurrentTarget->GetActorLocation());
    return Distance <= AttackRange;
}

void AInfectedAIController::ExecuteAttack()
{
    if (!CurrentTarget) return;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    // GAS를 통해 공격 어빌리티 실행
    if (ASurvivalCharacter* AICharacter = Cast<ASurvivalCharacter>(ControlledPawn))
    {
        AICharacter->TryAttack();
    }
}

void AInfectedAIController::TryInfectTarget()
{
    if (!CurrentTarget) return;

    // 타겟이 시민인지 확인
    if (ASurvivalCharacter* TargetCharacter = Cast<ASurvivalCharacter>(CurrentTarget))
    {
        // 감염 GameplayEffect 적용
        // GAS를 통해 처리
        UE_LOG(LogTemp, Log, TEXT("Infected AI: Attempting to infect %s"), *TargetCharacter->GetName());
    }
}

FVector AInfectedAIController::GetRandomPatrolLocation() const
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return FVector::ZeroVector;

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSystem) return ControlledPawn->GetActorLocation();

    FNavLocation RandomLocation;
    bool bFound = NavSystem->GetRandomReachablePointInRadius(
        ControlledPawn->GetActorLocation(),
        PatrolRadius,
        RandomLocation
    );

    return bFound ? RandomLocation.Location : ControlledPawn->GetActorLocation();
}
```

---

## 3. State Tree Tasks

### STTask_Patrol.h

```cpp
// STTask_Patrol.h
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STTask_Patrol.generated.h"

USTRUCT(meta = (DisplayName = "Patrol Task"))
struct SURVIVALGAME_API FSTTask_Patrol : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTTask_PatrolInstanceData;

    // 순찰 반경
    UPROPERTY(EditAnywhere, Category = "Patrol")
    float PatrolRadius = 1000.f;

    // 대기 시간 (목적지 도착 후)
    UPROPERTY(EditAnywhere, Category = "Patrol")
    float WaitTime = 2.f;

    // 이동 허용 오차
    UPROPERTY(EditAnywhere, Category = "Patrol")
    float AcceptanceRadius = 50.f;

protected:
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FSTTask_PatrolInstanceData::StaticStruct();
    }

    virtual EStateTreeRunStatus EnterState(
        FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    virtual EStateTreeRunStatus Tick(
        FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

    virtual void ExitState(
        FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;
};

USTRUCT()
struct FSTTask_PatrolInstanceData
{
    GENERATED_BODY()

    // 현재 목적지
    UPROPERTY()
    FVector CurrentDestination;

    // 대기 타이머
    UPROPERTY()
    float WaitTimer = 0.f;

    // 현재 대기 중인지
    UPROPERTY()
    bool bIsWaiting = false;

    // 목적지에 도착했는지
    UPROPERTY()
    bool bReachedDestination = false;
};
```

### STTask_Patrol.cpp

```cpp
// STTask_Patrol.cpp
#include "STTask_Patrol.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"

EStateTreeRunStatus FSTTask_Patrol::EnterState(
    FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition) const
{
    FSTTask_PatrolInstanceData& InstanceData = Context.GetInstanceData<FSTTask_PatrolInstanceData>(*this);

    // AI Controller 가져오기
    AAIController* AIController = nullptr;
    if (APawn* Pawn = Context.GetOwner<APawn>())
    {
        AIController = Cast<AAIController>(Pawn->GetController());
    }

    if (!AIController) return EStateTreeRunStatus::Failed;

    // 랜덤 순찰 위치 생성
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Context.GetWorld());
    if (NavSystem)
    {
        FNavLocation RandomLocation;
        bool bFound = NavSystem->GetRandomReachablePointInRadius(
            AIController->GetPawn()->GetActorLocation(),
            PatrolRadius,
            RandomLocation
        );

        if (bFound)
        {
            InstanceData.CurrentDestination = RandomLocation.Location;
            InstanceData.bIsWaiting = false;
            InstanceData.bReachedDestination = false;

            // 이동 시작
            AIController->MoveToLocation(InstanceData.CurrentDestination, AcceptanceRadius);
        }
    }

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTTask_Patrol::Tick(
    FStateTreeExecutionContext& Context,
    const float DeltaTime) const
{
    FSTTask_PatrolInstanceData& InstanceData = Context.GetInstanceData<FSTTask_PatrolInstanceData>(*this);

    AAIController* AIController = nullptr;
    if (APawn* Pawn = Context.GetOwner<APawn>())
    {
        AIController = Cast<AAIController>(Pawn->GetController());
    }

    if (!AIController) return EStateTreeRunStatus::Failed;

    // 대기 중인 경우
    if (InstanceData.bIsWaiting)
    {
        InstanceData.WaitTimer += DeltaTime;
        if (InstanceData.WaitTimer >= WaitTime)
        {
            // 새로운 순찰 위치로 이동
            InstanceData.bIsWaiting = false;
            InstanceData.WaitTimer = 0.f;

            UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Context.GetWorld());
            if (NavSystem)
            {
                FNavLocation RandomLocation;
                bool bFound = NavSystem->GetRandomReachablePointInRadius(
                    AIController->GetPawn()->GetActorLocation(),
                    PatrolRadius,
                    RandomLocation
                );

                if (bFound)
                {
                    InstanceData.CurrentDestination = RandomLocation.Location;
                    AIController->MoveToLocation(InstanceData.CurrentDestination, AcceptanceRadius);
                }
            }
        }
    }
    else
    {
        // 목적지 도착 확인
        float Distance = FVector::Dist(
            AIController->GetPawn()->GetActorLocation(),
            InstanceData.CurrentDestination
        );

        if (Distance <= AcceptanceRadius)
        {
            InstanceData.bIsWaiting = true;
            InstanceData.WaitTimer = 0.f;
            AIController->StopMovement();
        }
    }

    return EStateTreeRunStatus::Running;
}

void FSTTask_Patrol::ExitState(
    FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition) const
{
    // 이동 중지
    if (APawn* Pawn = Context.GetOwner<APawn>())
    {
        if (AAIController* AIController = Cast<AAIController>(Pawn->GetController()))
        {
            AIController->StopMovement();
        }
    }
}
```

---

### STTask_Chase.h

```cpp
// STTask_Chase.h
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STTask_Chase.generated.h"

USTRUCT(meta = (DisplayName = "Chase Task"))
struct SURVIVALGAME_API FSTTask_Chase : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    // 추적 업데이트 간격
    UPROPERTY(EditAnywhere, Category = "Chase")
    float UpdateInterval = 0.2f;

    // 추적 허용 오차
    UPROPERTY(EditAnywhere, Category = "Chase")
    float AcceptanceRadius = 100.f;

    // 추적 포기 거리
    UPROPERTY(EditAnywhere, Category = "Chase")
    float GiveUpDistance = 3000.f;

protected:
    virtual EStateTreeRunStatus EnterState(
        FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    virtual EStateTreeRunStatus Tick(
        FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

    virtual void ExitState(
        FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

private:
    mutable float UpdateTimer = 0.f;
};
```

### STTask_Chase.cpp

```cpp
// STTask_Chase.cpp
#include "STTask_Chase.h"
#include "StateTreeExecutionContext.h"
#include "SurvivalAIController.h"
#include "GameFramework/Pawn.h"

EStateTreeRunStatus FSTTask_Chase::EnterState(
    FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition) const
{
    UpdateTimer = 0.f;

    APawn* Pawn = Context.GetOwner<APawn>();
    if (!Pawn) return EStateTreeRunStatus::Failed;

    ASurvivalAIController* AIController = Cast<ASurvivalAIController>(Pawn->GetController());
    if (!AIController) return EStateTreeRunStatus::Failed;

    AActor* Target = AIController->GetTarget();
    if (!Target) return EStateTreeRunStatus::Failed;

    // 타겟 위치로 이동 시작
    AIController->MoveToActor(Target, AcceptanceRadius);

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTTask_Chase::Tick(
    FStateTreeExecutionContext& Context,
    const float DeltaTime) const
{
    APawn* Pawn = Context.GetOwner<APawn>();
    if (!Pawn) return EStateTreeRunStatus::Failed;

    ASurvivalAIController* AIController = Cast<ASurvivalAIController>(Pawn->GetController());
    if (!AIController) return EStateTreeRunStatus::Failed;

    AActor* Target = AIController->GetTarget();
    if (!Target)
    {
        // 타겟 상실
        return EStateTreeRunStatus::Failed;
    }

    // 추적 포기 거리 확인
    float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
    if (Distance > GiveUpDistance)
    {
        return EStateTreeRunStatus::Failed;
    }

    // 주기적으로 경로 업데이트
    UpdateTimer += DeltaTime;
    if (UpdateTimer >= UpdateInterval)
    {
        UpdateTimer = 0.f;
        AIController->MoveToActor(Target, AcceptanceRadius);
    }

    // 공격 범위에 도달했는지 확인은 Transition에서 처리

    return EStateTreeRunStatus::Running;
}

void FSTTask_Chase::ExitState(
    FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition) const
{
    if (APawn* Pawn = Context.GetOwner<APawn>())
    {
        if (AAIController* AIController = Cast<AAIController>(Pawn->GetController()))
        {
            AIController->StopMovement();
        }
    }
}
```

---

### STTask_Attack.h

```cpp
// STTask_Attack.h
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STTask_Attack.generated.h"

USTRUCT(meta = (DisplayName = "Attack Task"))
struct SURVIVALGAME_API FSTTask_Attack : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    // 공격 쿨다운
    UPROPERTY(EditAnywhere, Category = "Attack")
    float AttackCooldown = 1.5f;

    // 공격 범위
    UPROPERTY(EditAnywhere, Category = "Attack")
    float AttackRange = 200.f;

protected:
    virtual EStateTreeRunStatus EnterState(
        FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    virtual EStateTreeRunStatus Tick(
        FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

private:
    mutable float CooldownTimer = 0.f;
    mutable bool bCanAttack = true;
};
```

### STTask_Attack.cpp

```cpp
// STTask_Attack.cpp
#include "STTask_Attack.h"
#include "StateTreeExecutionContext.h"
#include "SurvivalAIController.h"
#include "SurvivalCharacter.h"
#include "GameFramework/Pawn.h"

EStateTreeRunStatus FSTTask_Attack::EnterState(
    FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition) const
{
    CooldownTimer = 0.f;
    bCanAttack = true;

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTTask_Attack::Tick(
    FStateTreeExecutionContext& Context,
    const float DeltaTime) const
{
    APawn* Pawn = Context.GetOwner<APawn>();
    if (!Pawn) return EStateTreeRunStatus::Failed;

    ASurvivalAIController* AIController = Cast<ASurvivalAIController>(Pawn->GetController());
    if (!AIController) return EStateTreeRunStatus::Failed;

    AActor* Target = AIController->GetTarget();
    if (!Target) return EStateTreeRunStatus::Failed;

    // 타겟 방향으로 회전
    FVector Direction = (Target->GetActorLocation() - Pawn->GetActorLocation()).GetSafeNormal();
    FRotator LookAtRotation = Direction.Rotation();
    Pawn->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));

    // 거리 확인
    float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
    if (Distance > AttackRange)
    {
        // 타겟이 범위를 벗어남 - Chase로 전환
        return EStateTreeRunStatus::Failed;
    }

    // 쿨다운 처리
    if (!bCanAttack)
    {
        CooldownTimer += DeltaTime;
        if (CooldownTimer >= AttackCooldown)
        {
            bCanAttack = true;
            CooldownTimer = 0.f;
        }
    }

    // 공격 실행
    if (bCanAttack)
    {
        if (ASurvivalCharacter* AICharacter = Cast<ASurvivalCharacter>(Pawn))
        {
            AICharacter->TryAttack();
            bCanAttack = false;
        }
    }

    return EStateTreeRunStatus::Running;
}
```

---

## 4. State Tree Schema (Context 정의)

### SurvivalAIStateTreeSchema.h

```cpp
// SurvivalAIStateTreeSchema.h
#pragma once

#include "CoreMinimal.h"
#include "StateTreeSchema.h"
#include "SurvivalAIStateTreeSchema.generated.h"

UCLASS(BlueprintType, EditInlineNew, CollapseCategories)
class SURVIVALGAME_API USurvivalAIStateTreeSchema : public UStateTreeSchema
{
    GENERATED_BODY()

public:
    USurvivalAIStateTreeSchema();

    virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;
    virtual bool IsClassAllowed(const UClass* InClass) const override;
    virtual bool IsExternalItemAllowed(const UStruct& InStruct) const override;
};
```

### SurvivalAIStateTreeSchema.cpp

```cpp
// SurvivalAIStateTreeSchema.cpp
#include "SurvivalAIStateTreeSchema.h"
#include "StateTreeConditionBase.h"
#include "StateTreeTaskBase.h"

USurvivalAIStateTreeSchema::USurvivalAIStateTreeSchema()
{
    // AI Schema로 설정
}

bool USurvivalAIStateTreeSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
    // 기본 Task/Condition 허용
    return InScriptStruct->IsChildOf(FStateTreeTaskCommonBase::StaticStruct()) ||
           InScriptStruct->IsChildOf(FStateTreeConditionCommonBase::StaticStruct());
}

bool USurvivalAIStateTreeSchema::IsClassAllowed(const UClass* InClass) const
{
    return true;
}

bool USurvivalAIStateTreeSchema::IsExternalItemAllowed(const UStruct& InStruct) const
{
    return true;
}
```

---

## 5. State Tree Conditions

### STCondition_HasTarget.h

```cpp
// STCondition_HasTarget.h
#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "STCondition_HasTarget.generated.h"

USTRUCT(meta = (DisplayName = "Has Target"))
struct SURVIVALGAME_API FSTCondition_HasTarget : public FStateTreeConditionCommonBase
{
    GENERATED_BODY()

    // 반전 여부 (타겟이 없을 때 true)
    UPROPERTY(EditAnywhere, Category = "Condition")
    bool bInvert = false;

protected:
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
```

### STCondition_HasTarget.cpp

```cpp
// STCondition_HasTarget.cpp
#include "STCondition_HasTarget.h"
#include "StateTreeExecutionContext.h"
#include "SurvivalAIController.h"
#include "GameFramework/Pawn.h"

bool FSTCondition_HasTarget::TestCondition(FStateTreeExecutionContext& Context) const
{
    APawn* Pawn = Context.GetOwner<APawn>();
    if (!Pawn) return bInvert;

    ASurvivalAIController* AIController = Cast<ASurvivalAIController>(Pawn->GetController());
    if (!AIController) return bInvert;

    bool bHasTarget = (AIController->GetTarget() != nullptr);

    return bInvert ? !bHasTarget : bHasTarget;
}
```

### STCondition_InRange.h

```cpp
// STCondition_InRange.h
#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "STCondition_InRange.generated.h"

USTRUCT(meta = (DisplayName = "Target In Range"))
struct SURVIVALGAME_API FSTCondition_InRange : public FStateTreeConditionCommonBase
{
    GENERATED_BODY()

    // 확인할 범위
    UPROPERTY(EditAnywhere, Category = "Condition")
    float Range = 200.f;

    // 반전 여부
    UPROPERTY(EditAnywhere, Category = "Condition")
    bool bInvert = false;

protected:
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
```

### STCondition_InRange.cpp

```cpp
// STCondition_InRange.cpp
#include "STCondition_InRange.h"
#include "StateTreeExecutionContext.h"
#include "SurvivalAIController.h"
#include "GameFramework/Pawn.h"

bool FSTCondition_InRange::TestCondition(FStateTreeExecutionContext& Context) const
{
    APawn* Pawn = Context.GetOwner<APawn>();
    if (!Pawn) return bInvert;

    ASurvivalAIController* AIController = Cast<ASurvivalAIController>(Pawn->GetController());
    if (!AIController) return bInvert;

    AActor* Target = AIController->GetTarget();
    if (!Target) return bInvert;

    float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
    bool bInRange = (Distance <= Range);

    return bInvert ? !bInRange : bInRange;
}
```

---

## 6. Blueprint에서 State Tree 에셋 생성

### 에디터에서 State Tree 생성 방법

1. **Content Browser** → 우클릭 → **Artificial Intelligence** → **State Tree**
2. State Tree 에셋 더블클릭하여 에디터 열기
3. **Schema**를 `SurvivalAIStateTreeSchema`로 설정

### 감염자 AI State Tree 구조 예시

```
Root
├── [Selector] Main
│   ├── [State] Attack
│   │   ├── Condition: HasTarget AND InRange(200)
│   │   └── Task: STTask_Attack
│   │
│   ├── [State] Chase
│   │   ├── Condition: HasTarget
│   │   └── Task: STTask_Chase
│   │
│   └── [State] Patrol
│       └── Task: STTask_Patrol
```

---

## 7. Build.cs에 모듈 추가

```csharp
// SurvivalGame.Build.cs
PublicDependencyModuleNames.AddRange(new string[]
{
    // ... 기존 모듈들 ...

    // AI 모듈
    "AIModule",
    "NavigationSystem",

    // State Tree (UE 5.6)
    "StateTreeModule",
    "GameplayStateTreeModule"  // AI용 State Tree
});
```

---

## 요약

| 파일 | 역할 |
|------|------|
| `SurvivalAIController` | AI 기본 컨트롤러 (State Tree + Perception) |
| `InfectedAIController` | 감염자 AI 특화 로직 |
| `STTask_Patrol` | 순찰 State Tree Task |
| `STTask_Chase` | 추적 State Tree Task |
| `STTask_Attack` | 공격 State Tree Task |
| `STCondition_HasTarget` | 타겟 존재 여부 조건 |
| `STCondition_InRange` | 범위 내 존재 조건 |

State Tree는 **Behavior Tree보다 시각적으로 명확하고 디버깅이 쉬운** 장점이 있습니다!
