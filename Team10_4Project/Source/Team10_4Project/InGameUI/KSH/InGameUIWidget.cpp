// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/InGameUIWidget.h"
#include "Components/ProgressBar.h"
#include "InGameUI/KSH/HealthBarWidget.h" // 체력바
#include "InGameUI/KSH/StaminaBarWidget.h" // 스테미나바
#include "InGameUI/KSH/InventoryWidget.h" // 인벤토리
#include "AbilitySystemComponent.h" // ASC 사용을 위해 포함

void UInGameUIWidget::InitializeUI(UAbilitySystemComponent* ASC)
{
    if (!ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("InGameUIWidget: ASC is null. Cannot initialize UI."));
        return;
    }

    // 1. HealthBar 초기화
    if (HealthBar) HealthBar->InitWithASC(ASC);
    // 2. StaminaBar 초기화
    if (StaminaBar) StaminaBar->InitWithASC(ASC);
    // 3. 인벤토리 초기화
    if (Inventory)
    {
        // ASC를 소유한 Owner(캐릭터)로부터 인벤토리 컴포넌트를 찾아 가져오기
        AActor* Owner = ASC->GetOwner();
        if (Owner)
        {
           /* UInventoryComponent* InvComp = Owner->FindComponentByClass<UInventoryComponent>();
            if (InvComp)
            {
                Inventory->InitWithInventory(InvComp);
            }*/
        }
    }
}

bool UInGameUIWidget::Initialize()
{
    // UMG에서 위젯이 생성될 때 호출되며, 위젯 변수(HealthBar, StaminaBar)가 유효한지 확인
    bool Success = Super::Initialize();
    if (!Success) return false;

    return true;
}
