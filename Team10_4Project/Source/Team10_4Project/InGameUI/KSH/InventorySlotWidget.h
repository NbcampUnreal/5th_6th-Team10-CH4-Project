// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemData.h"
#include "InventorySlotWidget.generated.h"

// 전방 선언
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // 슬롯 UI 업데이트 (표시할 아이템 정보, 수량)
    void SetItemData(const FInventoryItemData& ItemData, int32 StackCount);
    // 해당 슬롯이 인벤토리의 몇 번째 칸인지 저장
    void SetSlotIndex(int32 Index);

protected:
    // 아이템 아이콘
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemIcon;
    // 아이템 수량 텍스트
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> StackCountText;

private:
    // 슬롯 번호
    int32 SlotIndex = INDEX_NONE;
	
};
