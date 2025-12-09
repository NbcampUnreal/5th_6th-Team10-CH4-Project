# UI & 인벤토리 시스템 예시 코드 (김세훈, 조기혁, 김현수)

## 1. SurvivalPlayerState.h - 인벤토리 데이터 저장

```cpp
// SurvivalPlayerState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "SurvivalPlayerState.generated.h"

USTRUCT(BlueprintType)
struct FItemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ItemName;

    FItemData()
        : ItemID(NAME_None), Quantity(0), Icon(nullptr), ItemName(FText::GetEmpty())
    {}
};

UCLASS()
class SURVIVALGAME_API ASurvivalPlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ASurvivalPlayerState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // GAS 지원
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    class USurvivalAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    class UCharacterAttributeSet* AttributeSet;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    // 플레이어 역할
    UPROPERTY(ReplicatedUsing = OnRep_IsInfected, BlueprintReadOnly, Category = "Player")
    bool bIsInfected;

    UFUNCTION()
    void OnRep_IsInfected();

    // 생존 여부
    UPROPERTY(ReplicatedUsing = OnRep_IsAlive, BlueprintReadOnly, Category = "Player")
    bool bIsAlive;

    UFUNCTION()
    void OnRep_IsAlive();

    // 인벤토리
    UPROPERTY(ReplicatedUsing = OnRep_Inventory, BlueprintReadOnly, Category = "Inventory")
    TArray<FItemData> Inventory;

    UFUNCTION()
    void OnRep_Inventory();

    // 점수
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
    int32 Score;

    // Setter 함수들
    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetIsInfected(bool bInfected);

    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetIsAlive(bool bAlive);

    // 인벤토리 관리
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItemToInventory(const FItemData& Item);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItemFromInventory(FName ItemID, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
    FItemData GetItemByID(FName ItemID) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
    bool HasItem(FName ItemID) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player")
    bool IsInfected() const { return bIsInfected; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player")
    bool IsAlive() const { return bIsAlive; }

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    int32 MaxInventorySize = 20;
};
```

## 2. SurvivalPlayerState.cpp

```cpp
// SurvivalPlayerState.cpp
#include "SurvivalPlayerState.h"
#include "SurvivalAbilitySystemComponent.h"
#include "CharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"

ASurvivalPlayerState::ASurvivalPlayerState()
{
    // GAS 컴포넌트 생성 (PlayerState에 배치 권장)
    AbilitySystemComponent = CreateDefaultSubobject<USurvivalAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));

    // 기본값
    bIsInfected = false;
    bIsAlive = true;
    Score = 0;

    // 네트워크 업데이트 빈도 증가 (PlayerState는 기본적으로 느림)
    NetUpdateFrequency = 100.0f;
}

void ASurvivalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASurvivalPlayerState, bIsInfected);
    DOREPLIFETIME(ASurvivalPlayerState, bIsAlive);
    DOREPLIFETIME(ASurvivalPlayerState, Inventory);
    DOREPLIFETIME(ASurvivalPlayerState, Score);
}

UAbilitySystemComponent* ASurvivalPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ASurvivalPlayerState::OnRep_IsInfected()
{
    // UI 업데이트 또는 캐릭터 비주얼 변경
    UE_LOG(LogTemp, Warning, TEXT("Player %s is now %s"),
        *GetPlayerName(), bIsInfected ? TEXT("INFECTED") : TEXT("CITIZEN"));
}

void ASurvivalPlayerState::OnRep_IsAlive()
{
    UE_LOG(LogTemp, Warning, TEXT("Player %s is now %s"),
        *GetPlayerName(), bIsAlive ? TEXT("ALIVE") : TEXT("DEAD"));
}

void ASurvivalPlayerState::OnRep_Inventory()
{
    // 인벤토리 UI 업데이트
    UE_LOG(LogTemp, Log, TEXT("Inventory Updated for %s"), *GetPlayerName());

    // Blueprint 이벤트 호출 (UMG에서 바인딩)
}

void ASurvivalPlayerState::SetIsInfected(bool bInfected)
{
    if (HasAuthority())
    {
        bIsInfected = bInfected;
    }
}

void ASurvivalPlayerState::SetIsAlive(bool bAlive)
{
    if (HasAuthority())
    {
        bIsAlive = bAlive;
    }
}

bool ASurvivalPlayerState::AddItemToInventory(const FItemData& Item)
{
    if (!HasAuthority())
        return false;

    // 인벤토리 크기 체크
    if (Inventory.Num() >= MaxInventorySize)
    {
        UE_LOG(LogTemp, Warning, TEXT("Inventory Full!"));
        return false;
    }

    // 이미 존재하는 아이템인지 확인 (스택 가능)
    for (FItemData& ExistingItem : Inventory)
    {
        if (ExistingItem.ItemID == Item.ItemID)
        {
            ExistingItem.Quantity += Item.Quantity;
            OnRep_Inventory(); // 수동 호출 (서버)
            return true;
        }
    }

    // 새 아이템 추가
    Inventory.Add(Item);
    OnRep_Inventory(); // 수동 호출 (서버)

    return true;
}

bool ASurvivalPlayerState::RemoveItemFromInventory(FName ItemID, int32 Quantity)
{
    if (!HasAuthority())
        return false;

    for (int32 i = 0; i < Inventory.Num(); i++)
    {
        if (Inventory[i].ItemID == ItemID)
        {
            Inventory[i].Quantity -= Quantity;

            // 수량이 0 이하면 제거
            if (Inventory[i].Quantity <= 0)
            {
                Inventory.RemoveAt(i);
            }

            OnRep_Inventory(); // 수동 호출 (서버)
            return true;
        }
    }

    return false;
}

FItemData ASurvivalPlayerState::GetItemByID(FName ItemID) const
{
    for (const FItemData& Item : Inventory)
    {
        if (Item.ItemID == ItemID)
        {
            return Item;
        }
    }

    return FItemData();
}

bool ASurvivalPlayerState::HasItem(FName ItemID) const
{
    for (const FItemData& Item : Inventory)
    {
        if (Item.ItemID == ItemID)
        {
            return true;
        }
    }
    return false;
}
```

