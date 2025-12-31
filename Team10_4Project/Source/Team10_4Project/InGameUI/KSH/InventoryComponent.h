// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryItemData.h"
#include "InventoryComponent.generated.h"

// 데이터가 변경되었음을 UI에 알리기 위한 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged);

// 인벤토리의 한 칸을 구성하는 구조체
USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FInventoryItemData ItemData; // 아이템 정보

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentStack = 0;      // 현재 수량

    // 슬롯이 비어있는지 확인하는 함수
    bool IsEmpty() const { return ItemData.ItemID.IsNone() || CurrentStack <= 0; }

    // 슬롯 초기화
    void Clear()
    {
        ItemData = FInventoryItemData();
        CurrentStack = 0;
    }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM10_4PROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

    // --- 인벤토리 기능 ---

    // 아이템 추가
    bool AddItem(const FInventoryItemData& NewItem, int32 Count = 1);
    // =====테스트용=====
    void AddItemByID(FName ItemID, int32 Count = 1);
    // 에디터의 Details 패널에서 아이템 데이터 테이블을 선택
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TObjectPtr<UDataTable> ItemDataTable;
    // 특정 슬롯의 아이템 삭제
    void RemoveItem(int32 SlotIndex);
    // 특정 인덱스의 아이템 사용
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UseItem(int32 SlotIndex);
    // UI에서 전체 슬롯 정보를 가져갈 때 사용
    const TArray<FInventorySlot>& GetInventorySlots() const { return InventorySlots; }
    // 최대 슬롯 수 반환
    int32 GetMaxSlotCount() const { return MaxSlotCount; }

    // 데이터 변경 알림 델리게이트
    FOnInventoryChanged OnInventoryChanged;

protected:
    virtual void BeginPlay() override;

    // 최대 인벤토리 크기
    UPROPERTY(EditAnywhere, Category = "Inventory")
    int32 MaxSlotCount = 20;
    // 실제 아이템들이 담기는 배열
    UPROPERTY(ReplicatedUsing = OnRep_InventorySlots)
    TArray<FInventorySlot> InventorySlots;

    // 네트워크 복제 시 UI 갱신
    UFUNCTION()
    void OnRep_InventorySlots();

    // 네트워크 관련 설정
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
