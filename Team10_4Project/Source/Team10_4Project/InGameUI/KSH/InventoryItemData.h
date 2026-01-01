// InventoryItemData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "InventoryItemData.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItemData : public FTableRowBase
{
    GENERATED_BODY()

public:
    // 생성자를 통해 기본값 안전하게 설정
    FInventoryItemData()
        : ItemID(NAME_None)
        , ItemName(FText::GetEmpty())
        , ItemDescription(FText::GetEmpty())
        , ItemIcon(nullptr)
        , bIsStackable(true)
        , MaxStackSize(100)
    {
    }

    // 아이템 고유 식별자 (로직용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemID;

    // UI 표시용 정보
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TObjectPtr<UTexture2D> ItemIcon;

    // 수량 및 스택 규칙
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool bIsStackable;

    // 스택 가능할 때만 에디터에서 수정 가능하도록 제약
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (EditCondition = "bIsStackable", ClampMin = 1))
    int32 MaxStackSize;

    // GAS 연동을 위한 태그 (예: Item.Type.Potion)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FGameplayTag ItemTag;

    // 사용 시 적용할 이펙트 (필요 시 사용)
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    // TSubclassOf<class UGameplayEffect> ItemEffect;
};