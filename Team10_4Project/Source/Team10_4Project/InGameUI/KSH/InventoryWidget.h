// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemData.h"
#include "InventoryWidget.generated.h"

// 전방 선언
class UInventorySlotWidget;
class UUniformGridPanel;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 인벤토리 데이터를 가진 컴포넌트를 연결
    //void InitWithInventory(UInventoryComponent* InventoryComp);

protected:
    // 위젯 생성 시 호출 (초기화)
    virtual void NativeConstruct() override;
    // 위젯 파괴 시 호출 (메모리 관리)
    virtual void NativeDestruct() override;

    // 슬롯들이 들어갈 그리드 패널 (UMG에서 이름 맞출 것)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> SlotContainer;
    // 에디터에서 '어떤 슬롯 위젯 블루프린트'를 사용할지 지정하는 변수
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

private:
    // 연결된 실제 데이터 소스
    //UPROPERTY()
    //TObjectPtr<UInventoryComponent> InventoryComponent;
    // 생성된 슬롯 인스턴스들을 담아두는 배열 (데이터 갱신 시 순회용)
    UPROPERTY()
    TArray<TObjectPtr<UInventorySlotWidget>> SlotWidgets;

    // 데이터가 변경되었을 때 전체 슬롯 UI를 다시 그리는 함수
    void OnInventoryDataChanged();
    // 초기 설정 시 슬롯 개수만큼 UI를 미리 생성하는 함수
    void CreateSlots(int32 NumSlots);
};
