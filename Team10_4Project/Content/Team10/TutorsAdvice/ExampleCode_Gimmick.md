# Gimmick 시스템 예시 코드 (한금성)

## 1. InteractableActor.h - 상호작용 가능한 액터 베이스 클래스

```cpp
// InteractableActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionComponent.h"
#include "InteractableActor.generated.h"

UCLASS()
class SURVIVALGAME_API AInteractableActor : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    AInteractableActor();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 메시
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    // State Tree (기믹 로직)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateTree")
    class UStateTreeComponent* GimmickStateTree;

    // 활성화 상태
    UPROPERTY(ReplicatedUsing = OnRep_IsActivated, BlueprintReadOnly, Category = "Interaction")
    bool bIsActivated;

    UFUNCTION()
    void OnRep_IsActivated();

    // IInteractable 인터페이스 구현
    virtual bool CanInteract_Implementation(AActor* InteractingActor) override;
    virtual void Interact_Implementation(AActor* InteractingActor) override;
    virtual FText GetInteractionPrompt_Implementation() override;

protected:
    virtual void BeginPlay() override;

    // 상호작용 프롬프트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    FText InteractionPrompt;

    // 재사용 가능 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    bool bReusable = false;

    // 재사용 쿨다운
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float RespawnDelay = 30.0f;

    // 서버 상호작용
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerInteract(AActor* InteractingActor);

    // 재활성화
    UFUNCTION()
    void ReactivateGimmick();

    FTimerHandle RespawnTimerHandle;

    // Blueprint 이벤트
    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
    void OnInteracted(AActor* InteractingActor);

    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
    void OnActivated();

    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
    void OnDeactivated();
};
```

## 2. InteractableActor.cpp

```cpp
// InteractableActor.cpp
#include "InteractableActor.h"
#include "StateTreeComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"

AInteractableActor::AInteractableActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 메시 컴포넌트
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    // State Tree 컴포넌트
    GimmickStateTree = CreateDefaultSubobject<UStateTreeComponent>(TEXT("GimmickStateTree"));

    // 네트워크 복제
    bReplicates = true;
    SetReplicateMovement(false); // 고정 오브젝트

    bIsActivated = false;
    InteractionPrompt = FText::FromString("Press E to Interact");
}

void AInteractableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AInteractableActor, bIsActivated);
}

void AInteractableActor::BeginPlay()
{
    Super::BeginPlay();
}

bool AInteractableActor::CanInteract_Implementation(AActor* InteractingActor)
{
    // 이미 활성화되었고 재사용 불가능하면 상호작용 불가
    return !bIsActivated || bReusable;
}

void AInteractableActor::Interact_Implementation(AActor* InteractingActor)
{
    // 클라이언트에서 호출 → 서버로 전달
    if (!HasAuthority())
    {
        ServerInteract(InteractingActor);
        return;
    }

    // 서버에서 실행
    if (!CanInteract_Implementation(InteractingActor))
        return;

    bIsActivated = true;
    OnActivated();

    // Blueprint 이벤트
    OnInteracted(InteractingActor);

    // 재사용 가능하면 타이머 설정
    if (bReusable && RespawnDelay > 0.0f)
    {
        GetWorldTimerManager().SetTimer(RespawnTimerHandle, this,
            &AInteractableActor::ReactivateGimmick, RespawnDelay, false);
    }
}

FText AInteractableActor::GetInteractionPrompt_Implementation()
{
    return InteractionPrompt;
}

void AInteractableActor::ServerInteract_Implementation(AActor* InteractingActor)
{
    Interact_Implementation(InteractingActor);
}

bool AInteractableActor::ServerInteract_Validate(AActor* InteractingActor)
{
    return true;
}

void AInteractableActor::ReactivateGimmick()
{
    bIsActivated = false;
    OnDeactivated();

    UE_LOG(LogTemp, Log, TEXT("%s reactivated"), *GetName());
}

void AInteractableActor::OnRep_IsActivated()
{
    if (bIsActivated)
    {
        OnActivated();
    }
    else
    {
        OnDeactivated();
    }
}
```

## 3. HealthPackActor.h - 힐팩 기믹

```cpp
// HealthPackActor.h
#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "HealthPackActor.generated.h"

UCLASS()
class SURVIVALGAME_API AHealthPackActor : public AInteractableActor
{
    GENERATED_BODY()

public:
    AHealthPackActor();

    virtual void Interact_Implementation(AActor* InteractingActor) override;

protected:
    // 회복량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthPack")
    float HealAmount = 50.0f;

    // GameplayEffect 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
    TSubclassOf<class UGameplayEffect> HealEffect;
};
```

