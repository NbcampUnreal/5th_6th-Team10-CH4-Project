// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::SetItemData(const FInventoryItemData& ItemData, int32 StackCount)
{
    // 1. 아이콘 업데이트
    if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(ItemData.ItemIcon);
        ItemIcon->SetVisibility(ItemData.ItemIcon ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }

    // 2. 수량(스택) 텍스트 업데이트
    if (StackCountText)
    {
        if (StackCount > 1)
        {
            StackCountText->SetText(FText::AsNumber(StackCount));
            StackCountText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            // 수량이 0, 1 개일 경우 텍스트 표시x
            StackCountText->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void UInventorySlotWidget::SetSlotIndex(int32 Index)
{
    SlotIndex = Index;
}
