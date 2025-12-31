# 데디서버 멀티플레이어 게임 프로젝트 구조

## 1. 전체 시스템 아키텍처

```mermaid
graph TB
    subgraph "Dedicated Server"
        GM[GameMode<br/>게임 규칙 & 페이즈<br/>C++ 코드]
        GS[GameState<br/>게임 현황 공유]

        GM --> GS
    end

    subgraph "Server & Client"
        PS[PlayerState<br/>플레이어 정보]
        PC[PlayerController<br/>입력 처리]
        CHAR[Character<br/>GAS + 능력]

        PC --> CHAR
        PS --> CHAR
    end

    subgraph "Client Only"
        UI[UI System<br/>HUD/인벤토리]
        INPUT[Player Input]

        INPUT --> PC
        CHAR --> UI
        GS --> UI
    end

    subgraph "Replicated Actors"
        ITEM[Items<br/>무기/소모품]
        GIMMICK[Gimmicks<br/>힐팩/미션오브젝트<br/>StateTree 사용]

        CHAR -.상호작용.-> ITEM
        CHAR -.상호작용.-> GIMMICK
    end

    GM -.Replication.-> GS
    PS -.Replication.-> UI
    CHAR -.GAS Replication.-> UI
    ITEM -.Replication.-> UI
    GIMMICK -.Replication.-> UI
```

## 2. GAS (Gameplay Ability System) 구조

```mermaid
graph LR
    subgraph "Character (GAS 소유자)"
        ASC[AbilitySystemComponent]
        AS[AttributeSet<br/>HP/Stamina]
        GT[GameplayTags<br/>상태관리]
    end

    subgraph "Abilities"
        ATK[공격 Ability]
        HEAL[치료 Ability]
        ITEM_USE[아이템 사용 Ability]
        INTERACT[상호작용 Ability]
    end

    subgraph "Effects"
        DMG[데미지 Effect]
        BUFF[버프 Effect]
        DEBUFF[디버프/감염 Effect]
    end

    ASC --> AS
    ASC --> GT
    ASC --> ATK
    ASC --> HEAL
    ASC --> ITEM_USE
    ASC --> INTERACT

    ATK --> DMG
    HEAL --> BUFF
    ITEM_USE --> BUFF
    DEBUFF --> AS

    AS -.Replicate.-> CLIENT[Client UI]
    GT -.Replicate.-> CLIENT
```

## 3. 게임 페이즈 흐름 (GameMode - C++ 코드로 구현)

> **참고**: 게임 페이즈는 State Tree 대신 **C++ 코드**로 구현합니다. State Tree는 Gimmick에만 사용합니다.

```mermaid
stateDiagram-v2
    [*] --> Lobby

    Lobby --> GameStart: 플레이어 준비완료

    GameStart --> Phase1: 초기화

    Phase1 --> Phase2: 시간경과
    Phase2 --> Phase3: 시간경과
    Phase3 --> VotePhase: 미션완료 또는 시체발견

    VotePhase --> Phase1: 투표완료<br/>계속진행
    VotePhase --> GameEnd: 승리조건달성

    Phase1 --> GameEnd: 승리조건
    Phase2 --> GameEnd: 승리조건
    Phase3 --> GameEnd: 승리조건

    GameEnd --> [*]

    note right of Phase1
        자기장 1단계
        미션 활성화
    end note

    note right of Phase2
        자기장 2단계 축소
        긴급미션 발생
    end note

    note right of Phase3
        자기장 3단계 축소
        최종 대결
    end note

    note right of VotePhase
        투표 시스템
        30초 토론
    end note
```

## 3-1. State Tree 구조 (Gimmick & AI)

> State Tree는 **Gimmick**과 **AI 행동** 관리에 사용합니다. (Behavior Tree 대체)

```mermaid
stateDiagram-v2
    [*] --> Inactive: 기믹 스폰

    state "힐팩 (HealthPack)" as HealthPack {
        Inactive --> ReadyToUse: 게임 시작
        ReadyToUse --> Used: 플레이어 상호작용
        Used --> Respawning: 쿨다운 시작
        Respawning --> ReadyToUse: 쿨다운 완료
    }

    state "미션 오브젝트 (MissionObject)" as Mission {
        MissionInactive --> MissionActive: 페이즈 시작
        MissionActive --> InProgress: 플레이어 시작
        InProgress --> Completed: 미션 완료
        InProgress --> Failed: 시간 초과
    }

    state "세이프존 (SafeZone)" as SafeZone {
        ZoneInactive --> ZoneActive: 페이즈 전환
        ZoneActive --> Shrinking: 축소 시작
        Shrinking --> ZoneActive: 축소 완료
    }
```

## 3-2. AI State Tree 구조

> UE 5.6에서는 **Behavior Tree 대신 State Tree**를 AI에 사용합니다.

