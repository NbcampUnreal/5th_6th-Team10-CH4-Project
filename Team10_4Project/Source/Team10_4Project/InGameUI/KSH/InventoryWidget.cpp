// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/InventoryWidget.h"
#include "InGameUI/KSH/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "InventoryComponent.h" // 인벤토리 컴포넌트

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInventoryWidget::NativeDestruct()
{
    Super::NativeDestruct();
}

void UInventoryWidget::InitWithInventory(UInventoryComponent* InventoryComp)
{
    if (!InventoryComp) return;

    InventoryComponent = InventoryComp;

    // 1. 컴포넌트에 설정된 최대 슬롯 수만큼 UI 슬롯 생성
    CreateSlots(InventoryComp->GetMaxSlotCount());

    // 2. 델리게이트 연결
    InventoryComponent->OnInventoryChanged.AddUObject(this, &UInventoryWidget::OnInventoryDataChanged);

    // 3. 초기 데이터 반영
    OnInventoryDataChanged();
}

void UInventoryWidget::CreateSlots(int32 NumSlots)
{
    if (!SlotContainer || !SlotWidgetClass) return;

    SlotWidgets.Empty();
    SlotContainer->ClearChildren();

    const int32 Columns = 5; // 한 줄에 5칸씩 배치

    for (int32 i = 0; i < NumSlots; ++i)
    {
        // 슬롯 위젯 인스턴스 생성
        UInventorySlotWidget* NewSlot = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
        if (NewSlot)
        {
            NewSlot->SetSlotIndex(i);

            // 그리드 패널에 추가 (행, 열 계산)
            SlotContainer->AddChildToUniformGrid(NewSlot, i / Columns, i % Columns);

            // 관리를 위해 배열에 저장
            SlotWidgets.Add(NewSlot);
        }
    }
}

void UInventoryWidget::OnInventoryDataChanged()
{
    if (!InventoryComponent) return;

    // 컴포넌트로부터 실제 슬롯 데이터 배열 가져옴
    const TArray<FInventorySlot>& Slots = InventoryComponent->GetInventorySlots();

    for (int32 i = 0; i < SlotWidgets.Num(); ++i)
    {
        // UI 슬롯 인덱스가 데이터 범위 안에 있는지 확인 후 데이터를 전달
        if (Slots.IsValidIndex(i))
        {
            SlotWidgets[i]->SetItemData(Slots[i].ItemData, Slots[i].CurrentStack);
        }
    }
}