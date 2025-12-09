# 캐릭터 시스템 예시 코드 (강태웅)

## 1. SurvivalCharacter.h - 기본 캐릭터 클래스

```cpp
// SurvivalCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h" // UE 5.6 Enhanced Input
#include "SurvivalCharacter.generated.h"

UCLASS()
class SURVIVALGAME_API ASurvivalCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ASurvivalCharacter();

    // IAbilitySystemInterface 구현
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    // GAS 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    class USurvivalAbilitySystemComponent* AbilitySystemComponent;

    // Attribute Set
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    class UCharacterAttributeSet* AttributeSet;

    // 상호작용 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    class UInteractionComponent* InteractionComponent;

protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    // GAS 초기화
    void InitializeAbilitySystem();
    void GiveDefaultAbilities();
    void ApplyDefaultEffects();

    // 기본 어빌리티 목록
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Abilities")
    TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

    // 기본 효과 목록
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Effects")
    TArray<TSubclassOf<class UGameplayEffect>> DefaultEffects;

    // Enhanced Input (UE 5.6)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* InteractAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* AttackAction;

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Enhanced Input 콜백 함수들
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump();
    void StopJump();

    // 상호작용
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void TryInteract();

    // 공격
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TryAttack();

    // 아이템 사용
    UFUNCTION(BlueprintCallable, Category = "Item")
    void TryUseItem(int32 ItemSlot);

    // Attribute 변경 콜백
    UFUNCTION()
    void OnHealthChanged(const struct FOnAttributeChangeData& Data);

    UFUNCTION()
    void OnStaminaChanged(const struct FOnAttributeChangeData& Data);

    // 사망 처리
    UFUNCTION(BlueprintImplementableEvent, Category = "Character")
    void OnDeath();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastHandleDeath();
};
```

## 2. SurvivalCharacter.cpp

```cpp
// SurvivalCharacter.cpp
#include "SurvivalCharacter.h"
#include "SurvivalAbilitySystemComponent.h"
#include "CharacterAttributeSet.h"
#include "InteractionComponent.h"
#include "SurvivalPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Net/UnrealNetwork.h"

ASurvivalCharacter::ASurvivalCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // GAS 컴포넌트 생성
    AbilitySystemComponent = CreateDefaultSubobject<USurvivalAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    // Attribute Set 생성
    AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));

    // 상호작용 컴포넌트
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

    // 네트워크 설정
    bReplicates = true;
    SetReplicateMovement(true);
}

void ASurvivalCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Attribute 변경 델리게이트 바인딩
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
            AttributeSet->GetHealthAttribute()).AddUObject(this, &ASurvivalCharacter::OnHealthChanged);

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
            AttributeSet->GetStaminaAttribute()).AddUObject(this, &ASurvivalCharacter::OnStaminaChanged);
    }
}

void ASurvivalCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // 서버에서만 실행
    if (HasAuthority())
    {
        InitializeAbilitySystem();
    }
}

void ASurvivalCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // 클라이언트에서 실행
    InitializeAbilitySystem();
}

UAbilitySystemComponent* ASurvivalCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ASurvivalCharacter::InitializeAbilitySystem()
{
    if (!AbilitySystemComponent)
        return;

    ASurvivalPlayerState* PS = GetPlayerState<ASurvivalPlayerState>();
    if (!PS)
        return;

    // PlayerState의 ASC 사용 (권장 방법)
    AbilitySystemComponent = Cast<USurvivalAbilitySystemComponent>(PS->GetAbilitySystemComponent());

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(PS, this);

        if (HasAuthority())
        {
            GiveDefaultAbilities();
            ApplyDefaultEffects();
        }
    }
}

void ASurvivalCharacter::GiveDefaultAbilities()
{
    if (!HasAuthority() || !AbilitySystemComponent)
        return;

    for (TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
    {
        if (Ability)
        {
            AbilitySystemComponent->GiveAbility(
                FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
        }
    }
}

void ASurvivalCharacter::ApplyDefaultEffects()
{
    if (!HasAuthority() || !AbilitySystemComponent)
        return;

    FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    for (TSubclassOf<UGameplayEffect>& Effect : DefaultEffects)
    {
        if (Effect)
        {
            FGameplayEffectSpecHandle SpecHandle =
                AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, EffectContext);

            if (SpecHandle.IsValid())
            {
                AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            }
        }
    }
}

void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // UE 5.6: Enhanced Input Subsystem 등록
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Enhanced Input Component
    if (UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // 이동
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
            this, &ASurvivalCharacter::Move);

        // 시점
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,
            this, &ASurvivalCharacter::Look);

        // 점프
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,
            this, &ASurvivalCharacter::StartJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
            this, &ASurvivalCharacter::StopJump);

        // 상호작용
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started,
            this, &ASurvivalCharacter::TryInteract);

        // 공격
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started,
            this, &ASurvivalCharacter::TryAttack);
    }
}

void ASurvivalCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void ASurvivalCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void ASurvivalCharacter::StartJump()
{
    Jump();
}

void ASurvivalCharacter::StopJump()
{
    StopJumping();
}

void ASurvivalCharacter::TryInteract()
{
    if (InteractionComponent)
    {
        InteractionComponent->TryInteract();
    }
}

void ASurvivalCharacter::TryAttack()
{
    if (!AbilitySystemComponent)
        return;

    // 공격 어빌리티 활성화 (GameplayTag 사용)
    FGameplayTagContainer AbilityTags;
    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack")));

    AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags);
}

void ASurvivalCharacter::TryUseItem(int32 ItemSlot)
{
    // PlayerState에서 아이템 정보 가져오기
    ASurvivalPlayerState* PS = GetPlayerState<ASurvivalPlayerState>();
    if (!PS || !AbilitySystemComponent)
        return;

    // 아이템 사용 어빌리티 활성화
    FGameplayTagContainer AbilityTags;
    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.UseItem")));

    AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags);
}

void ASurvivalCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    // UI 업데이트 (Blueprint에서 구현)
    UE_LOG(LogTemp, Warning, TEXT("Health Changed: %f -> %f"), Data.OldValue, Data.NewValue);

    // 사망 체크
    if (Data.NewValue <= 0.0f && Data.OldValue > 0.0f)
    {
        MulticastHandleDeath();
    }
}

void ASurvivalCharacter::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
    // UI 업데이트
    UE_LOG(LogTemp, Log, TEXT("Stamina Changed: %f -> %f"), Data.OldValue, Data.NewValue);
}

void ASurvivalCharacter::MulticastHandleDeath_Implementation()
{
    // 모든 클라이언트에서 실행
    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 사망 애니메이션/이펙트
    OnDeath();
}
```