```mermaid
stateDiagram-v2
    [*] --> Idle: AI 스폰

    state "감염자 AI (Infected AI)" as InfectedAI {
        Idle --> Patrol: 게임 시작
        Patrol --> Chase: 시민 발견
        Chase --> Attack: 공격 범위 도달
        Attack --> Chase: 타겟 이탈
        Chase --> Patrol: 타겟 사망/소실
        Attack --> Patrol: 공격 성공
    }

    state "시민 AI (Citizen AI)" as CitizenAI {
        CitizenIdle --> DoMission: 미션 발견
        DoMission --> Flee: 감염자 발견
        Flee --> Hide: 은신처 도달
        Hide --> DoMission: 안전 확인
        DoMission --> CitizenIdle: 미션 완료
    }

    state "보스 AI (Boss AI)" as BossAI {
        BossIdle --> Summon: 페이즈 시작
        Summon --> AreaAttack: 소환 완료
        AreaAttack --> Chase: 쿨다운
        Chase --> AreaAttack: 스킬 준비
    }
```

## 4. 네트워크 아키텍처 (서버-클라이언트 통신)

```mermaid
sequenceDiagram
    participant C as Client
    participant S as Server
    participant GAS as GAS (Server)
    participant GS as GameState

    Note over C,GS: 1. 플레이어 아이템 습득
    C->>S: ServerRPC_RequestPickupItem(ItemID)
    S->>S: 검증: 거리/권한 체크
    S->>GAS: GiveAbility(ItemAbility)
    GAS-->>C: OnRep_Attributes (자동복제)
    S->>C: ClientRPC_PlayPickupEffect()

    Note over C,GS: 2. 공격 실행
    C->>S: ServerRPC_TryActivateAbility(AttackAbility)
    S->>GAS: ActivateAbility (서버 실행)
    GAS->>GAS: ApplyGameplayEffect(Damage)
    GAS-->>C: OnRep_Health (자동복제)
    S->>C: MulticastRPC_PlayAttackAnimation()

    Note over C,GS: 3. 게임 페이즈 전환
    S->>GS: SetGamePhase(Phase2)
    GS-->>C: OnRep_GamePhase (자동복제)
    C->>C: UI 업데이트
    S->>C: MulticastRPC_ShowPhaseTransition()

    Note over C,GS: 4. 투표 시스템
    C->>S: ServerRPC_CastVote(TargetPlayer)
    S->>GS: AddVote(TargetPlayer)
    GS-->>C: OnRep_VoteResults (자동복제)
    C->>C: UI 투표 현황 표시
```

## 5. 클래스 상속 구조

```mermaid
classDiagram
    class AGameModeBase {
        <<Server Only>>
    }

    class ASurvivalGameMode {
        +UStateTree* GameFlowStateTree
        +StartGame()
        +HandleVote()
        +CheckWinCondition()
    }

    class AGameStateBase {
        <<Replicated>>
    }

    class ASurvivalGameState {
        +int32 CurrentPhase
        +TArray VoteResults
        +int32 AliveCitizenCount
        +int32 AliveInfectedCount
        +OnRep_CurrentPhase()
        +OnRep_VoteResults()
    }

    class APlayerState {
        <<Replicated>>
    }

    class ASurvivalPlayerState {
        +bool bIsInfected
        +TArray~FItemData~ Inventory
        +int32 Score
        +OnRep_Inventory()
        +OnRep_IsInfected()
    }

    class ACharacter {
        <<Replicated>>
    }

    class ASurvivalCharacter {
        +UAbilitySystemComponent* ASC
        +UCharacterAttributeSet* Attributes
        +ServerRPC_UseItem()
        +OnRep_Health()
    }

    class AActor {
        <<Replicated>>
    }

    class AInteractableActor {
        +UStateTree* InteractionStateTree
        +bool bIsActivated
        +ServerRPC_Interact()
        +OnRep_IsActivated()
    }

    AGameModeBase <|-- ASurvivalGameMode
    AGameStateBase <|-- ASurvivalGameState
    APlayerState <|-- ASurvivalPlayerState
    ACharacter <|-- ASurvivalCharacter
    AActor <|-- AInteractableActor

    ASurvivalGameMode --> ASurvivalGameState : 관리
    ASurvivalCharacter --> ASurvivalPlayerState : 소유
```

## 6. 역할별 담당 클래스 매핑

```mermaid
graph TB
    subgraph "강태웅 - 캐릭터"
        CH1[ASurvivalCharacter]
        CH2[UCitizenAbility]
        CH3[UInfectedAbility]
        CH4[UCharacterAttributeSet]
        CH5[UInteractionComponent]
    end

    subgraph "서원준 - 게임모드"
        GM1[ASurvivalGameMode]
        GM3[UPhaseManager]
        GM4[UWinConditionChecker]
    end

    subgraph "김세훈, 조기혁 - InGame UI"
        UI1[UInGameHUD]
        UI2[UChatWidget]
        UI3[UHealthBarWidget]
        UI4[UInventoryWidget]
        UI5[UVoiceChatComponent]
    end

    subgraph "김현수 - OutGame UI"
        OUI1[UMainMenuWidget]
        OUI2[ULobbyWidget]
        OUI3[UResultWidget]
        OUI4[AMenuGameMode]
    end

    subgraph "한금성 - Gimmick"
        GIM1[AHealthPackActor]
        GIM2[AMissionObjectActor]
        GIM3[ASafeZoneActor]
        GIM4[UGimmickStateTree]
    end

    subgraph "공통 - GameState & PlayerState"
        COMMON1[ASurvivalGameState]
        COMMON2[ASurvivalPlayerState]
    end

    CH1 --> COMMON2
    GM1 --> COMMON1
    UI1 --> COMMON1
    UI1 --> COMMON2
    GIM1 --> COMMON1
```

