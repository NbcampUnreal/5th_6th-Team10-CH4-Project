# UE 5.6 기준 업데이트 사항

## 주요 변경점

### 1. Build.cs 업데이트 (UE 5.6)

```csharp
// SurvivalGame.Build.cs
using UnrealBuildTool;

public class SurvivalGame : ModuleRules
{
    public SurvivalGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // UE 5.6에서 권장하는 Include Order
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

        // C++ 표준 (UE 5.6은 C++20 지원)
        CppStandard = CppStandardVersion.Cpp20;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "HeadMountedDisplay", // VR 지원 (선택)

            // GAS 모듈
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",

            // State Tree (UE 5.6에서 정식 지원)
            "StateTreeModule",
            "StateTreeEditorModule",

            // 네트워킹
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Sockets",
            "Networking",

            // UI
            "UMG",
            "Slate",
            "SlateCore",

            // 추가 모듈 (UE 5.6)
            "CommonUI", // CommonUI 프레임워크 (권장)
            "AIModule",
            "NavigationSystem"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"
        });

        // Voice Chat (UE 5.6 - EOSVoiceChat 사용 권장)
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDependencyModuleNames.Add("EOSVoiceChat");
        }
    }
}
```

### 2. Target.cs 업데이트

```csharp
// SurvivalGame.Target.cs
using UnrealBuildTool;
using System.Collections.Generic;

public class SurvivalGameTarget : TargetRules
{
    public SurvivalGameTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5; // UE 5.6
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
        ExtraModuleNames.Add("SurvivalGame");

        // UE 5.6 최적화 옵션
        bUseUnityBuild = true;
        bUsePCHFiles = true;
        bUseAutoRTFMCompiler = false; // RTFM 비활성화 (필요시 활성화)
    }
}
```

```csharp
// SurvivalGameServer.Target.cs
using UnrealBuildTool;
using System.Collections.Generic;

public class SurvivalGameServerTarget : TargetRules
{
    public SurvivalGameServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V5; // UE 5.6
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
        ExtraModuleNames.Add("SurvivalGame");

        // 서버 최적화
        bUseLoggingInShipping = true;
        bCompileWithPluginSupport = true;
        bUseUnityBuild = true;
        bUsePCHFiles = true;
    }
}
```

### 3. GAS 업데이트 (UE 5.6)

#### AttributeSet 변경사항

```cpp
// CharacterAttributeSet.h (UE 5.6)
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CharacterAttributeSet.generated.h"

// UE 5.6에서는 ATTRIBUTE_ACCESSORS 매크로가 개선됨
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

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // UE 5.6: PreAttributeChange 대신 PreAttributeBaseChange 추가 사용 권장
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    // Health
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxHealth)

    // Stamina
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Stamina)
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Stamina)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxStamina)
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxStamina)

    // Damage (Meta Attribute)
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Damage;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Damage)

    // Move Speed
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MoveSpeed)

protected:
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
};
```

```cpp
// CharacterAttributeSet.cpp (UE 5.6 추가 부분)
#include "CharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

void UCharacterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);

    // Base Value 변경 전처리 (UE 5.6 신규)
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
    }
}
```

### 4. State Tree 업데이트 (UE 5.6)

UE 5.6에서는 State Tree가 정식 기능으로 포함되었습니다.

```cpp
// ST_CheckPhaseTimer.h (UE 5.6 버전)
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
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

// UE 5.6: FStateTreeTaskBase 구조 변경
USTRUCT(meta = (DisplayName = "Check Phase Timer"))
struct SURVIVALGAME_API FST_CheckPhaseTimer : public FStateTreeTaskBase
{
    GENERATED_BODY()

    using FInstanceDataType = FST_CheckPhaseTimerInstanceData;

    FST_CheckPhaseTimer() = default;

    // UE 5.6: GetInstanceDataType 시그니처 변경
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    // UE 5.6: 새로운 시그니처
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

    virtual void ExitState(FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;
};
```

### 5. Enhanced Input 업데이트 (UE 5.6)

```cpp
// SurvivalCharacter.h - Enhanced Input (UE 5.6)
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h" // UE 5.6
#include "SurvivalCharacter.generated.h"

UCLASS()
class SURVIVALGAME_API ASurvivalCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

protected:
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

    // Input callbacks
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
```

```cpp
// SurvivalCharacter.cpp - Enhanced Input (UE 5.6)
#include "SurvivalCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // UE 5.6: Enhanced Input Subsystem
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
        // Move
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
            this, &ASurvivalCharacter::Move);

        // Look
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,
            this, &ASurvivalCharacter::Look);

        // Jump
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,
            this, &ASurvivalCharacter::StartJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
            this, &ASurvivalCharacter::StopJump);

        // Interact
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started,
            this, &ASurvivalCharacter::TryInteract);

        // Attack
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started,
            this, &ASurvivalCharacter::TryAttack);
    }
}

void ASurvivalCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller)
    {
        // Forward/Backward
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
```

