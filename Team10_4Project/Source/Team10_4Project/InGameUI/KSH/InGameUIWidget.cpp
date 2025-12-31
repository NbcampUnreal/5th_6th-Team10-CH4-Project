// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/InGameUIWidget.h"
#include "Components/ProgressBar.h"
#include "InGameUI/KSH/HealthBarWidget.h" // 체력바
#include "InGameUI/KSH/SanityBarWidget.h" // 감염도바
#include "InGameUI/KSH/InventoryWidget.h" // 인벤토리
#include "InGameUI/KSH/AmmoWidget.h" // 탄약
#include "AbilitySystemComponent.h" // ASC 사용을 위해 포함
#include "InGameUI/KSH/InventoryComponent.h" // 인벤토리컴포넌트

void UInGameUIWidget::InitializeUI(UAbilitySystemComponent* ASC)
{
    if (!ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("InGameUIWidget: ASC is null. Cannot initialize UI."));
        return;
    }
    UE_LOG(LogTemp, Error, TEXT("InGameUIWidget: ASC."));

    // 1. HealthBar 초기화
    if (HealthBar) HealthBar->InitWithASC(ASC);
    // 2. SanityBar 초기화
    if (SanityBar) SanityBar->InitWithASC(ASC);
    // 3. 인벤토리 초기화
    if (Inventory)
    {
        // ASC를 소유한 Owner(캐릭터)로부터 인벤토리 컴포넌트를 찾아 가져오기
        AActor* Owner = ASC->GetOwner();
        if (Owner)
        {
            UE_LOG(LogTemp, Warning, TEXT("InGameUIWidget: Searching Inventory on Actor: %s"), *Owner->GetName());
            UInventoryComponent* InvComp = Owner->FindComponentByClass<UInventoryComponent>();
            if (InvComp)
            {
                Inventory->InitWithInventory(InvComp);
                UE_LOG(LogTemp, Warning, TEXT("InGameUIWidget: InventoryComponent Found!"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("InGameUIWidget: InventoryComponent NOT Found on Owner!"));
            }
        }
    }
    if (AmmoWidget) AmmoWidget->InitWithASC(ASC);
}

bool UInGameUIWidget::Initialize()
{
    // UMG에서 위젯이 생성될 때 호출되며, 위젯 변수(HealthBar, SanityBar)가 유효한지 확인
    bool Success = Super::Initialize();
    if (!Success) return false;

    return true;
}
