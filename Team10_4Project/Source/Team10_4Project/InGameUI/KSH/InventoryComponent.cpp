// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
    // 멀티플레이어 환경일 경우 컴포넌트 복제 활성화
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // 시작할 때 인벤토리 칸을 미리 만들어 둠
    InventorySlots.SetNum(MaxSlotCount);
}

bool UInventoryComponent::AddItem(const FInventoryItemData& NewItem, int32 Count)
{
    // 1. 이미 같은 아이템이 있는지 확인 (Stackable일 경우)
    if (NewItem.bIsStackable)
    {
        for (FInventorySlot& Slot : InventorySlots)
        {
            if (Slot.ItemData.ItemID == NewItem.ItemID)
            {
                Slot.CurrentStack += Count;
                OnInventoryChanged.Broadcast(); // UI에 알림
                return true;
            }
        }
    }

    // 2. 빈 공간 찾아서 넣기
    for (FInventorySlot& Slot : InventorySlots)
    {
        if (Slot.IsEmpty())
        {
            Slot.ItemData = NewItem;
            Slot.CurrentStack = Count;
            OnInventoryChanged.Broadcast(); // UI에 알림
            return true;
        }
    }

    return false; // 인벤토리가 가득 참
}

void UInventoryComponent::AddItemByID(FName ItemID, int32 Count)
{
    if (!ItemDataTable) return; // 데이터 테이블이 에디터에서 할당되어 있어야 함

    // 1. ID를 가지고 데이터 테이블에서 구조체 정보를 찾음
    static const FString ContextString(TEXT("Item Search"));
    FInventoryItemData* FoundData = ItemDataTable->FindRow<FInventoryItemData>(ItemID, ContextString);

    if (FoundData)
    {
        // 2. 찾은 구조체를 기존 AddItem 함수에 그대로 전달
        AddItem(*FoundData, Count);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ID [%s] 에 해당하는 아이템을 찾을 수 없습니다!"), *ItemID.ToString());
    }
}

void UInventoryComponent::RemoveItem(int32 SlotIndex)
{
    if (InventorySlots.IsValidIndex(SlotIndex))
    {
        InventorySlots[SlotIndex].Clear();
        OnInventoryChanged.Broadcast(); // UI에 알림
    }
}

void UInventoryComponent::UseItem(int32 SlotIndex)
{
    // 서버에서만 실행
    if (GetOwnerRole() < ROLE_Authority) return;

    if (InventorySlots.IsValidIndex(SlotIndex) && !InventorySlots[SlotIndex].IsEmpty())
    {
        // 1. 아이템 효과 적용 로직
        UE_LOG(LogTemp, Warning, TEXT("%d번 슬롯 아이템 사용!"), SlotIndex + 1);

        // 2. 아이템 수량 감소
        InventorySlots[SlotIndex].CurrentStack--;

        // 3. 수량이 0이 되면 슬롯 비우기
        if (InventorySlots[SlotIndex].CurrentStack <= 0)
        {
            InventorySlots[SlotIndex].Clear();
        }

        // 4. 데이터가 변했으므로 UI에 알림 (델리게이트 호출)
        OnInventoryChanged.Broadcast();
    }
}

void UInventoryComponent::OnRep_InventorySlots()
{
    // 서버에서 데이터가 복제되어 클라이언트에 도착하면 UI 갱신
    OnInventoryChanged.Broadcast();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // InventorySlots 배열이 네트워크를 통해 복제되도록 설정
    DOREPLIFETIME(UInventoryComponent, InventorySlots);
}