### 6. Online Subsystem 업데이트 (UE 5.6)

UE 5.6에서는 EOS (Epic Online Services) 사용을 강력히 권장합니다.

```ini
; DefaultEngine.ini (UE 5.6 - EOS 사용)

[OnlineSubsystem]
DefaultPlatformService=EOS

[OnlineSubsystemEOS]
bEnabled=true

[/Script/OnlineSubsystemEOS.NetDriverEOS]
bIsUsingP2PSockets=true
NetConnectionClassName="/Script/OnlineSubsystemEOS.NetConnectionEOS"

[/Script/OnlineSubsystemEOS.EOSSettings]
CacheDir=CacheDir
DefaultArtifactName=SurvivalGame
TickBudgetInMilliseconds=0
bEnableOverlay=True
bEnableSocialOverlay=True
bShouldEnforceBeingLaunchedByEGS=False
TitleStorageReadChunkLength=0
+Artifacts=(ArtifactName="SurvivalGame",ClientId="YOUR_CLIENT_ID",ClientSecret="YOUR_CLIENT_SECRET",ProductId="YOUR_PRODUCT_ID",SandboxId="YOUR_SANDBOX_ID",DeploymentId="YOUR_DEPLOYMENT_ID")

; LAN 테스트용으로는 NULL 사용
[OnlineSubsystemNull]
bEnabled=true
```

### 7. Replication Graph (UE 5.6 권장)

UE 5.6에서는 대규모 멀티플레이어를 위해 Replication Graph 사용을 권장합니다.

```cpp
// SurvivalReplicationGraph.h
#pragma once

#include "CoreMinimal.h"
#include "ReplicationGraph.h"
#include "SurvivalReplicationGraph.generated.h"

UCLASS()
class SURVIVALGAME_API USurvivalReplicationGraph : public UReplicationGraph
{
    GENERATED_BODY()

public:
    virtual void InitGlobalActorClassSettings() override;
    virtual void InitGlobalGraphNodes() override;
    virtual void RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo,
        FGlobalActorReplicationInfo& GlobalInfo) override;
    virtual void RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo) override;

protected:
    UPROPERTY()
    class UReplicationGraphNode_GridSpatialization2D* GridNode;

    UPROPERTY()
    class UReplicationGraphNode_ActorList* AlwaysRelevantNode;
};
```

```ini
; DefaultEngine.ini - Replication Graph 활성화
[/Script/OnlineSubsystemUtils.IpNetDriver]
ReplicationDriverClassName="/Script/SurvivalGame.SurvivalReplicationGraph"
```

### 8. 플러그인 설정 (UE 5.6)

```ini
; SurvivalGame.uproject
{
    "FileVersion": 3,
    "EngineAssociation": "5.6",
    "Category": "",
    "Description": "",
    "Modules": [
        {
            "Name": "SurvivalGame",
            "Type": "Runtime",
            "LoadingPhase": "Default",
            "AdditionalDependencies": [
                "Engine",
                "GameplayAbilities"
            ]
        }
    ],
    "Plugins": [
        {
            "Name": "GameplayAbilities",
            "Enabled": true
        },
        {
            "Name": "StateTree",
            "Enabled": true
        },
        {
            "Name": "OnlineSubsystemEOS",
            "Enabled": true
        },
        {
            "Name": "OnlineSubsystemNull",
            "Enabled": true
        },
        {
            "Name": "CommonUI",
            "Enabled": true
        },
        {
            "Name": "EOSVoiceChat",
            "Enabled": true
        }
    ]
}
```

## 주요 차이점 요약

| 항목 | UE 5.3/5.4 | UE 5.6 |
|------|-----------|--------|
| C++ 표준 | C++17 | C++20 |
| BuildSettings | V4 | V5 |
| State Tree | 실험적 기능 | 정식 기능 |
| Online Subsystem | NULL/Steam | EOS 권장 |
| Enhanced Input | 기본 | 필수 |
| Replication Graph | 선택 | 권장 |
| Voice Chat | Voice 모듈 | EOSVoiceChat |
| CommonUI | 선택 | 권장 |

## 주의사항

1. **UE 5.6은 2025년 정식 릴리스**되었습니다
2. API 변경이 있을 수 있으니 공식 문서 확인 필요
3. 데디서버 빌드는 동일하게 작동합니다

기존 코드를 UE 5.6으로 마이그레이션할 때 이 문서를 참고하세요!
