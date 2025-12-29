// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/SanityBarWidget.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GameAttributeTypes.h" 

void USanityBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void USanityBarWidget::NativeDestruct()
{
    // Delegate Unbind (UI 파괴될 때 크래시 방지)
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent
            ->GetGameplayAttributeValueChangeDelegate(GET_SANITY_ATTRIBUTE)
            .Remove(SanityChangedHandle);
    }

    Super::NativeDestruct();
}

void USanityBarWidget::InitWithASC(UAbilitySystemComponent* ASC)
{
    if (!ASC) return;

    AbilitySystemComponent = ASC;

    BindGASDelegates(ASC);

    // 위젯 초기 표시값 설정
    float CurrentSanity = ASC->GetNumericAttribute(GET_SANITY_ATTRIBUTE);

    UpdateSanityBar(CurrentSanity);
}

void USanityBarWidget::BindGASDelegates(UAbilitySystemComponent* ASC)
{
    // Sanity 변경 감지
    SanityChangedHandle =
        ASC->GetGameplayAttributeValueChangeDelegate(GET_SANITY_ATTRIBUTE)
        .AddUObject(this, &USanityBarWidget::OnSanityAttributeChanged);
}

void USanityBarWidget::OnSanityAttributeChanged(const FOnAttributeChangeData& Data)
{
    if (!AbilitySystemComponent) return;

    UpdateSanityBar(Data.NewValue);
}

void USanityBarWidget::UpdateSanityBar(float CurrentSanity)
{
    if (SanityBar)
    {
        // Sanity는 Max 수치가 따로 없으므로 100.0f를 기준으로 비율을 계산합니다.
        float SanityPercent = FMath::Clamp(CurrentSanity / 100.0f, 0.0f, 1.0f);
        SanityBar->SetPercent(SanityPercent);
    }
}