## 7. 데이터 흐름 (아이템 획득 예시)

```mermaid
sequenceDiagram
    participant Player as Player Character
    participant Item as Item Actor
    participant Server as Server
    participant PS as PlayerState
    participant GAS as AbilitySystemComponent
    participant UI as Client UI

    Player->>Item: Overlap Event (Trigger)
    Item->>Player: Show Interaction Prompt (E)
    Player->>Server: ServerRPC_RequestPickupItem(Item)

    Server->>Server: Validate (Distance, Authority)

    alt Validation Success
        Server->>PS: AddItemToInventory(ItemData)
        PS->>PS: Inventory.Add(ItemData)
        PS-->>UI: OnRep_Inventory()
        UI->>UI: Update Inventory UI

        Server->>GAS: GiveAbility(ItemAbility)
        GAS-->>Player: OnRep_GrantedAbilities()

        Server->>Item: Destroy()
        Server->>Player: ClientRPC_PlayPickupEffect()
    else Validation Failed
        Server->>Player: ClientRPC_ShowErrorMessage()
    end
```

## 8. 폴더 구조

```
Source/
├── SurvivalGame/
│   ├── Characters/
│   │   ├── SurvivalCharacter.h/cpp
│   │   ├── CitizenCharacter.h/cpp
│   │   └── InfectedCharacter.h/cpp
│   ├── GAS/
│   │   ├── Abilities/
│   │   │   ├── GA_Attack.h/cpp
│   │   │   ├── GA_Heal.h/cpp
│   │   │   ├── GA_UseItem.h/cpp
│   │   │   └── GA_Interact.h/cpp
│   │   ├── Attributes/
│   │   │   └── CharacterAttributeSet.h/cpp
│   │   ├── Effects/
│   │   │   ├── GE_Damage.h/cpp
│   │   │   ├── GE_Heal.h/cpp
│   │   │   └── GE_Infection.h/cpp
│   │   └── AbilitySystemComponent/
│   │       └── SurvivalASC.h/cpp
│   ├── GameModes/
│   │   ├── SurvivalGameMode.h/cpp
│   │   ├── MenuGameMode.h/cpp
│   │   └── LobbyGameMode.h/cpp
│   ├── GameStates/
│   │   └── SurvivalGameState.h/cpp
│   ├── PlayerStates/
│   │   └── SurvivalPlayerState.h/cpp
│   ├── PlayerControllers/
│   │   └── SurvivalPlayerController.h/cpp
│   ├── AI/
│   │   ├── SurvivalAIController.h/cpp
│   │   ├── InfectedAIController.h/cpp
│   │   ├── CitizenAIController.h/cpp
│   │   └── Components/
│   │       ├── AIPerceptionComponent.h/cpp
│   │       └── AIStateTreeComponent.h/cpp
│   ├── StateTrees/
│   │   ├── Gimmicks/
│   │   │   ├── ST_HealthPack (Asset)
│   │   │   ├── ST_MissionObject (Asset)
│   │   │   └── ST_SafeZone (Asset)
│   │   ├── AI/
│   │   │   ├── ST_InfectedAI (Asset)
│   │   │   ├── ST_CitizenAI (Asset)
│   │   │   └── ST_BossAI (Asset)
│   │   └── Tasks/
│   │       ├── ST_ActivateGimmick.h/cpp
│   │       ├── STTask_Patrol.h/cpp
│   │       ├── STTask_Chase.h/cpp
│   │       ├── STTask_Attack.h/cpp
│   │       └── STTask_Flee.h/cpp
│   ├── Items/
│   │   ├── ItemBase.h/cpp
│   │   ├── WeaponItem.h/cpp
│   │   └── ConsumableItem.h/cpp
│   ├── Gimmicks/
│   │   ├── HealthPackActor.h/cpp
│   │   ├── MissionObjectActor.h/cpp
│   │   └── SafeZoneActor.h/cpp
│   ├── UI/
│   │   ├── InGame/
│   │   │   ├── InGameHUD.h/cpp
│   │   │   ├── ChatWidget.h/cpp
│   │   │   ├── HealthBarWidget.h/cpp
│   │   │   └── InventoryWidget.h/cpp
│   │   └── OutGame/
│   │       ├── MainMenuWidget.h/cpp
│   │       ├── LobbyWidget.h/cpp
│   │       └── ResultWidget.h/cpp
│   └── Online/
│       ├── SurvivalGameInstance.h/cpp
│       ├── VoiceChatComponent.h/cpp
│       └── VotingSystem.h/cpp
```

---