## 4. HealthPackActor.cpp

```cpp
// HealthPackActor.cpp
#include "HealthPackActor.h"
#include "SurvivalCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

AHealthPackActor::AHealthPackActor()
{
    InteractionPrompt = FText::FromString("Press E to Heal");
    bReusable = true;
    RespawnDelay = 60.0f; // 60초 후 재생성
}

void AHealthPackActor::Interact_Implementation(AActor* InteractingActor)
{
    // 부모 클래스 호출
    Super::Interact_Implementation(InteractingActor);

    if (!HasAuthority())
        return;

    // 캐릭터 확인
    ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(InteractingActor);
    if (!Character)
        return;

    UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
    if (!ASC)
        return;

    // GameplayEffect 적용 (체력 회복)
    if (HealEffect)
    {
        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
        EffectContext.AddSourceObject(this);

        FGameplayEffectSpecHandle SpecHandle =
            ASC->MakeOutgoingSpec(HealEffect, 1, EffectContext);

        if (SpecHandle.IsValid())
        {
            // Magnitude를 동적으로 설정
            SpecHandle.Data->SetSetByCallerMagnitude(
                FGameplayTag::RequestGameplayTag(FName("Data.Healing")), HealAmount);

            ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

            UE_LOG(LogTemp, Log, TEXT("%s healed %s by %f HP"),
                *GetName(), *Character->GetName(), HealAmount);
        }
    }

    // 비주얼 비활성화 (Blueprint에서 구현)
    MeshComponent->SetVisibility(false);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
```

## 5. MissionObjectActor.h - 미션 오브젝트

```cpp
// MissionObjectActor.h
#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "MissionObjectActor.generated.h"

UENUM(BlueprintType)
enum class EMissionType : uint8
{
    Instant UMETA(DisplayName = "즉시 완료"),
    Progress UMETA(DisplayName = "진행도"),
    Timed UMETA(DisplayName = "시간 기반")
};

UCLASS()
class SURVIVALGAME_API AMissionObjectActor : public AInteractableActor
{
    GENERATED_BODY()

public:
    AMissionObjectActor();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Interact_Implementation(AActor* InteractingActor) override;
    virtual void Tick(float DeltaTime) override;

protected:
    // 미션 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    EMissionType MissionType;

    // 미션 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    FName MissionID;

    // 진행도 (0.0 ~ 1.0)
    UPROPERTY(ReplicatedUsing = OnRep_Progress, BlueprintReadOnly, Category = "Mission")
    float Progress;

    UFUNCTION()
    void OnRep_Progress();

    // 완료 여부
    UPROPERTY(ReplicatedUsing = OnRep_IsCompleted, BlueprintReadOnly, Category = "Mission")
    bool bIsCompleted;

    UFUNCTION()
    void OnRep_IsCompleted();

    // 완료에 필요한 시간 (Timed 타입)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    float CompletionTime = 10.0f;

    // 현재 상호작용 중인 플레이어들
    UPROPERTY()
    TArray<AActor*> InteractingPlayers;

    // 진행도 증가율 (초당)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    float ProgressRate = 0.1f;

    // 미션 완료
    UFUNCTION()
    void CompleteMission();

    // Blueprint 이벤트
    UFUNCTION(BlueprintImplementableEvent, Category = "Mission")
    void OnMissionCompleted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Mission")
    void OnProgressUpdated(float NewProgress);
};
```

## 6. MissionObjectActor.cpp