## 3. InGameHUD.h - HUD 위젯 (Blueprint Base)

```cpp
// InGameHUD.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameHUD.generated.h"

UCLASS()
class SURVIVALGAME_API UInGameHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // UI 업데이트 함수들 (Blueprint Implementable)
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void UpdateHealth(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void UpdateStamina(float CurrentStamina, float MaxStamina);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void UpdatePhaseInfo(EGamePhase CurrentPhase, float TimeRemaining);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void ShowInteractionPrompt(const FText& PromptText);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void HideInteractionPrompt();

    // 채팅 관련
    UFUNCTION(BlueprintCallable, Category = "Chat")
    void SendChatMessage(const FString& Message);

    UFUNCTION(BlueprintImplementableEvent, Category = "Chat")
    void OnChatMessageReceived(const FString& PlayerName, const FString& Message);

protected:
    // 캐릭터 레퍼런스
    UPROPERTY(BlueprintReadOnly, Category = "References")
    class ASurvivalCharacter* OwningCharacter;

    UPROPERTY(BlueprintReadOnly, Category = "References")
    class ASurvivalPlayerState* OwningPlayerState;

    UPROPERTY(BlueprintReadOnly, Category = "References")
    class ASurvivalGameState* GameState;

    // Attribute 변경 콜백 바인딩
    void BindAttributeCallbacks();

    UFUNCTION()
    void OnHealthChanged(const struct FOnAttributeChangeData& Data);

    UFUNCTION()
    void OnStaminaChanged(const struct FOnAttributeChangeData& Data);
};
```

## 4. InGameHUD.cpp

```cpp
// InGameHUD.cpp
#include "InGameHUD.h"
#include "SurvivalCharacter.h"
#include "SurvivalPlayerState.h"
#include "SurvivalGameState.h"
#include "CharacterAttributeSet.h"
#include "SurvivalAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

void UInGameHUD::NativeConstruct()
{
    Super::NativeConstruct();

    // 소유 캐릭터 가져오기
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        OwningCharacter = Cast<ASurvivalCharacter>(PC->GetPawn());
        OwningPlayerState = PC->GetPlayerState<ASurvivalPlayerState>();
    }

    // GameState 가져오기
    GameState = Cast<ASurvivalGameState>(UGameplayStatics::GetGameState(this));

    // Attribute 콜백 바인딩
    BindAttributeCallbacks();
}

void UInGameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 페이즈 정보 업데이트
    if (GameState)
    {
        UpdatePhaseInfo(GameState->CurrentPhase, GameState->PhaseTimeRemaining);
    }
}

void UInGameHUD::BindAttributeCallbacks()
{
    if (OwningPlayerState && OwningPlayerState->GetAbilitySystemComponent())
    {
        USurvivalAbilitySystemComponent* ASC = Cast<USurvivalAbilitySystemComponent>(
            OwningPlayerState->GetAbilitySystemComponent());

        UCharacterAttributeSet* Attributes = OwningPlayerState->AttributeSet;

        if (ASC && Attributes)
        {
            // 체력 변경 콜백
            ASC->GetGameplayAttributeValueChangeDelegate(Attributes->GetHealthAttribute())
                .AddUObject(this, &UInGameHUD::OnHealthChanged);

            // 스태미나 변경 콜백
            ASC->GetGameplayAttributeValueChangeDelegate(Attributes->GetStaminaAttribute())
                .AddUObject(this, &UInGameHUD::OnStaminaChanged);

            // 초기 값 설정
            UpdateHealth(Attributes->GetHealth(), Attributes->GetMaxHealth());
            UpdateStamina(Attributes->GetStamina(), Attributes->GetMaxStamina());
        }
    }
}

void UInGameHUD::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    if (OwningPlayerState && OwningPlayerState->AttributeSet)
    {
        UpdateHealth(Data.NewValue, OwningPlayerState->AttributeSet->GetMaxHealth());
    }
}

void UInGameHUD::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
    if (OwningPlayerState && OwningPlayerState->AttributeSet)
    {
        UpdateStamina(Data.NewValue, OwningPlayerState->AttributeSet->GetMaxStamina());
    }
}

void UInGameHUD::SendChatMessage(const FString& Message)
{
    // PlayerController를 통해 서버로 전송
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        // TODO: ServerRPC 호출하여 채팅 메시지 전송
    }
}
```