## 3. CharacterAttributeSet.h - Attribute 정의

```cpp
// CharacterAttributeSet.h
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CharacterAttributeSet.generated.h"

// Attribute 매크로
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class SURVIVALGAME_API UCharacterAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UCharacterAttributeSet();

    // 네트워크 복제
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Attribute 값 변경 전 처리
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    // GameplayEffect 적용 후 처리
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    // ========== Attributes ==========

    // 체력
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxHealth)

    // 스태미나
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Stamina)
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Stamina)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxStamina)
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxStamina)

    // 데미지 (Meta Attribute - 복제 안 함)
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Damage;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Damage)

    // 이동 속도
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MoveSpeed)

protected:
    // OnRep 함수들
    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

    UFUNCTION()
    void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

    UFUNCTION()
    void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

    // Attribute 값 조정 헬퍼
    void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
                                      const FGameplayAttributeData& MaxAttribute,
                                      float NewMaxValue,
                                      const FGameplayAttribute& AffectedAttributeProperty);
};
```

## 4. CharacterAttributeSet.cpp

```cpp
// CharacterAttributeSet.cpp
#include "CharacterAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UCharacterAttributeSet::UCharacterAttributeSet()
{
    // 기본값 설정
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
    InitStamina(100.0f);
    InitMaxStamina(100.0f);
    InitMoveSpeed(600.0f);
}

void UCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    // 체력은 0~MaxHealth 범위로 제한
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    // 스태미나는 0~MaxStamina 범위로 제한
    else if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
    }
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // 데미지 처리
    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        const float LocalDamage = GetDamage();
        SetDamage(0.0f); // 데미지는 Meta Attribute이므로 즉시 리셋

        if (LocalDamage > 0.0f)
        {
            const float NewHealth = GetHealth() - LocalDamage;
            SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
        }
    }
    // 체력 변경
    else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }
    // 스태미나 변경
    else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
    {
        SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
    }
}

void UCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, Health, OldHealth);
}

void UCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxHealth, OldMaxHealth);
}

void UCharacterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, Stamina, OldStamina);
}

void UCharacterAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxStamina, OldMaxStamina);
}

void UCharacterAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MoveSpeed, OldMoveSpeed);
}
```

## 5. InteractionComponent.h - 상호작용 시스템

```cpp
// InteractionComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

class IInteractable
{
    GENERATED_BODY()

public:
    // 상호작용 가능 여부
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    bool CanInteract(AActor* InteractingActor);

    // 상호작용 실행
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interact(AActor* InteractingActor);

    // 상호작용 프롬프트 텍스트
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    FText GetInteractionPrompt();
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SURVIVALGAME_API UInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInteractionComponent();

    // 상호작용 시도
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void TryInteract();

    // 상호작용 가능한 액터 찾기
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    AActor* FindInteractableActor();

protected:
    virtual void BeginPlay() override;

    // 상호작용 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionDistance = 200.0f;

    // 상호작용 반경
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionRadius = 50.0f;

    // 현재 감지된 상호작용 가능한 액터
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    AActor* CurrentInteractable;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    // 서버 RPC
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerInteract(AActor* InteractableActor);
};
```

---

**다음 파일 예시:**
- GA_Attack.h/cpp (공격 어빌리티)
- GA_Interact.h/cpp (상호작용 어빌리티)
- GE_Damage (데미지 GameplayEffect)

위 코드는 **강태웅(캐릭터 담당)** 학생이 작업할 기본 프레임워크입니다!