```cpp
// MissionObjectActor.cpp
#include "MissionObjectActor.h"
#include "Net/UnrealNetwork.h"

AMissionObjectActor::AMissionObjectActor()
{
    PrimaryActorTick.bCanEverTick = true;

    MissionType = EMissionType::Progress;
    Progress = 0.0f;
    bIsCompleted = false;
    bReusable = false;

    InteractionPrompt = FText::FromString("Press E to Start Mission");
}

void AMissionObjectActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMissionObjectActor, Progress);
    DOREPLIFETIME(AMissionObjectActor, bIsCompleted);
}

void AMissionObjectActor::Interact_Implementation(AActor* InteractingActor)
{
    if (!HasAuthority())
    {
        ServerInteract(InteractingActor);
        return;
    }

    if (bIsCompleted)
        return;

    switch (MissionType)
    {
    case EMissionType::Instant:
        // 즉시 완료
        CompleteMission();
        break;

    case EMissionType::Progress:
    case EMissionType::Timed:
        // 상호작용 시작
        if (!InteractingPlayers.Contains(InteractingActor))
        {
            InteractingPlayers.Add(InteractingActor);
            UE_LOG(LogTemp, Log, TEXT("%s started interacting with mission %s"),
                *InteractingActor->GetName(), *GetName());
        }
        break;
    }
}

void AMissionObjectActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!HasAuthority() || bIsCompleted || InteractingPlayers.Num() == 0)
        return;

    // 진행도 증가
    Progress += DeltaTime * ProgressRate * InteractingPlayers.Num();
    Progress = FMath::Clamp(Progress, 0.0f, 1.0f);

    OnProgressUpdated(Progress);

    // 완료 체크
    if (Progress >= 1.0f)
    {
        CompleteMission();
    }
}

void AMissionObjectActor::CompleteMission()
{
    if (!HasAuthority() || bIsCompleted)
        return;

    bIsCompleted = true;
    Progress = 1.0f;

    UE_LOG(LogTemp, Warning, TEXT("Mission %s completed!"), *MissionID.ToString());

    OnMissionCompleted();

    // GameState에 미션 완료 알림 (TODO)
}

void AMissionObjectActor::OnRep_Progress()
{
    OnProgressUpdated(Progress);
}

void AMissionObjectActor::OnRep_IsCompleted()
{
    if (bIsCompleted)
    {
        OnMissionCompleted();
    }
}
```

## 7. SafeZoneActor.h - 자기장 시스템

```cpp
// SafeZoneActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SafeZoneActor.generated.h"

UCLASS()
class SURVIVALGAME_API ASafeZoneActor : public AActor
{
    GENERATED_BODY()

public:
    ASafeZoneActor();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Tick(float DeltaTime) override;

    // State Tree 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateTree")
    class UStateTreeComponent* SafeZoneStateTree;

    // 안전지대 표시용 구체
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* SafeZoneSphere;

    // 데미지 구체 (안전지대 밖)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* DamageZoneSphere;

    // 자기장 축소 시작
    UFUNCTION(BlueprintCallable, Category = "SafeZone")
    void StartShrinking(float TargetRadius, float ShrinkDuration);

    // 현재 반경
    UPROPERTY(ReplicatedUsing = OnRep_CurrentRadius, BlueprintReadOnly, Category = "SafeZone")
    float CurrentRadius;

    UFUNCTION()
    void OnRep_CurrentRadius();

protected:
    virtual void BeginPlay() override;

    // 초기 반경
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SafeZone")
    float InitialRadius = 5000.0f;

    // 목표 반경
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "SafeZone")
    float TargetRadius;

    // 축소 시간
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "SafeZone")
    float ShrinkDuration;

    // 축소 중 여부
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "SafeZone")
    bool bIsShrinking;

    // 경과 시간
    float ElapsedShrinkTime;

    // 데미지 (초당)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SafeZone")
    float DamagePerSecond = 5.0f;

    // Overlap 이벤트
    UFUNCTION()
    void OnDamageZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnSafeZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // 캐릭터에게 데미지 적용
    UFUNCTION()
    void ApplyDamageToCharacters();

    FTimerHandle DamageTimerHandle;

    // 안전지대 밖에 있는 캐릭터들
    UPROPERTY()
    TArray<AActor*> CharactersOutsideSafeZone;
};
```

## 8. SafeZoneActor.cpp