## 5. InventoryWidget.h - 인벤토리 UI

```cpp
// InventoryWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalPlayerState.h"
#include "InventoryWidget.generated.h"

UCLASS()
class SURVIVALGAME_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // 인벤토리 새로고침
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RefreshInventory();

    // Blueprint에서 구현할 함수
    UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
    void PopulateInventorySlots(const TArray<FItemData>& Items);

    // 아이템 사용
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UseItem(FName ItemID);

    // 아이템 드롭
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void DropItem(FName ItemID, int32 Quantity);

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    ASurvivalPlayerState* OwningPlayerState;

    // 서버 RPC
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerUseItem(FName ItemID);

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerDropItem(FName ItemID, int32 Quantity);
};
```

## 6. InventoryWidget.cpp

```cpp
// InventoryWidget.cpp
#include "InventoryWidget.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        OwningPlayerState = PC->GetPlayerState<ASurvivalPlayerState>();
    }

    RefreshInventory();
}

void UInventoryWidget::RefreshInventory()
{
    if (OwningPlayerState)
    {
        PopulateInventorySlots(OwningPlayerState->Inventory);
    }
}

void UInventoryWidget::UseItem(FName ItemID)
{
    if (OwningPlayerState)
    {
        ServerUseItem(ItemID);
    }
}

void UInventoryWidget::ServerUseItem_Implementation(FName ItemID)
{
    if (!OwningPlayerState)
        return;

    // 아이템 존재 확인
    if (!OwningPlayerState->HasItem(ItemID))
        return;

    // GAS Ability 활성화 또는 직접 효과 적용
    // TODO: 아이템별 로직 구현

    // 소모품이면 인벤토리에서 제거
    OwningPlayerState->RemoveItemFromInventory(ItemID, 1);
}

bool UInventoryWidget::ServerUseItem_Validate(FName ItemID)
{
    return true;
}

void UInventoryWidget::DropItem(FName ItemID, int32 Quantity)
{
    ServerDropItem(ItemID, Quantity);
}

void UInventoryWidget::ServerDropItem_Implementation(FName ItemID, int32 Quantity)
{
    if (!OwningPlayerState)
        return;

    if (!OwningPlayerState->HasItem(ItemID))
        return;

    // 인벤토리에서 제거
    OwningPlayerState->RemoveItemFromInventory(ItemID, Quantity);

    // 월드에 아이템 스폰
    // TODO: 아이템 액터 스폰 로직
}

bool UInventoryWidget::ServerDropItem_Validate(FName ItemID, int32 Quantity)
{
    return Quantity > 0;
}
```

## 7. ChatWidget.h - 채팅 시스템

```cpp
// ChatWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

USTRUCT(BlueprintType)
struct FChatMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString PlayerName;

    UPROPERTY(BlueprintReadWrite)
    FString Message;

    UPROPERTY(BlueprintReadWrite)
    FLinearColor TextColor;

    FChatMessage()
        : PlayerName(""), Message(""), TextColor(FLinearColor::White)
    {}
};

UCLASS()
class SURVIVALGAME_API UChatWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 채팅 메시지 전송
    UFUNCTION(BlueprintCallable, Category = "Chat")
    void SendMessage(const FString& Message);

    // 메시지 수신 (Blueprint에서 구현)
    UFUNCTION(BlueprintImplementableEvent, Category = "Chat")
    void ReceiveMessage(const FChatMessage& ChatMessage);

protected:
    // 최대 메시지 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat")
    int32 MaxMessages = 50;

    // 메시지 리스트
    UPROPERTY(BlueprintReadOnly, Category = "Chat")
    TArray<FChatMessage> Messages;

    // 서버 RPC
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSendMessage(const FString& Message);

    // 멀티캐스트 RPC
    UFUNCTION(NetMulticast, Reliable)
    void MulticastReceiveMessage(const FString& PlayerName, const FString& Message);
};
```

---

## Blueprint 예시 (UMG)

### InGameHUD_WBP (Widget Blueprint)
```
Canvas Panel
├── HealthBar (ProgressBar)
├── StaminaBar (ProgressBar)
├── AmmoText (TextBlock)
├── PhaseInfo (TextBlock)
├── InteractionPrompt (TextBlock) - Visibility: Collapsed
├── ChatWidget (User Created Widget)
└── InventoryButton (Button)
```

### InventoryWidget_WBP
```
Canvas Panel
├── Background (Image)
├── InventoryGrid (UniformGridPanel - 5x4)
│   └── InventorySlot_WBP (여러 개, 동적 생성)
└── CloseButton (Button)
```

### InventorySlot_WBP
```
Border
├── ItemIcon (Image)
├── ItemName (TextBlock)
└── ItemQuantity (TextBlock)
```

---

이 코드들은 **김세훈(InGame UI), 조기혁(InGame UI), 김현수(OutGame UI)** 학생들이 작업할 내용입니다!
