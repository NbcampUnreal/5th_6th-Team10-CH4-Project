// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/InventoryWidget.h"
#include "InGameUI/KSH/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
//#include "InventoryComponent.h" 인벤토리 컴포넌트

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInventoryWidget::NativeDestruct()
{
    Super::NativeDestruct();
}

//void UInventoryWidget::InitWithInventory(UInventoryComponent* InventoryComp)
//{
//    if (!InventoryComp) return;
//
//    InventoryComponent = InventoryComp;
//
//    // 1. 컴포넌트에 설정된 최대 슬롯 수만큼 UI 슬롯 생성
//    CreateSlots(InventoryComp->GetMaxSlotCount());
//
//    // 2. 초기 데이터 반영
//    OnInventoryDataChanged();
//}

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
    //if (!InventoryComponent) return;

    //// 컴포넌트에서 TArray<FInventoryItemData> 등을 받아와야 함
    //// 예시 루프:
    //for (int32 i = 0; i < SlotWidgets.Num(); ++i)
    //{
    //    // InventoryComponent->GetItemAt(i) 같은 함수로 데이터를 가져와서
    //    // SlotWidgets[i]->SetItemData(가져온데이터, 수량); 을 호출
    //}
}