```cpp
// SafeZoneActor.cpp
#include "SafeZoneActor.h"
#include "Components/SphereComponent.h"
#include "StateTreeComponent.h"
#include "SurvivalCharacter.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

ASafeZoneActor::ASafeZoneActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // 루트 컴포넌트
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // 안전지대 구체
    SafeZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SafeZoneSphere"));
    SafeZoneSphere->SetupAttachment(RootComponent);
    SafeZoneSphere->SetSphereRadius(InitialRadius);
    SafeZoneSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SafeZoneSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

    // 데미지 구체 (전체 맵)
    DamageZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageZoneSphere"));
    DamageZoneSphere->SetupAttachment(RootComponent);
    DamageZoneSphere->SetSphereRadius(100000.0f); // 매우 큰 반경
    DamageZoneSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DamageZoneSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

    // State Tree
    SafeZoneStateTree = CreateDefaultSubobject<UStateTreeComponent>(TEXT("SafeZoneStateTree"));

    bReplicates = true;
    SetReplicateMovement(false);

    CurrentRadius = InitialRadius;
    bIsShrinking = false;
}

void ASafeZoneActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASafeZoneActor, CurrentRadius);
    DOREPLIFETIME(ASafeZoneActor, TargetRadius);
    DOREPLIFETIME(ASafeZoneActor, ShrinkDuration);
    DOREPLIFETIME(ASafeZoneActor, bIsShrinking);
}

void ASafeZoneActor::BeginPlay()
{
    Super::BeginPlay();

    CurrentRadius = InitialRadius;
    SafeZoneSphere->SetSphereRadius(CurrentRadius);

    // Overlap 이벤트 바인딩
    DamageZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &ASafeZoneActor::OnDamageZoneBeginOverlap);
    SafeZoneSphere->OnComponentEndOverlap.AddDynamic(this, &ASafeZoneActor::OnSafeZoneEndOverlap);

    // 데미지 타이머 시작
    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(DamageTimerHandle, this,
            &ASafeZoneActor::ApplyDamageToCharacters, 1.0f, true);
    }
}

void ASafeZoneActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!HasAuthority() || !bIsShrinking)
        return;

    // 반경 축소
    ElapsedShrinkTime += DeltaTime;
    float Alpha = FMath::Clamp(ElapsedShrinkTime / ShrinkDuration, 0.0f, 1.0f);

    CurrentRadius = FMath::Lerp(InitialRadius, TargetRadius, Alpha);
    SafeZoneSphere->SetSphereRadius(CurrentRadius);

    // 축소 완료
    if (Alpha >= 1.0f)
    {
        bIsShrinking = false;
        InitialRadius = TargetRadius;
        UE_LOG(LogTemp, Warning, TEXT("Safe Zone shrink completed! New Radius: %f"), CurrentRadius);
    }
}

void ASafeZoneActor::StartShrinking(float NewTargetRadius, float NewShrinkDuration)
{
    if (!HasAuthority())
        return;

    TargetRadius = NewTargetRadius;
    ShrinkDuration = NewShrinkDuration;
    bIsShrinking = true;
    ElapsedShrinkTime = 0.0f;

    UE_LOG(LogTemp, Warning, TEXT("Safe Zone shrinking from %f to %f over %f seconds"),
        CurrentRadius, TargetRadius, ShrinkDuration);
}

void ASafeZoneActor::OnDamageZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority())
        return;

    ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(OtherActor);
    if (Character && !CharactersOutsideSafeZone.Contains(Character))
    {
        CharactersOutsideSafeZone.Add(Character);
    }
}

void ASafeZoneActor::OnSafeZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // 안전지대를 벗어남
    // CharactersOutsideSafeZone에 추가는 DamageZone Overlap에서 처리됨
}

void ASafeZoneActor::ApplyDamageToCharacters()
{
    if (!HasAuthority())
        return;

    for (int32 i = CharactersOutsideSafeZone.Num() - 1; i >= 0; i--)
    {
        ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(CharactersOutsideSafeZone[i]);
        if (!Character || !Character->IsValidLowLevel())
        {
            CharactersOutsideSafeZone.RemoveAt(i);
            continue;
        }

        // 안전지대 안에 있는지 체크
        float DistanceFromCenter = FVector::Dist(GetActorLocation(), Character->GetActorLocation());
        if (DistanceFromCenter <= CurrentRadius)
        {
            // 안전지대 안 → 제거
            CharactersOutsideSafeZone.RemoveAt(i);
            continue;
        }

        // 데미지 적용 (GAS)
        UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
        if (ASC)
        {
            // TODO: GameplayEffect로 데미지 적용
            UE_LOG(LogTemp, Log, TEXT("%s taking safe zone damage!"), *Character->GetName());
        }
    }
}

void ASafeZoneActor::OnRep_CurrentRadius()
{
    // 클라이언트에서 반경 업데이트
    SafeZoneSphere->SetSphereRadius(CurrentRadius);
}
```

---

## State Tree 구성 예시 (Gimmick)

```
GimmickStateTree (HealthPack):
├── State: "Available"
│   └── Transition: OnInteract → "Used"
├── State: "Used"
│   ├── Task: ST_WaitTimer (RespawnDelay)
│   └── Transition: TimerExpired → "Available"
```

```
SafeZoneStateTree:
├── State: "Phase1"
│   ├── Radius: 5000
│   └── Transition: GamePhase == Phase2 → "Phase2Shrink"
├── State: "Phase2Shrink"
│   ├── Task: ST_ShrinkZone (Target: 3000, Duration: 30s)
│   └── Transition: ShrinkComplete → "Phase2"
├── State: "Phase2"
│   └── Transition: GamePhase == Phase3 → "Phase3Shrink"
├── State: "Phase3Shrink"
│   ├── Task: ST_ShrinkZone (Target: 1000, Duration: 30s)
│   └── Transition: ShrinkComplete → "Phase3"
```

---

이 코드는 **한금성(Gimmick 담당)** 학생이 작업할 내용입니다!
