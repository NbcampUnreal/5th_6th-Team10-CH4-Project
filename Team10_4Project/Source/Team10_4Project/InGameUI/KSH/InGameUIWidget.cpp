// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/InGameUIWidget.h"
#include "Components/ProgressBar.h"
#include "InGameUI/KSH/HealthBarWidget.h" // 체력바
#include "InGameUI/KSH/StaminaBarWidget.h" // 스테미나바
#include "AbilitySystemComponent.h" // ASC 사용을 위해 포함

void UInGameUIWidget::InitializeUI(UAbilitySystemComponent* ASC)
{
    if (!ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("InGameUIWidget: ASC is null. Cannot initialize UI."));
        return;
    }

    // 1. HealthBar 초기화
    if (HealthBar)
    {
        HealthBar->InitWithASC(ASC);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InGameUIWidget: HealthBar is not bound or invalid."));
    }

    // 2. StaminaBar 초기화
    if (StaminaBar)
    {
        StaminaBar->InitWithASC(ASC);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InGameUIWidget: StaminaBar is not bound or invalid."));
    }
}

bool UInGameUIWidget::Initialize()
{
    // UMG에서 위젯이 생성될 때 호출되며, 위젯 변수(HealthBar, StaminaBar)가 유효한지 확인
    bool Success = Super::Initialize();
    if (!Success) return false;

    return true;
}
